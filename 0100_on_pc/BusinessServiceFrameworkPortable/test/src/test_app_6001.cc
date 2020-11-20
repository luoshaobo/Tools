#include "test_app_6001.h"

namespace test_app_6001 {

    static const unsigned int TIMER1_INNTERVAL = 10 * 1000000; // 10 seconds

    class TestClass : public ITimerExpired
    {
    public:
        TestClass()
            : m_businessServer()
            , m_timer1()
            , m_stopped(false)
        {
            BSFWK_LOG_GEN_PRINTF("\n");
            m_timer1 = Timer(this, TIMER1_INNTERVAL, false);            // To create a new timer in the thread other than m_businessServer thread.
            m_businessServer.addTimer(m_timer1);                        // To start the timer in the thread other than m_businessServer thread.
        }

        virtual ~TestClass()
        {
            BSFWK_LOG_GEN_PRINTF("\n");
            m_businessServer.removeTimer(m_timer1.getId());             // To stop the timer in the thread other than m_businessServer thread.
        }

        void Func1(int n)
        {
            BSFWK_LOG_GEN_PRINTF("\n");
            m_businessServer.PostAsyncCall(std::bind(&TestClass::OnFunc1, this, n));
        }

        void OnFunc1(int n)
        {
            BSFWK_LOG_GEN_PRINTF("\n");
        }

        virtual void handleTimeout(int timerId)
        {
            BSFWK_LOG_GEN_PRINTF("\n");

            static unsigned int count = 0;

            if (m_timer1.getId() == timerId) {
                m_businessServer.removeTimer(m_timer1.getId());         // To stop the timer in m_businessServer thread.

                BSFWK_LOG_GEN_PRINTF("count=%u\n", count);

                if (count >= 2) {
                    m_stopped = true;
                } else {
                    m_timer1 = Timer(this, TIMER1_INNTERVAL, false);    // To create a new timer in m_businessServer thread, otherwise, the old timer will not expire any more.
                    m_businessServer.addTimer(m_timer1);                // To start the timer again in m_businessServer thread.
                }

                count++;
            }
        }

        bool GetStopped() const { return m_stopped; }

    private:
        BusinessServer m_businessServer;
        Timer m_timer1;
        bool m_stopped;
    };

    int test_main(int argc, char *argv[])
    {
        BSFWK_LOG_PRINTF("##########################################################################################\n");
        BSFWK_LOG_PRINTF("## [%s: %s]\n", BSFWK_FILEINFO, BSFWK_FUNCINFO);
        BSFWK_LOG_PRINTF("##\n");

        TestClass t;
        t.Func1(99);

        if (IsAutoTest()) {
            while (!t.GetStopped()) {
                msleep(1000);
            }
        } else {
            // NOTE: enther any character to exit this program.
            getchar();
        }

        return 0;
    }

} // namespace test_app_6001 {
