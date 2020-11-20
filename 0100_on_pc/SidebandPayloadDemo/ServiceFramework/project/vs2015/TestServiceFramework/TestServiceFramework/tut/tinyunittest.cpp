/*! \file tinyunittest.cpp
    \brief minimal unit test framework optimized for developer.
    Copyright  ©2006-2006 Siemens VDO

\par History
\verbatim
(ks) = Kurt Stege, kurt.stege@siemens.com

2006-01-16 (ks):    File created.
2006-01-25 (ks):    First release.
2006-02-07 (ks):    Using std::cout for all debug infos, thanks to std::ios_base::Init.
2006-02-15 (ks):    Renamed TUT_MAIN to TUT_UNITTEST.
2006-02-16 (ks):    CTestSuiteCreator added (as dummy, without actually creating a suite).
2006-02-16 (ks):    Checking that all test cases are logically belonging to a test suite.
2006-04-27 (ks):    Bugfix: Standard Exceptions are now caught by reference instead of value
                    to omit the slicing.
2006-07-07 (ks):    Added new function runNamedTest.
\endverbatim
*/

#include "tut/tinyunittest.hpp"
#include <vector>
#include <sstream>
#include <string.h>

/*!
    The following defines configure some features of
    this file in the following way:

     0 = The feature is not compiled at all and therefore
         cannot be enabled during run-time. This is useful,
         when the compiler does not provide the feature.
     1 = The feature is compiled and activated by default.
         During run-time, the option can be disabled later on.
    -1 = The feature is compiled, but disabled by default.
         During run-time, the option can be enabled later on.

    See the macro \c TUT_UNITTEST() and the function
    \c Ntut::COptions::setOptions to learn more about run-time
    configuration.
*/
#define TUT_OUTPUT_WINDOWS_DEBUGGER()   (+1) //!< enables result output to the windows debugger output window
#define TUT_OUTPUT_STDERR()             (+1) //!< enables result output to stderr/cerr
#define TUT_DEBUG()                     (-1) //!< activates trace output to stdout
#define TUT_CATCH_EXCEPTIONS()          (+1) //!< activates some try/catch mechanisms. Don't use when you want your debugger to catch exceptions.
#define TUT_BREAKPOINT_WINDOWS()        (-1) //!< activates calling the windows debugger in case of a failed test


#if TUT_OUTPUT_STDERR() || TUT_DEBUG()
#   include <iostream>
#   include <ostream>
#   include <iomanip>
#endif

#if TUT_CATCH_EXCEPTIONS()
#   include <exception>
#endif

#if TUT_OUTPUT_WINDOWS_DEBUGGER() || TUT_BREAKPOINT_WINDOWS()
#   include <windows.h>
#endif



namespace Ntut {
bool getOrSetActiveTestSuite (bool set, bool value = true);
void outputMessage (const char *type, const char *file, int line, const char *msg);
void breakpoint ();
} // namespace Ntut

// Note: The class ITestresults can easily be extracted to a file ITestresults.hpp.

namespace Ntut {

class ITestresults
{
    public:

    enum EcountingType
    {
        ECT_registeredTests = 0,
        ECT_createdSuites,
        ECT_startedTests,
        ECT_abortedTests,
        ECT_finishedTests,

        ECT_oks,
        ECT_warnings,
        ECT_errors,

        ECT_numberOfTypes           //!< used for array indices
    };

    virtual void increment(EcountingType type) = 0;
    virtual int  getValue(EcountingType type) const = 0;

    bool hasFailed() { return getValue(ECT_errors) != 0; }
};

Ntut::ITestresults &getMainTestresults();

} // namespace Ntut



// Note: The class COptions can easily be extracted to files COptions.hpp/cpp.

namespace Ntut {

class COptions
{
    public:

    explicit COptions();

    enum EOption
    {
        OPT_debug,
        OPT_outputStderr,
        OPT_outputWindebug,
        OPT_breakpointWindebug,
        OPT_catchExceptions,

        OPT_numberOfOptions           //!< used for array indices
    };

    void setOptions(const char *options);
    bool getOption(EOption opt) const;

    private:

    bool m_options[OPT_numberOfOptions];
};

Ntut::COptions &getMainOptions();

} // namespace Ntut


