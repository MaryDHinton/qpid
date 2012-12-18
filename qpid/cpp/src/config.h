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

/*
 * This file is automatically generated and will be overwritten by the
 * next CMake invocation.
 */

#ifndef QPID_CONFIG_H
#define QPID_CONFIG_H

// PACKAGE_NAME and PACKAGE_VERSION are carry-overs from the autoconf world.
// They tend to cause confusion and problems when mixing headers from multiple
// autoconf-configured packages, so it's best to remove these in favor of
// Qpid-specific names as soon as the autoconf stuff is removed.
#define PACKAGE_NAME "qpid-cpp"
#define PACKAGE_VERSION "0.21"

#define QPIDC_CONF_FILE "conf/qpidc.conf"
#define QPIDD_CONF_FILE "conf/qpidd.conf"

#define QPIDC_MODULE_DIR "plugins/client"
#define QPIDD_MODULE_DIR "plugins/broker"

#define QPID_SHLIB_PREFIX ""
#define QPID_MODULE_PREFIX
/* #undef QPID_DEBUG_POSTFIX */
#if defined(QPID_DEBUG_POSTFIX) && defined (_DEBUG)
#  define QPID_SHLIB_POSTFIX QPID_DEBUG_POSTFIX
#  define QPID_MODULE_POSTFIX QPID_DEBUG_POSTFIX
#else
#  define QPID_SHLIB_POSTFIX
#  define QPID_MODULE_POSTFIX
#endif
#define QPID_SHLIB_SUFFIX ".dll"
#define QPID_MODULE_SUFFIX ".dll"

/* #undef QPID_HAS_CLOCK_GETTIME */

/* #undef BROKER_SASL_NAME */
/* #undef HAVE_SASL */

/* #undef HAVE_OPENAIS_CPG_H */
/* #undef HAVE_COROSYNC_CPG_H */
/* #undef HAVE_LIBCMAN_H */
/* #undef HAVE_SYS_SDT_H */
/* #undef HAVE_LOG_AUTHPRIV */
/* #undef HAVE_LOG_FTP */

#endif /* QPID_CONFIG_H */
