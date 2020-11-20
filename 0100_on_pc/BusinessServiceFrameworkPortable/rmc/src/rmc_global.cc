#include "rmc_global.h"
#include "rmc_door_tailgate.h"
#include "rmc_flash_horn.h"
#include "rmc_pm25.h"
#include "rmc_window_roof.h"

namespace volvo_on_call {

    const uint32_t TEST_RMC_SIGNAL_COUNT = 2;

    void post_signal_01()
    {
        msleep(0 * 1000);

        for (uint32_t i = 0; i < TEST_RMC_SIGNAL_COUNT; ++i) {
            std::shared_ptr<GlyVdsRmcSignal> signal = simulator::CreateGlyVdsSignal(fsm::kVDServiceRdl);
            PostSignalEvent(signal);
        }
    }

    void post_signal_02()
    {
        msleep(5 * 1000);

        for (uint32_t i = 0; i < TEST_RMC_SIGNAL_COUNT; ++i) {
            std::shared_ptr<GlyVdsRmcSignal> signal = simulator::CreateGlyVdsSignal(fsm::kVDServiceRhl);
            PostSignalEvent(signal);
        }
    }

    void post_signal_03()
    {
        msleep(10 * 1000);

        for (uint32_t i = 0; i < TEST_RMC_SIGNAL_COUNT; ++i) {
            std::shared_ptr<GlyVdsRmcSignal> signal = simulator::CreateGlyVdsSignal(fsm::kVDServiceRpp);
            PostSignalEvent(signal);
        }
    }

    void post_signal_04()
    {
        msleep(15 * 1000);

        for (uint32_t i = 0; i < TEST_RMC_SIGNAL_COUNT; ++i) {
            std::shared_ptr<GlyVdsRmcSignal> signal = simulator::CreateGlyVdsSignal(fsm::kVDServiceRws);
            PostSignalEvent(signal);
        }
    }

	int test_rmc_main(int argc, char *argv[])
	{
	    using namespace volvo_on_call;	

	    BusinessServer businessServer;
		g_PostSignalEvent_Driver = [&businessServer](std::shared_ptr<fsm::Signal> signal) {
            businessServer.PostSignalEvent(signal);
		};

		EntityFactory_RMCDTG factoryRMCDTG;
	    BusinessService businessServiceRMCDTG(&businessServer, &factoryRMCDTG);
	    businessServer.AddService(&businessServiceRMCDTG);
        //businessServiceRMCDTG.SetLogLevel(LogLevel_Verbose);
		businessServiceRMCDTG.Start();

		EntityFactory_RMCFH factoryRMCFH;
		BusinessService businessServiceRMCFH(&businessServer, &factoryRMCFH);
		businessServer.AddService(&businessServiceRMCFH);
        //businessServiceRMCFH.SetLogLevel(LogLevel_Warning);
		businessServiceRMCFH.Start();

		EntityFactory_RMCPM25 factoryRMCPM25;
		BusinessService businessServiceRMCPM25(&businessServer, &factoryRMCPM25);
		businessServer.AddService(&businessServiceRMCPM25);
        //businessServiceRMCPM25.SetLogLevel(LogLevel_Warning);
		businessServiceRMCPM25.Start();

		EntityFactory_RMCWR factoryRMCWR;
		BusinessService businessServiceRMCWR(&businessServer, &factoryRMCWR);
		businessServer.AddService(&businessServiceRMCWR);
        //businessServiceRMCWR.SetLogLevel(LogLevel_Warning);
		businessServiceRMCWR.Start();

        really_async(std::bind(&post_signal_01));
        really_async(std::bind(&post_signal_02));
        really_async(std::bind(&post_signal_03));
        really_async(std::bind(&post_signal_04));

        // NOTE: enther any character to exit this program.
		getchar();

        g_PostSignalEvent_Driver = NULL;

        businessServiceRMCDTG.Stop();
        businessServiceRMCFH.Stop();
        businessServiceRMCPM25.Stop();
        businessServiceRMCWR.Stop();

	    return 0;
	}

#ifdef TCAM_TARGET
    class RMCTransaction : public fsm::Transaction
    {
    public:
        RMCTransaction();
        virtual ~RMCTransaction();
        
    public:
        virtual bool WantsSignal (std::shared_ptr<fsm::Signal> signal) { return true; }
        virtual bool HandleSignal (std::shared_ptr<fsm::Signal> signal);