Ntut::COptions::COptions()
{
    for (int i=0; i<OPT_numberOfOptions; i++)
    {
        m_options[i] = false;
    }

#   if (TUT_OUTPUT_WINDOWS_DEBUGGER() > 0)
    m_options[OPT_outputWindebug] = true;
#   endif

#   if (TUT_BREAKPOINT_WINDOWS() > 0)
    m_options[OPT_breakpointWindebug] = true;
#   endif

#   if (TUT_OUTPUT_STDERR() > 0)
    m_options[OPT_outputStderr] = true;
#   endif

#   if (TUT_DEBUG() > 0)
    m_options[OPT_debug] = true;
#   endif

#   if (TUT_CATCH_EXCEPTIONS() > 0)
    m_options[OPT_catchExceptions] = true;
#   endif

}

void Ntut::COptions::setOptions(const char *options)
{
    if (options == 0)
    {
        return;
    }

    bool enable = true;
    bool active = false;

    while (*options)
    {
        if (active)
        {
            switch (*options)
            {
                case '.':   active = false;                             break;
                case '+':   enable = true;                              break;
                case '-':   enable = false;                             break;
                case 'b':   m_options[OPT_breakpointWindebug] = enable; break;
                case 'd':   m_options[OPT_debug] = enable;              break;
                case 'e':   m_options[OPT_catchExceptions] = enable;    break;
                case 's':   m_options[OPT_outputStderr] = enable;       break;
                case 'w':   m_options[OPT_outputWindebug] = enable;     break;
                default:
                {
                    // option character not recognized...
                }
            }
        }
        else // not active
        {
            if (strncmp(options, "OPTNATIVE:", 10) == 0)
            {
                options += 9;   // one options++ is following.
                active = true;
            }
        }
        options ++;
    }
}

bool Ntut::COptions::getOption(EOption opt) const
{
    int i = opt;

    if (i >= 0 && i < OPT_numberOfOptions)
    {
        return m_options[i];
    }
    return false;
}

Ntut::COptions &Ntut::getMainOptions()
{
    static Ntut::COptions mainoptions;

    return mainoptions;
}


// Note: The implementaion of ITestcase can easily be extracted to a file ITestcase.cpp.

Ntut::ITestcase::ITestcase (const char *name, const char *file, int line)
:
    m_name(name),
    m_file(file),
    m_line(line)
{
}

Ntut::ITestcase::~ITestcase ()
{
}

void Ntut::ITestcase::runTest() const
{
#if TUT_CATCH_EXCEPTIONS()
    if (Ntut::getMainOptions().getOption(Ntut::COptions::OPT_catchExceptions))
    {
        try
        {
            Ntut::getMainTestresults().increment(Ntut::ITestresults::ECT_startedTests);
            runTestImpl();
            Ntut::getMainTestresults().increment(Ntut::ITestresults::ECT_finishedTests);
        }
        catch (const char *e)
        {

            if (strncmp (e, "TUT_EXPECTED", 12) == 0)
            {
                // This exception is expected and shall not be counted as error.
                // Alas, the test case has been aborted anyway, and this still has to be counted.

                Ntut::getMainTestresults().increment(Ntut::ITestresults::ECT_abortedTests);
#if TUT_DEBUG()
                if (Ntut::getMainOptions().getOption(Ntut::COptions::OPT_debug))
                {
                    Ntut::outputMessage("expected string exception caught", __FILE__, __LINE__, e);
                }
#endif
            }
            else
            {
                // This exception is not expected and shall be counted as error.

                Ntut::getMainTestresults().increment(Ntut::ITestresults::ECT_abortedTests);
                Ntut::getMainTestresults().increment(Ntut::ITestresults::ECT_errors);

                std::stringstream msg;
                msg << "unexpected string exception caught within test case '" << getName() << "'";
                if (*getFile() == '\0')
                {
                    Ntut::outputMessage(msg.str().c_str(), __FILE__, __LINE__, e);
                }
                else
                {
                    Ntut::outputMessage(msg.str().c_str(), getFile(), getLine(), e);
                }
            }
        }
        catch (std::exception &e)
        {
            Ntut::getMainTestresults().increment(Ntut::ITestresults::ECT_abortedTests);
            Ntut::getMainTestresults().increment(Ntut::ITestresults::ECT_errors);

            std::stringstream msg;
            msg << "unexpected std::exception caught within test case '" << getName() << "'";
            if (*getFile() == '\0')
            {
                Ntut::outputMessage(msg.str().c_str(), __FILE__, __LINE__, e.what());
            }
            else
            {
                Ntut::outputMessage(msg.str().c_str(), getFile(), getLine(), e.what());
            }
        }
        catch (...)
        {
            Ntut::getMainTestresults().increment(Ntut::ITestresults::ECT_abortedTests);
            Ntut::getMainTestresults().increment(Ntut::ITestresults::ECT_errors);

            std::stringstream msg;
            msg << "unexpected unknown exception caught within test case '" << getName() << "'";
            if (*getFile() == '\0')
            {
                Ntut::outputMessage(msg.str().c_str(), __FILE__, __LINE__, 0);
            }
            else
            {
                Ntut::outputMessage(msg.str().c_str(), getFile(), getLine(), 0);
            }
        }
    }
    else
#endif // TUT_CATCH_EXCEPTIONS()
    {
        Ntut::getMainTestresults().increment(Ntut::ITestresults::ECT_startedTests);
        runTestImpl();
        Ntut::getMainTestresults().increment(Ntut::ITestresults::ECT_finishedTests);
    }
}

