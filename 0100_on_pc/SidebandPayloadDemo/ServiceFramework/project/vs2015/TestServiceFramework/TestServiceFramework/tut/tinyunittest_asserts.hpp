/*! \file tinyunittest_asserts.hpp
    \brief minimal unit test framework optimized for developer,
    "native implementation".

    Copyright  ©2006-2006 Siemens VDO

    See the file \c tinyunittest.hpp for details.

    This header declares the parts of the Tiny Unit Test
    framework that are used for checks and assertions.
    This might be useful to speed up compiling time,
    or when the check-functions of the framework are
    used for assertions in whole applications.

    When you want to define test cases, test suites
    or unit tests, include "tut/tinyunittest.hpp" instead.
    That file includes this file.

    The user of this header file is supposed to use only
    a few classes, functions, and macros:

    macro \c TUT_WARN(chk)
    macro \c TUT_CHECK(chk)
    macro \c TUT_REQUIRE(chk)

    macro \c TUT_WARN_MSG(chk, msg)
    macro \c TUT_CHECK_MSG(chk, msg)
    macro \c TUT_REQUIRE_MSG(chk, msg)

    function \c Ntut::warn()
    function \c Ntut::check()
    function \c Ntut::require()

    All other stuff declared in this header file (or in
    accompanying implementation files) are to be treated
    as implementation details and not to be used at all.

    Note: At least at the moment, this complete file does
    not provide any implementation details. Thus it should
    be shared and re-used for all implementations of
    the Tiny Unit Test framework. Of course, each implementation
    has to implement the three functions \c warn(), \c check(),
    and \c require() in an implementation specific way.

\par History
\verbatim
(ks) = Kurt Stege, kurt.stege@siemens.com

2006-01-16 (ks):    Project created.
2006-02-16 (ks):    File extracted from tinyunittest.hpp.
\endverbatim
*/

#ifndef Ntut_tinyunittest_asserts_HPP_INCLUDED
#define Ntut_tinyunittest_asserts_HPP_INCLUDED

namespace Ntut {


/*
    Note: The functions Ntut::warn, Ntut::check and Ntut::require
    should be declared exactly in this form within all implementations
    of the Tiny Unit Test framework. Of course, the implementations
    may vary.
*/

void warn (bool chk, const char *file, int line, const char *msg=0);
void check (bool chk, const char *file, int line, const char *msg=0);
void require (bool chk, const char *file, int line, const char *msg=0);

} // namespace Ntut


/*
    Note: The implementation of the macros TUT_WARN, TUT_WARN_MSG,
    TUT_CHECK, TUT_CHECK_MSG, TUT_REQUIRE, and TUT_REQUIRE_MSG
    should be the same for all implementations of the Tiny Unit
    Test framework.

    The developer of a test case assumes that there is no difference
    between calling the function \c Ntut::check() directly or use
    the macro TUT_CHECK() instead.

    So, please change the implementation only when you have a
    good reason for it.
*/

/*!
    The macro TUT_WARN is used to check a boolean condition \c chk.
    When this condition is false, a warning is generated that is
    _not_ counted as error and will not result in a failure of the test.

    \see Ntut::warn() for further details.

    \see TUT_CHECK for the typically used version of checking test results.
    \see TUT_REQUIRE for another version of checking test results.
*/
#define TUT_WARN(chk) do { Ntut::warn((chk), __FILE__, __LINE__, #chk); } while (0)

/*!
    The macro TUT_WARN_MSG is similar to \c TUT_WARN but accepts
    a user defined message.

    \see TUT_CHECK_MSG()
*/
#define TUT_WARN_MSG(chk, msg) do { Ntut::warn((chk), __FILE__, __LINE__, msg); } while (0)

/*!
    The macro TUT_CHECK is used to check a boolean condition \c chk.
    When this condition is false, an error message is generated that
    will result in a failure of the test. The test case still runs
    further.

    \see Ntut::check() for further details.

    \see TUT_WARN for another version of checking test results.
    \see TUT_REQUIRE for another version of checking test results.
*/
#define TUT_CHECK(chk) do { Ntut::check((chk), __FILE__, __LINE__, #chk); } while (0)

/*!
    The macro TUT_CHECK_MSG is similar to \c TUT_CHECK but accepts
    a user defined message. \c TUT_CHECK takes the checked condition
    as message. This is useful when the check itself does not
    contain any information.

    Example: TUT_CHECK_MSG(false, "The tested object called sqrt() with a negativ number.");
*/
#define TUT_CHECK_MSG(chk,msg) do { Ntut::check((chk), __FILE__, __LINE__, msg); } while (0)

/*!
    The macro TUT_REQUIRE is used to check a boolean condition \c chk.
    When this condition is false, an error message is generated that
    will result in a failure of the test. Furthermore, the test case
    will not run any further and will be aborted immediately.
    However, other test cases will continue and are not aborted.

    \see Ntut::require() for further details.

    \see TUT_WARN for another version of checking test results.
    \see TUT_CHECK for the typically used version of checking test results.
*/
#define TUT_REQUIRE(chk) do { Ntut::require((chk), __FILE__, __LINE__, #chk); } while (0)

/*!
    The macro TUT_REQUIRE_MSG is similar to \c TUT_REQUIRE but accepts
    a user defined message.

    \see TUT_CHECK_MSG()
*/

#define TUT_REQUIRE_MSG(chk,msg) do { Ntut::require((chk), __FILE__, __LINE__, msg); } while (0)

#endif /* Ntut_tinyunittest_asserts_HPP_INCLUDED */
