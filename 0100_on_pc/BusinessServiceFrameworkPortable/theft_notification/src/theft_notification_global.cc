#include "theft_notification_global.h"
#include "theft_notification.h"

namespace volvo_on_call {

    const uint32_t TEST_RMC_SIGNAL_COUNT = 2;

    static void post_tn_signal()
    {
        msleep(0 * 1000);

        for (uint32_t i = 0; i < TEST_RMC_SIGNAL_COUNT; ++i) {
            TheftUpload_t response{100, 101, true};
            fsm::VdServiceTransactionId transaction_id;
            std::shared_ptr<GlyVdsTheftSignal> signal = std::make_shared<GlyVdsTheftSignal>(transaction_id, (void *)NULL);
            signal->SetTheftPayload(response);
            PostSignalEvent(signal);
        }
    }

	int test_tn_main(int argc, char *argv[])
	{
	    using namespace volvo_on_call;	

	    BusinessServer businessServer;
		g_PostSignalEvent_Driver = [&businessServer](std::shared_ptr<fsm::Signal> signal) {
            businessServer.PostSignalEvent(signal);
		};

		EntityFactory_TN factoryTN;
	    BusinessService businessServiceTN(&businessServer, &factoryTN);
	    businessServer.AddService(&businessServiceTN);
        //businessServiceTN.SetLogLevel(LogLevel_Verbose);
		businessServiceTN.Start();

        really_async(std::bind(&post_tn_signal));

        // NOTE: enther any character to exit this program.
		getchar();

        g_PostSignalEvent_Driver = NULL;

        businessServiceTN.Stop();
		
	    return 0;
	}

#ifdef TCAM_TARGET
    class TNTransaction : public fsm::Transaction
    {
    public:
        TNTransaction();
        virtual ~TNTransaction();
        
    public:
        virtual bool WantsSignal (std::shared_ptr<fsm::Signal> signal) { return true; }
        virtual bool HandleSignal (std::shared_ptr<fsm::Signal> signal);

    private:
        BusinessServer *m_pBusinessServer;
        EntityFactory_TN *m_pFactoryTN;
        BusinessService *m_pBusinessServiceTN;
    };

    TNTransaction::TNTransaction()
        : fsm::Transaction()
        ,m_pBusinessServer(NULL)
        ,m_pFactoryTN(NULL)
        ,m_pBusinessServiceTN(NULL)
    {
        BSFWK_LOG_GEN_PRINTF("\n");
        m_pBusinessServer = new BusinessServer();

		m_pFactoryTN = new EntityFactory_TN();
	    m_pBusinessServiceTN = new BusinessService(m_pBusinessServer, m_pFactoryTN);
	    m_pBusinessServer->AddService(m_pBusinessServiceTN);
        //m_pBusinessServiceTN->SetLogLevel(LogLevel_Verbose);
		m_pBusinessServiceTN->Start();

        g_PostSignalEvent_Driver = [this](std::shared_ptr<fsm::Signal> signal) {
            this->m_pBusinessServer->PostSignalEvent(signal);
		};
    }

    TNTransaction::~TNTransaction()
    {
        BSFWK_LOG_GEN_PRINTF("\n");
        g_PostSignalEvent_Driver = NULL;
        
        m_pBusinessServiceTN->Stop();
            
        delete m_pBusinessServiceTN;
        delete m_pFactoryTN;
        delete m_pBusinessServer;
    }

    bool TNTransaction::HandleSignal (std::shared_ptr<fsm::Signal> signal)
    {
        BSFWK_LOG_GEN_PRINTF("\n");
        if (m_pBusinessServer != NULL) {
            BSFWK_LOG_GEN_PRINTF("\n");
            m_pBusinessServer->PostSignalEvent(signal);
        }

        return true;
    }

    std::shared_ptr<fsm::Transaction> CreateTNTransaction()
    {
        return std::make_shared<TNTransaction>();
    }
#endif // #ifdef TCAM_TARGET

} // namespace volvo_on_call {