const char *Ntut::ITestcase::getName() const
{
    const char *name = getNameImpl();

    if (name)
    {
        return name;
    }
    else
    {
        return "<noname>";
    }
}

const char *Ntut::ITestcase::getFile() const
{
    const char *file = getFileImpl();

    if (file)
    {
        return file;
    }
    else
    {
        return "";
    }
}

int Ntut::ITestcase::getLine() const
{
    return getLineImpl();
}

const char *Ntut::ITestcase::getNameImpl() const
{
    return m_name;
}

const char *Ntut::ITestcase::getFileImpl() const
{
    return m_file;
}

int Ntut::ITestcase::getLineImpl() const
{
    return m_line;
}



// Note: The class ITestsuite can easily be extracted to files ITestsuite.hpp/cpp.
namespace Ntut {
/*!
    A test suite is just a container of test cases.

    A test suite itself is formal a test case that
    can be run. When run, it runs all the test cases
    stored in the container.

    Thus, a test suite might contain other test suites.
    In this way, test cases can be structured in a
    tree like way.
*/

class ITestsuite : public Ntut::ITestcase
{
    public:

    explicit ITestsuite(const char *name = 0, const char *file = 0, int line = 0)
    :
        ITestcase(name, file, line)
    {
    }

    /*!
        \c registerTest() registers a further test case.

        Please note that the test suite just stores the
        pointer without taking any ownership of the test
        case (regarding memory management).
    */
    virtual void registerTest(ITestcase *testcase) = 0;

    /*!
        Each ITestcase, therefore each ITestsuite has a function
        \c runTest() to run the test. For a testsuit that function
        runs all tests registered in that suite.

        For special usage this additional function \c runNamedTest()
        just calls the one registered test with the given name.
    */
    virtual int runNamedTest(const char *name) = 0;
};

Ntut::ITestsuite &getMainTestsuite();

} // namespace Ntut


// Note: The class CTestsuite can easily be extracted to files CTestsuite.hpp/cpp.
namespace Ntut {

class CTestsuite : public Ntut::ITestsuite
{
    public:

    explicit CTestsuite(const char *name = 0, const char *file = 0, int line = 0);

    virtual void registerTest(ITestcase *testcase);

    virtual int runNamedTest(const char *name);

    private:

    virtual void runTestImpl() const;

    typedef std::vector<Ntut::ITestcase *> TTestcaseList;

    TTestcaseList m_testcases;
};

} // namespace Ntut

Ntut::CTestsuite::CTestsuite(const char *name, const char *file, int line)
:
    ITestsuite(name, file, line)
{
}

void Ntut::CTestsuite::registerTest(ITestcase *testcase)
{
    if (testcase != 0)
    {
        if (! getOrSetActiveTestSuite(false))
        {
            Ntut::check(false, testcase->getFile(), testcase->getLine(), "test case defined without any test suite");
        }
        Ntut::getMainTestresults().increment(Ntut::ITestresults::ECT_registeredTests);
        m_testcases.push_back(testcase);
    }
}

void Ntut::CTestsuite::runTestImpl() const
{
    TTestcaseList::const_iterator iter;
#if TUT_DEBUG()
    if (Ntut::getMainOptions().getOption(Ntut::COptions::OPT_debug))
    {
        std::cout << "tinyunittest: running test suite '" << getName() << "' starting\n";
    }
#endif
	for (iter = m_testcases.begin (); iter != m_testcases.end (); ++iter)
    {
#if TUT_DEBUG()
        if (Ntut::getMainOptions().getOption(Ntut::COptions::OPT_debug))
        {
            std::cout << "tinyunittest: running test case '" << (*iter)->getName() << "'\n";
        }
#endif
		(*iter)->runTest();
    }
#if TUT_DEBUG()
    if (Ntut::getMainOptions().getOption(Ntut::COptions::OPT_debug))
    {
        std::cout << "tinyunittest: running test suite '" << getName() << "' finished\n";
    }
#endif
}


