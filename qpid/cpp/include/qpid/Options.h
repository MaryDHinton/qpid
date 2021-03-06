#ifndef QPID_COMMONOPTIONS_H
#define QPID_COMMONOPTIONS_H

/*
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */

#include "qpid/Exception.h"

// Disable warnings triggered by boost.
#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4251 4275)
#endif

#include <boost/program_options.hpp>
#include <boost/format.hpp>

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

#include <sstream>
#include <iterator>
#include <algorithm>
#include <string>
#include "qpid/CommonImportExport.h"

namespace qpid {
namespace po=boost::program_options;



///@internal
QPID_COMMON_EXTERN std::string prettyArg(const std::string&, const std::string&);

/** @internal Normally only constructed by optValue() */
template <class T>
class OptionValue : public po::typed_value<T> {
  public:
    OptionValue(T& value, const std::string& arg)
        : po::typed_value<T>(&value), argName(arg) {}
    std::string name() const { return argName; }

  private:
    std::string argName;
};


/** Create an option value.
 * name, value appear after the option name in help like this:
 * <name> (=<value>)
 * T must support operator <<.
 *@see Options for example of use.
 */
template<class T>
po::value_semantic* optValue(T& value, const char* name) {
    std::string valstr(boost::lexical_cast<std::string>(value));
    return new OptionValue<T>(value, prettyArg(name, valstr));
}

/** Create a vector value. Multiple occurences of the option are
 * accumulated into the vector
 */
template <class T>
po::value_semantic* optValue(std::vector<T>& value, const char* name) {
    std::ostringstream os;
    std::copy(value.begin(), value.end(), std::ostream_iterator<T>(os, " "));
    std::string val=os.str();
    if (!val.empty())
        val.erase(val.end()-1); // Remove trailing " "
    return (new OptionValue<std::vector<T> >(value, prettyArg(name, val)));
}

/** Create a boolean switch value. Presence of the option sets the value. */
inline po::value_semantic* optValue(bool& value) { return po::bool_switch(&value); }

/**
 * Base class for options.
 * Example of use:
 @code
 struct MySubOptions : public Options {
   int x;
   string y;
   MySubOptions() : Options("Sub options") {
     addOptions()
     ("x", optValue(x,"XUNIT"), "Option X")
     ("y", optValue(y, "YUNIT"), "Option Y");
   }
 };

 struct MyOptions : public Options {
   bool z;
   vector<string> foo;
   MySubOptions subOptions;
   MyOptions() : Options("My Options") {
    addOptions()
      ("z", boolSwitch(z), "Option Z")
      ("foo", optValue(foo), "Multiple option foo");
    add(subOptions);
 }

 main(int argc, char** argv) {
   Options opts;
   opts.parse(argc, char** argv);
   // Use values
   dosomething(opts.subOptions.x);
   if (error)
     cout << opts << end;       // Help message.
 }

 @endcode
 */




struct Options : public po::options_description {

    struct Exception : public qpid::Exception {
        Exception(const std::string& msg) : qpid::Exception(msg) {}
    };

    QPID_COMMON_EXTERN Options(const std::string& name=std::string());

    /**
     * Parses options from argc/argv, environment variables and config file.
     * Note the filename argument can reference an options variable that
     * is updated by argc/argv or environment variable parsing.
     */
    QPID_COMMON_EXTERN void parse(int argc, char const* const* argv,
               const std::string& configfile=std::string(),
               bool  allowUnknown = false);

    /**
     * Tests for presence of argc/argv switch
     */
    QPID_COMMON_EXTERN bool findArg(int argc, char const* const* argv,
                                       const std::string& theArg);

  boost::program_options::options_description_easy_init addOptions() {
      return add_options();
  }
};



/**
 * Standard options for configuration
 */
struct CommonOptions : public Options {
    QPID_COMMON_EXTERN CommonOptions(const std::string& name=std::string(),
                                     const std::string& configfile=std::string(),
                                     const std::string& clientConfigFile=std::string());
    bool help;
    bool version;
    std::string config;
    std::string clientConfig;
};




} // namespace qpid

#endif  /*!QPID_COMMONOPTIONS_H*/