    private:
        BusinessServer *m_pBusinessServer;
        EntityFactory_RMCDTG *m_pFactoryRMCDTG;
        EntityFactory_RMCFH *m_pFactoryRMCFH;
        EntityFactory_RMCPM25 *m_pFactoryRMCPM25;
        EntityFactory_RMCWR *m_pFactoryRMCWR;
        BusinessService *m_pBusinessServiceRMCDTG;
        BusinessService *m_pBusinessServiceRMCFH;
        BusinessService *m_pBusinessServiceRMCPM25;
        BusinessService *m_pBusinessServiceRMCWR;
    };

    RMCTransaction::RMCTransaction()
        : fsm::Transaction()
        ,m_pBusinessServer(NULL)
        ,m_pFactoryRMCDTG(NULL)
        ,m_pFactoryRMCFH(NULL)
        ,m_pFactoryRMCPM25(NULL)
        ,m_pFactoryRMCWR(NULL)
        ,m_pBusinessServiceRMCDTG(NULL)
        ,m_pBusinessServiceRMCFH(NULL)
        ,m_pBusinessServiceRMCPM25(NULL)
        ,m_pBusinessServiceRMCWR(NULL)
    {
        BSFWK_LOG_GEN_PRINTF("\n");
        m_pBusinessServer = new BusinessServer();

		m_pFactoryRMCDTG = new EntityFactory_RMCDTG();
	    m_pBusinessServiceRMCDTG = new BusinessService(m_pBusinessServer, m_pFactoryRMCDTG);
	    m_pBusinessServer->AddService(m_pBusinessServiceRMCDTG);
        //m_pBusinessServiceRMCDTG->SetLogLevel(LogLevel_Verbose);
		m_pBusinessServiceRMCDTG->Start();

		m_pFactoryRMCFH = new EntityFactory_RMCFH();
		m_pBusinessServiceRMCFH = new BusinessService(m_pBusinessServer, m_pFactoryRMCFH);
		m_pBusinessServer->AddService(m_pBusinessServiceRMCFH);
        //m_pBusinessServiceRMCFH->SetLogLevel(LogLevel_Warning);
		m_pBusinessServiceRMCFH->Start();

		m_pFactoryRMCPM25 = new EntityFactory_RMCPM25();
		m_pBusinessServiceRMCPM25 = new BusinessService(m_pBusinessServer, m_pFactoryRMCPM25);
		m_pBusinessServer->AddService(m_pBusinessServiceRMCPM25);
        //m_pBusinessServiceRMCPM25->SetLogLevel(LogLevel_Warning);
		m_pBusinessServiceRMCPM25->Start();

		m_pFactoryRMCWR = new EntityFactory_RMCWR();
		m_pBusinessServiceRMCWR = new BusinessService(m_pBusinessServer, m_pFactoryRMCWR);
		m_pBusinessServer->AddService(m_pBusinessServiceRMCWR);
        //m_pBusinessServiceRMCWR->SetLogLevel(LogLevel_Warning);
		m_pBusinessServiceRMCWR->Start();

        g_PostSignalEvent_Driver = [this](std::shared_ptr<fsm::Signal> signal) {
            this->m_pBusinessServer->PostSignalEvent(signal);
		};
    }

    RMCTransaction::~RMCTransaction()
    {
        BSFWK_LOG_GEN_PRINTF("\n");
        g_PostSignalEvent_Driver = NULL;
        
        m_pBusinessServiceRMCDTG->Stop();
        m_pBusinessServiceRMCFH->Stop();
        m_pBusinessServiceRMCPM25->Stop();
        m_pBusinessServiceRMCWR->Stop();
    
        delete m_pBusinessServiceRMCWR;
        delete m_pBusinessServiceRMCPM25;
        delete m_pBusinessServiceRMCFH;
        delete m_pBusinessServiceRMCDTG;
        delete m_pFactoryRMCWR;
        delete m_pFactoryRMCPM25;
        delete m_pFactoryRMCFH;
        delete m_pFactoryRMCDTG;
        delete m_pBusinessServer;
    }

    bool RMCTransaction::HandleSignal (std::shared_ptr<fsm::Signal> signal)
    {
        BSFWK_LOG_GEN_PRINTF("\n");
        if (m_pBusinessServer != NULL) {
            BSFWK_LOG_GEN_PRINTF("\n");
            m_pBusinessServer->PostSignalEvent(signal);
        }

        return true;
    }

    std::shared_ptr<fsm::Transaction> CreateRMCTransaction()
    {
        return std::make_shared<RMCTransaction>();
    }
#endif // #ifdef TCAM_TARGET

} // namespace volvo_on_call {