/*!
    \c runNamedTest calls one of the registered test cases, namely that
    one identified by the given name.

    The function returns the number of new errors, triggered by this test case.
    When the returned value is zero, the test passed OK. Otherwise it failed.
*/
int Ntut::CTestsuite::runNamedTest(const char *name)
{
    int errors1 = Ntut::getMainTestresults().getValue(Ntut::ITestresults::ECT_errors);
    int number_of_found_tests = 0;

    TTestcaseList::const_iterator iter;
	for (iter = m_testcases.begin (); iter != m_testcases.end (); ++iter)
    {
#if TUT_DEBUG()
        if (Ntut::getMainOptions().getOption(Ntut::COptions::OPT_debug))
        {
            std::cout << "tinyunittest: running test case '" << (*iter)->getName() << "'\n";
        }
#endif
        if (0 == strcmp(name, (*iter)->getName()))
        {
            number_of_found_tests ++;
          (*iter)->runTest();
        }
    }

    if (number_of_found_tests == 0)
    {
        TUT_CHECK_MSG (false, "There is no test of the given name.");
    }

    int errors2 = Ntut::getMainTestresults().getValue(Ntut::ITestresults::ECT_errors);

    return errors2 - errors1;
}


Ntut::ITestsuite &Ntut::getMainTestsuite()
{
    static Ntut::CTestsuite maintestsuite("main unit test suite", __FILE__, __LINE__);

    return maintestsuite;
}



// Note: The implementation of class CTestSuiteCreator can easily be extracted to a file CTestSuiteCreator.cpp.

/*!
    This test suite creator does _not_ actually create a real test suite.
    It only counts for the statistics that a test suite has been
    supposed to be created, and it sets a marker that enables the
    creation of test cases.
*/

Ntut::CTestSuiteCreator::CTestSuiteCreator(const char *name, const char *file, int line)
{
    Ntut::getMainTestresults().increment(Ntut::ITestresults::ECT_createdSuites);
    Ntut::getOrSetActiveTestSuite(true, true);

#if TUT_DEBUG()
    /*
       Note: This code is executed while dynamic initialization
       before main() is started. At this time, the run-time
       configuration has not yet been executed. Therfore,
       the call of \c getOption() returns the default configuration
       at this time.
    */
    if (Ntut::getMainOptions().getOption(Ntut::COptions::OPT_debug))
    {
        /*
           Note: This code is executed while dynamic initialization
           before main() is started. std::cout is not required to
           be initialized yet, according to the C++ standard.
           The Visual C++ 6.0 implementation crashes with some
           access violation when using cout.

           Creating an object of type std::ios_base::Init
           initializes the standard iostreams objects like cout.
        */
        std::ios_base::Init dummy_to_initialize_cout; // initializes std::cout
        if (file != 0)
        {
            std::cout << "tinyunittest: creating test suite '" << name << "' (file " << file << ", line " << line << ")\n";
        }
        else
        {
            std::cout << "tinyunittest: creating test suite '" << name << "'\n";
        }
    }
#else
    name;
    file;
    line;
#endif
}

Ntut::CTestSuiteCreator::~CTestSuiteCreator()
{
}


// Note: The implementation of class CTestSuiteDestroyer can easily be extracted to a file CTestSuiteDestroyer.cpp.

/*!
    This test suite destroyer removes any test suite from being "active".
    It is not allowed to create test cases before specifing any test
    suite as the active suite that collects the new test cases.
*/

Ntut::CTestSuiteDestroyer::CTestSuiteDestroyer()
{
    Ntut::getOrSetActiveTestSuite(true, false);
}

Ntut::CTestSuiteDestroyer::~CTestSuiteDestroyer()
{
}


// Note: The class CTestresults can easily be extracted to files CTestresults.hpp/cpp.
namespace Ntut {

class CTestresults : public Ntut::ITestresults
{
    public:

    CTestresults ();

    virtual void increment(EcountingType type);
    virtual int  getValue(EcountingType type) const ;

    private:

    int m_values[ECT_numberOfTypes];
};

} // namespace Ntut

