/*! \file tinyunittest.hpp
    \brief minimal unit test framework optimized for developer,
    "native implementation".

    Copyright  ©2006-2006 Siemens VDO

    The user of this header file is supposed to use only
    a few classes, functions, and macros:

    macro \c TUT_WARN(chk)
    macro \c TUT_CHECK(chk)
    macro \c TUT_REQUIRE(chk)

    macro \c TUT_WARN_MSG(chk, msg)
    macro \c TUT_CHECK_MSG(chk, msg)
    macro \c TUT_REQUIRE_MSG(chk, msg)

    macro \c TUT_UNITTEST(title, options)
    macro \c TUT_TESTSUITE(identifier)
    macro \c TUT_TESTCASE(identifier)
    macro \c TUT_NO_TESTCASE(identifier)

    function \c Ntut::warn()
    function \c Ntut::check()
    function \c Ntut::require()

    All other stuff declared in this header file (or in
    accompanying implementation files) are to be treated
    as implementation details and not to be used at all.

    The user has to be aware that there are many different
    implementations of the tinyunittest framework to achieve
    completely different behaviour of the test code.


    This actual implementation of tinyunittest is called
    the "native implementation" and is used to generate for
    each single unit test an own executable that can be
    run easily in a debugger.

    Other implementations adapt the interface to complexer
    unit test frameworks that support organization of many
    single unit tests to complex systems and manage compiling
    and running that unit tests in a powerful way.

\par History
\verbatim
(ks) = Kurt Stege, kurt.stege@siemens.com

2006-01-16 (ks):    File created.
2006-01-25 (ks):    First release.
2006-02-14 (ks):    Added TUT_MAIN_FUNCTION (to support Rational Test Real Time)
2006-02-15 (ks):    Marked the parts of this header file that should be the same for all implementations.
2006-02-15 (ks):    Renamed TUT_MAIN to TUT_UNITTEST.
2006-02-15 (ks):    Added TUT_TESTSUITE.
2006-02-16 (ks):    Large parts moved to "tut/tinyunittest_asserts.hpp".
2006-02-16 (ks):    Second release.
2006-07-07 (ks):    Added new function runNamedTest.
\endverbatim
*/

#ifndef Ntut_tinyunittest_HPP_INCLUDED
#define Ntut_tinyunittest_HPP_INCLUDED

#include "tut/tinyunittest_asserts.hpp"

namespace Ntut {

/*!
    \c ITestcase is the basic test case interface used in
    the native implementation of the tinyunittest framework.

    \warning This class is an implementation detail of the macros
    that generate test cases. Do not use this class on your own.

    Other implementations of the TUT framework might not use
    or define this class.
*/

class ITestcase
{
    public:

    explicit ITestcase(const char *name = 0, const char *file = 0, int line = 0);
    virtual ~ITestcase();

    void runTest() const;
    const char *getName() const;    //!< returns the name of the test case
    const char *getFile() const;    //!< returns "" or the name of the source file name where the test case is implemented
    int         getLine() const;    //!< returns 0 or the line number of the source file name where the test case is implemented

    private:

    virtual void runTestImpl() const = 0;
    virtual const char *getNameImpl() const; //!< is allowed to return 0, when no name is available
    virtual const char *getFileImpl() const; //!< is allowed to return 0 or "", when no file name is available
    virtual int         getLineImpl() const; //!< is allowed to return 0 when no line number is available

    const char *m_name;
    const char *m_file;
    int m_line;
};


/*!
    The class \c CTestSuiteCreator is used from the macro TUT_TESTSUITE
    (and TUT_UNITTEST) to create and register a test suite.

    The current implementation just provides means to update the
    statistical data and to verify that all test cases are logically
    connected to a test suite. This implementation does not actually
    create a test suite but uses a single global test suite for all
    test cases.
*/

class CTestSuiteCreator
{
    public:

    explicit CTestSuiteCreator(const char *name = 0, const char *file = 0, int line = 0);
    ~CTestSuiteCreator();
};


/*!
    The class \c CTestSuiteDestroyer is used from the macro TUT_TESTSUITE
    (and TUT_UNITTEST) to create and register a test suite.

    The current implementation just provides means to update the
    statistical data and to verify that all test cases are logically
    connected to a test suite. This implementation does not actually
    create a test suite but uses a single global test suite for all
    test cases.
*/

class CTestSuiteDestroyer
{
    public:

    explicit CTestSuiteDestroyer();
    ~CTestSuiteDestroyer();
};

void registerTestcase (Ntut::ITestcase *testcase);  //!< implementation detail
int mainImpl (const char *file, int line, const char *title, const char *options);  //!< implementation detail

/*!
    This function is used when not all tests are to be executed
    (see \c mainImpl for that case) but when only one test has
    to be executed at this time.

    \param name     The name of the test to be executed.

    \return         0, when the test passed and no errors occured.
    \return         > 0, when the test failed. The return value gives the
                    number of errors (i.e. failed checks) within this test.
*/
int runNamedTest (const char *name);

} // namespace Ntut

#define TUT_MAKETESTSUITE1(arg) TUT_MAKETESTSUITE2(arg)
#define TUT_MAKETESTSUITE2(arg) TUT_MAKETESTSUITE3(global_##arg##_suite)
#define TUT_MAKETESTSUITE3(arg) TUT_TESTSUITE(arg)

