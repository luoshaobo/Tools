#ifndef BIG_DATA_SERVICE_CONFIG_MANAGER
#define BIG_DATA_SERVICE_CONFIG_MANAGER

#include "BigDataServiceGlobal.h"
#include "BigDataServiceConfig.h"

namespace volvo_on_call {
namespace bds {

    class BigDataServiceConfigManager
    {
    public:
        typedef std::function<bool (BigDataServiceConfig &)> ServiceConfigProdeFunc;

    public:
        static BigDataServiceConfigManager &getInstance();

    public:
        ~BigDataServiceConfigManager();

    public:
        bool loadConfigs();
        bool saveConfigs();

    public:
        std::vector<std::shared_ptr<BigDataServiceConfig> > &getConfigs();
        void addConfig(const std::shared_ptr<BigDataServiceConfig> &config);
        void removeConfig(const std::string &configId);
        bool isConfigValid(const BigDataServiceConfig &config);
        void addServiceConfigProdeFunc(ServiceConfigProdeFunc func);
        void removeAllServiceConfigProdeFuncs();

    public:
        static std::string trim(const std::string &str, const std::string& whitespace = " \t");
        static std::vector<std::string> split(const std::string& s, char delimiter);
        static std::vector<std::string> splitWithTrim(const std::string& s, char delimiter, const std::string& whitespace = " \t");

    private:
        bool prodeServiceConfig(BigDataServiceConfig &config);
        void FileSync();

    private:
        BigDataServiceConfigManager();

    private:
        std::vector<std::shared_ptr<BigDataServiceConfig> > m_configs;
        std::vector<ServiceConfigProdeFunc> m_prodeFuncs;
    };

} // namespace bds {
} // namespace volvo_on_call {

#endif // #ifndef BIG_DATA_SERVICE_CONFIG_MANAGER