Ntut::CTestresults::CTestresults()
{
    for (int i=0; i<ECT_numberOfTypes; i++)
    {
        m_values[i] = 0;
    }
}

void Ntut::CTestresults::increment(EcountingType type)
{
    if (type >= 0 && type < ECT_numberOfTypes)
    {
        m_values[type] ++;
    }
}

int Ntut::CTestresults::getValue(EcountingType type) const
{
    if (type >= 0 && type < ECT_numberOfTypes)
    {
        return m_values[type];
    }
    return (0);
}


Ntut::ITestresults &Ntut::getMainTestresults()
{
    static Ntut::CTestresults maintestresults;

    return maintestresults;
}


void Ntut::warn (bool chk, const char *file, int line, const char *msg)
{
    if (chk)
    {
        Ntut::getMainTestresults().increment(Ntut::ITestresults::ECT_oks);
    }
    else
    {
        Ntut::getMainTestresults().increment(Ntut::ITestresults::ECT_warnings);
        Ntut::outputMessage ("warning (not counted as failure)", file, line, msg);
        Ntut::breakpoint();
    }
}

void Ntut::check (bool chk, const char *file, int line, const char *msg)
{
    if (chk)
    {
        Ntut::getMainTestresults().increment(Ntut::ITestresults::ECT_oks);
    }
    else
    {
        Ntut::getMainTestresults().increment(Ntut::ITestresults::ECT_errors);
        Ntut::outputMessage ("check failed", file, line, msg);
        Ntut::breakpoint();
    }
}

void Ntut::require (bool chk, const char *file, int line, const char *msg)
{
    if (chk)
    {
        Ntut::getMainTestresults().increment(Ntut::ITestresults::ECT_oks);
    }
    else
    {
        Ntut::getMainTestresults().increment(Ntut::ITestresults::ECT_errors);
        Ntut::outputMessage ("requirement failed, test case aborted", file, line, msg);
        Ntut::breakpoint();
        // Note: The prefix "TUT_EXPECTED" is used to tell that this exception is not to be counted as error and does not need to be logged at all.
        throw "TUT_EXPECTED: test case aborted due to failed requirement";
    }
}


/*!
    This function is called whenever an error or a warning occurs.
    It may be used when debugging the application to set a break point
    at this position.

    In addition, for the windows environment this file can be configured
    to trigger a break point without a manually set break point.
*/

void Ntut::breakpoint()
{
    int i;
    i = 0;  // You might want to set the break point to this line.

#if TUT_BREAKPOINT_WINDOWS()
    if (Ntut::getMainOptions().getOption(Ntut::COptions::OPT_breakpointWindebug))
    {
        DebugBreak();
    }
#endif
}

bool Ntut::getOrSetActiveTestSuite (bool set, bool value)
{
    static bool activeTestsuite = false;

    if (set)
    {
        activeTestsuite = value;
    }

    return activeTestsuite;
}

void Ntut::outputMessage (const char *type, const char *file, int line, const char *msg)
{
#if TUT_OUTPUT_STDERR()
    if (Ntut::getMainOptions().getOption(Ntut::COptions::OPT_outputStderr))
    {
        /*
            The unit test might create some global objects and put
            checks within these construtors. Therefore, this function
            might be called before main() has been started and before
            std::cerr has been initialized.
        */
        std::ios_base::Init dummy_to_initialize_cout; // initializes std::cerr

        std::cerr << "tinyunittest: " << type << " in file " << file << "(" << line << ")";
        if (msg)
        {
            std::cerr << ", msg='" << msg << "'";
        }
        std::cerr << ".\n";
    }
#endif

#if TUT_OUTPUT_WINDOWS_DEBUGGER()
    if (Ntut::getMainOptions().getOption(Ntut::COptions::OPT_outputWindebug))
    {
        std::stringstream ln;

        ln << file << "(" << line << ") : tinyunittest " << type;
        if (msg)
        {
            ln << ", msg='" << msg << "'";
        }
        ln << "\n";
        OutputDebugStringA (ln.str().c_str());
    }
#endif
}