/*!
    The macro TUT_MAIN_FUNCTION can be pre-defined before
    including this header file, typically by using a compiler
    option, to change the function name of the unit test
    main function.
*/

#ifndef TUT_MAIN_FUNCTION
#   define TUT_MAIN_FUNCTION    main
#endif

/*!
    The macro \c TUT_UNITTEST(title, options) is used
    to collect and run all test cases and gives the
    test a title.

    Each unit test source code file defining test cases
    should call this macro exactly once, to put the
    test cases into one group.

    The native implementation of tinyunittest uses this
    macro to create a main() function that calls these
    tests. Therefore, the native implementation supports
    only simple unit test applications with one single
    file for the test cases.
*/

#define TUT_UNITTEST(title, options) \
    int TUT_MAIN_FUNCTION (int /* argc */, char * /* argv */ []) \
    { \
        return Ntut::mainImpl(__FILE__, __LINE__, title, options); \
    } \
    TUT_MAKETESTSUITE1(TUT_MAIN_FUNCTION)


/*!
    The macro \c TUT_TESTSUITE(identifier) creates a test suite.
    All following test cases (within the same source code)
    are registered to this test suite. The test suite itself
    is registered to the (one and global) unit test.

    The native implementation just discard all test suites
    and collects all test cases without further structuring
    into the main unit test.

    Alas, the developer of a complex unit test has to use
    test suites, when splitting the test cases into several
    files: A test case is only be allowed to be defined
    after calling \c TUT_UNITTEST or \c TUT_TESTSUITE, and
    a unit test has to be defined exactly once.

    \todo Implement \c TUT_TESTSUITE in a way that statistics
    are generated about the number of test suites executed
    (count them at least as test cases), and in a way to
    provide a (run-time) error, when a test case is registered
    without a context (test suite or unit test).
*/

#define TUT_TESTSUITE(identifier)                               \
    namespace Ntut {                                            \
    Ntut::CTestSuiteCreator testsuite_##identifier##_creator_object \
        (#identifier, __FILE__, __LINE__);  \
    } /* namespace Ntut */                                      \



/*!
    The macro \c TUT_TESTCASE generates a new class and
    an object of that class. This object implements a
    test case. Furthermore, the test case is registrated
    for further execution.

    The implementation of the runTestImpl() function has
    to follow immediately after calling this macro.

    \see TUT_NO_TESTCASE for disabling a single test case.
*/

#define TUT_TESTCASE(identifier)    TUT_TESTCASE_FOR_INTERNAL_USE(identifier, 1)


/*!
    The macro \c TUT_NO_TESTCASE works the same as \c TUT_TESTCASE.
    Alas, it does not register the generated object, and thus
    prevents the automatic running of the test case.

    Due to the cryptic internal name of that object, the
    test case cannot be run at all.

    This macro is used to disable a single test case in a quick way.
*/

#define TUT_NO_TESTCASE(identifier) TUT_TESTCASE_FOR_INTERNAL_USE(identifier, 0)


#define TUT_MAKESTRING1(arg) TUT_MAKESTRING2(arg)
#define TUT_MAKESTRING2(arg) #arg
// Example: TUT_MAKESTRING1(__LINE__) generates a string containing the line number.

#define TUT_TESTCASE_FOR_INTERNAL_USE(name,reg)             \
    namespace Ntut {                                        \
    class CTestcase_##name##_TUT : public Ntut::ITestcase   \
    {                                                       \
        public:                                             \
        CTestcase_##name##_TUT()                            \
        :                                                   \
            ITestcase(#name, __FILE__, __LINE__)            \
        {                                                   \
            if (reg)                                        \
            {                                               \
                Ntut::registerTestcase (this);              \
            }                                               \
        }                                                   \
                                                            \
        private:                                            \
        void runTestImpl() const;                           \
    } testcase_##name##_object;                             \
    } /* namespace Ntut */                                  \
                                                            \
    void Ntut::CTestcase_##name##_TUT::runTestImpl() const  \


/*
    Note: The implementation of the macro TUT_ANCH_AMUN
    should be the same for all implementations of the Tiny Unit
    Test framework.

    It is implemented for fun, and developers of test cases might
    use it for fun. Thus, it is probably no good idea to discard
    the macro, or to give it any semantical meaning.
*/

/*!
    \warning obsolete
    \warning deprecated

    The macro TUT_ANCH_AMUN(comment) is introduced just for fun
    and might be used to introduce comments, like author, date
    and such things about the unit test. Ordinary C++ comments
    are much more appropriate for this task, so please don't
    use this macro.

    It is marked as obsolete and deprecated, and thus might
    be discarded in further revisions of the TUT framework.
    It has never been documented as availabe, at least not without
    this warning...
*/
#define TUT_ANCH_AMUN(comment)


namespace { // anonymous namespace

/*!
    Yes, this is a header file. Including this header file
    creates a static object that deletes the flag that allows
    the creation of test cases.

    This object is defined at first in any file using test cases,
    before defining any test case. This way, defining of a
    test case will fail, unless prior the flag has been set
    again by defining a test suite.
*/

Ntut::CTestSuiteDestroyer dummy_forbidTestcasesWithoutDefiningATestsuite;

} // anonymous namespace

#endif /* Ntut_tinyunittest_HPP_INCLUDED */
