#ifndef QPID_ACL_ACLDATA_H
#define QPID_ACL_ACLDATA_H


/*
 *
 * Copyright (c) 2006 The Apache Software Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "qpid/broker/AclModule.h"
#include "AclTopicMatch.h"
#include "qpid/log/Statement.h"
#include "boost/shared_ptr.hpp"
#include <vector>
#include <sstream>

namespace qpid {
namespace acl {

class AclData {


public:

    typedef std::map<qpid::acl::Property, std::string> propertyMap;
    typedef propertyMap::const_iterator                propertyMapItr;

    typedef std::map<qpid::acl::SpecProperty, std::string> specPropertyMap;
    typedef specPropertyMap::const_iterator                specPropertyMapItr;

    //
    // rule
    //
    // Created by AclReader and stored in a ruleSet vector for subsequent
    //  run-time lookup matching and allow/deny decisions.
    // RuleSet vectors are indexed by Action-Object-actorId so these
    //  attributes are not part of a rule.
    // A single ACL file entry may create many rule entries in
    //  many ruleset vectors.
    //
    struct Rule {
        typedef broker::TopicExchange::TopicExchangeTester topicTester;

        int                   rawRuleNum;   // rule number in ACL file
        qpid::acl::AclResult  ruleMode;     // combined allow/deny log/nolog
        specPropertyMap       props;        //
        bool                  pubRoutingKeyInRule;
        std::string           pubRoutingKey;
        boost::shared_ptr<topicTester> pTTest;
        bool                  pubExchNameInRule;
        std::string           pubExchName;
        std::vector<bool>     ruleHasUserSub;

        Rule (int ruleNum, qpid::acl::AclResult res, specPropertyMap& p) :
            rawRuleNum(ruleNum),
            ruleMode(res),
            props(p),
            pubRoutingKeyInRule(false),
            pubRoutingKey(),
            pTTest(boost::shared_ptr<topicTester>(new topicTester())),
            pubExchNameInRule(false),
            pubExchName(),
            ruleHasUserSub(PROPERTYSIZE, false)
            {}


        std::string toString () const {
            std::ostringstream ruleStr;
            ruleStr << "[rule " << rawRuleNum
                    << " ruleMode = " << AclHelper::getAclResultStr(ruleMode)
                    << " props{";
            for (specPropertyMapItr pMItr  = props.begin();
                                    pMItr != props.end();
                                    pMItr++) {
                ruleStr << " "
                        << AclHelper::getPropertyStr((SpecProperty) pMItr-> first)
                        << "=" << pMItr->second;
            }
            ruleStr << " }]";
            return ruleStr.str();
        }

        void addTopicTest(const std::string& pattern) {
            pTTest->addBindingKey(broker::TopicExchange::normalize(pattern));
        }

        // Topic Exchange tester
        // return true if any bindings match 'pattern'
        bool matchRoutingKey(const std::string& pattern) const
        {
            topicTester::BindingVec bv;
            return pTTest->findMatches(pattern, bv);
        }
    };

    typedef  std::vector<Rule>               ruleSet;
    typedef  ruleSet::const_iterator         ruleSetItr;
    typedef  std::map<std::string, ruleSet > actionObject; // user
    typedef  actionObject::iterator          actObjItr;
    typedef  actionObject*                   aclAction;

    // Action*[] -> Object*[] -> map<user -> set<Rule> >
    aclAction*           actionList[qpid::acl::ACTIONSIZE];
    qpid::acl::AclResult decisionMode;  // allow/deny[-log] if no matching rule found
    bool                 transferAcl;
    std::string          aclSource;

    AclResult lookup(
        const std::string&               id,        // actor id
        const Action&                    action,
        const ObjectType&                objType,
        const std::string&               name,      // object name
        std::map<Property, std::string>* params=0);

    AclResult lookup(
        const std::string&               id,        // actor id
        const Action&                    action,
        const ObjectType&                objType,
        const std::string&               ExchangeName,
        const std::string&               RoutingKey);

    bool matchProp(const std::string & src, const std::string& src1);
    void clear ();
    static const std::string USER_SUBSTITUTION_KEYWORD;
    static const std::string DOMAIN_SUBSTITUTION_KEYWORD;
    static const std::string USERDOMAIN_SUBSTITUTION_KEYWORD;
    void substituteString(std::string& targetString,
                          const std::string& placeholder,
                          const std::string& replacement);
    std::string normalizeUserId(const std::string& userId);
    void substituteUserId(std::string& ruleString,
                          const std::string& userId);
    void substituteKeywords(std::string& ruleString,
                            const std::string& userId);

    AclData();
    virtual ~AclData();

private:
    bool compareIntMax(const qpid::acl::SpecProperty theProperty,
                       const std::string             theAclValue,
                       const std::string             theLookupValue);

    bool compareIntMin(const qpid::acl::SpecProperty theProperty,
                       const std::string             theAclValue,
                       const std::string             theLookupValue);
};

}} // namespace qpid::acl

#endif // QPID_ACL_ACLDATA_H