int Ntut::mainImpl (const char *file, int line, const char *title, const char *options)
{
    // Evaluate the run-time configuration.
    // Alas, this configuration is not activated before main(), when
    // the test cases are registrated.

    Ntut::getMainOptions().setOptions(options);

    /*
        This one global test suite is not registered anywhere,
        but called directly (at this moment). It will be counted
        as started and (hopefully) finished test case, but has
        not yet been counted as registered test case.

        To make the resulting statistics more consistent, it will
        be counted as registered test case at this moment. In some
        understanding, this is not a really lie, for somehow this
        function \c mainImpl() has been "registered" somewhere,
        at least it just has been called.
    */

    Ntut::getMainTestresults().increment(Ntut::ITestresults::ECT_registeredTests);

    // First run all the tests...

    Ntut::getMainTestsuite().runTest();

    // Second (and last) present the results

#if TUT_DEBUG()
    if (Ntut::getMainOptions().getOption(Ntut::COptions::OPT_debug))
    {
        std::cout << "tinyunittest: all tests are run. Summary:\n";
        for (int i=0; i<Ntut::ITestresults::ECT_numberOfTypes; i++)
        {
            Ntut::ITestresults::EcountingType type = static_cast<Ntut::ITestresults::EcountingType>(i);
            std::cout << std::setw(13) << Ntut::getMainTestresults().getValue(type) << " ";
            switch (type)
            {
                case Ntut::ITestresults::ECT_registeredTests: std::cout << "test(s) registered"; break;
                case Ntut::ITestresults::ECT_createdSuites: std::cout << "suite(s) created"; break;
                case Ntut::ITestresults::ECT_startedTests:  std::cout << "test(s) started"; break;
                case Ntut::ITestresults::ECT_abortedTests:  std::cout << "test(s) aborted"; break;
                case Ntut::ITestresults::ECT_finishedTests: std::cout << "test(s) finished"; break;
                case Ntut::ITestresults::ECT_oks:           std::cout << "check(s) OK"; break;
                case Ntut::ITestresults::ECT_warnings:      std::cout << "warning(s)"; break;
                case Ntut::ITestresults::ECT_errors:        std::cout << "error(s)"; break;
                default:                                    std::cout << "<unknown>"; break;
            }
            std::cout << "\n";
        }
    }
#endif

    std::stringstream s1, s2;

    s1 << "results of test '" << title << "'";

    if (Ntut::getMainTestresults().hasFailed())
    {
        s2 << "FAILED. " << Ntut::getMainTestresults().getValue(Ntut::ITestresults::ECT_errors) << " error(s)";
    }
    else if (Ntut::getMainTestresults().getValue(Ntut::ITestresults::ECT_warnings) != 0)
    {
        s2 << "OK. Alas " << Ntut::getMainTestresults().getValue(Ntut::ITestresults::ECT_warnings) << " warning(s)";
    }
    else
    {
        s2 << "OK. All " << Ntut::getMainTestresults().getValue(Ntut::ITestresults::ECT_oks) << " check(s) passed";
    }

    Ntut::outputMessage (s1.str().c_str(), file, line, s2.str().c_str());

    if (Ntut::getMainTestresults().hasFailed())
    {
        return 1;
    }
    return 0;
}



/*!
    This function is used when not all tests are to be executed
    (see \c mainImpl for that case) but when only one test has
    to be executed at this time.

    \param name     The name of the test to be executed.

    \return         0, when the test passed and no errors occured.
    \return         > 0, when the test failed. The return value gives the
                    number of errors (i.e. failed checks) within this test.
*/
int Ntut::runNamedTest (const char *name)
{
    return Ntut::getMainTestsuite().runNamedTest(name);
}


/*!
    This function is mainly implemented to be used in a macro
    in tinyunittest.hpp that calls this function and thus evaluates
    a macro parameter exactly once.
*/
void Ntut::registerTestcase (Ntut::ITestcase *testcase)
{
    if (testcase != 0)
    {
#if TUT_DEBUG()
        /*
           Note: This code is executed while dynamic initialization
           before main() is started. At this time, the run-time
           configuration has not yet been executed. Therfore,
           the call of \c getOption() returns the default configuration
           at this time.
        */
        if (Ntut::getMainOptions().getOption(Ntut::COptions::OPT_debug))
        {
            /*
               Note: This code is executed while dynamic initialization
               before main() is started. std::cout is not required to
               be initialized yet, according to the C++ standard.
               The Visual C++ 6.0 implementation crashes with some
               access violation when using cout.

               Creating an object of type std::ios_base::Init
               initializes the standard iostreams objects like cout.
            */
            std::ios_base::Init dummy_to_initialize_cout; // initializes std::cout
            std::cout << "tinyunittest: registering test case '" << testcase->getName() << "'\n";
        }
#endif
        Ntut::getMainTestsuite().registerTest (testcase);
    }
}
