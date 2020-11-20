#include "BigDataServiceGlobal.h"
#include "BigDataServiceConfigManager.h"

namespace volvo_on_call {
namespace bds {

#ifdef WINDOWS
    static const char *BIG_DATA_SERVICE_CONFIG_FILE_PATH = "../config/BigDataServiceConfigs.xml";
#else
    static const char *BIG_DATA_SERVICE_CONFIG_FILE_PATH = "/data/BigDataServiceConfigs.xml";
#endif // #ifdef WINDOWS

    static const char *XML_TAG__NODE_CONFIGS = "Configs";
    static const char *XML_TAG__NODE_CONFIG = "Config";
    static const char *XML_TAG__NODE_CONFIG__ATTR_ID = "id";
    static const char *XML_TAG__NODE_CONFIG__ATTR_VERSION = "version";
    static const char *XML_TAG__NODE_CONFIG__ATTR_TARGET = "target";
    static const char *XML_TAG__NODE_CONFIG__ATTR_UPLOAD_CYCLE = "uploadCycle";
    static const char *XML_TAG__NODE_CONFIG__ATTR_COLLECT_CYCLE = "collectCycle";
    static const char *XML_TAG__NODE_CONFIG__ATTR_CONDITION = "condition";
    static const char *XML_TAG__NODE_CONFIG__ATTR_TRIGGER_TIMES = "triggerTimes";

    BigDataServiceConfigManager::BigDataServiceConfigManager()
        : m_configs()
        , m_prodeFuncs()
    {

    }

    BigDataServiceConfigManager::~BigDataServiceConfigManager()
    {

    }

    BigDataServiceConfigManager &BigDataServiceConfigManager::getInstance()
    {
        static BigDataServiceConfigManager bigDataServiceConfigManager;
        return bigDataServiceConfigManager;
    }

    bool BigDataServiceConfigManager::loadConfigs()
    {
        bool ret = true;
        pugi::xml_document xmlDoc;
        pugi::xml_parse_result xmlResult;

        xmlResult = xmlDoc.load_file(BIG_DATA_SERVICE_CONFIG_FILE_PATH);
        if (!xmlResult) {
            return false;
        }

        for (pugi::xml_node xmlConfig : xmlDoc.child(XML_TAG__NODE_CONFIGS).children(XML_TAG__NODE_CONFIG)) {
            std::shared_ptr<BigDataServiceConfig> config(std::make_shared<BigDataServiceConfig>());
            if (config != nullptr) {
                config->configId = xmlConfig.attribute(XML_TAG__NODE_CONFIG__ATTR_ID).as_string();
                config->version = xmlConfig.attribute(XML_TAG__NODE_CONFIG__ATTR_VERSION).as_uint();
                config->selectedPortNames = xmlConfig.attribute(XML_TAG__NODE_CONFIG__ATTR_TARGET).as_string();
                config->collectingDuration = xmlConfig.attribute(XML_TAG__NODE_CONFIG__ATTR_COLLECT_CYCLE).as_uint();
                config->uploadingDuration = xmlConfig.attribute(XML_TAG__NODE_CONFIG__ATTR_UPLOAD_CYCLE).as_uint();
                config->condition = xmlConfig.attribute(XML_TAG__NODE_CONFIG__ATTR_CONDITION).as_string();
                config->triggeringCount = xmlConfig.attribute(XML_TAG__NODE_CONFIG__ATTR_TRIGGER_TIMES).as_uint();
                prodeServiceConfig(*config);
                m_configs.push_back(config);
            }
        }

        return ret;
    }

    bool BigDataServiceConfigManager::saveConfigs()
    {
        bool ret = true;
        pugi::xml_document xmlDoc;
        pugi::xml_parse_result xmlResult;

        pugi::xml_node xmlConfigs = xmlDoc.append_child(XML_TAG__NODE_CONFIGS);
        for (std::shared_ptr<BigDataServiceConfig> &config : m_configs) {
            if (config != nullptr) {
                pugi::xml_node xmlConfig = xmlConfigs.append_child(XML_TAG__NODE_CONFIG);
                xmlConfig.append_attribute(XML_TAG__NODE_CONFIG__ATTR_ID) = config->configId.c_str();
                xmlConfig.append_attribute(XML_TAG__NODE_CONFIG__ATTR_VERSION) = config->version;
                xmlConfig.append_attribute(XML_TAG__NODE_CONFIG__ATTR_TARGET) = config->selectedPortNames.c_str();
                xmlConfig.append_attribute(XML_TAG__NODE_CONFIG__ATTR_COLLECT_CYCLE) = config->collectingDuration;
                xmlConfig.append_attribute(XML_TAG__NODE_CONFIG__ATTR_UPLOAD_CYCLE) = config->uploadingDuration;
                xmlConfig.append_attribute(XML_TAG__NODE_CONFIG__ATTR_CONDITION) = config->condition.c_str();
                xmlConfig.append_attribute(XML_TAG__NODE_CONFIG__ATTR_TRIGGER_TIMES) = config->triggeringCount;
            }
        }

        if (!xmlDoc.save_file(BIG_DATA_SERVICE_CONFIG_FILE_PATH, "    ")) {
            return false;
        }

        FileSync();

        return ret;
    }

    void BigDataServiceConfigManager::FileSync()
    {
#ifndef WINDOWS
        sync();
#endif // #ifndef WINDOWS
    }

    std::vector<std::shared_ptr<BigDataServiceConfig> > &BigDataServiceConfigManager::getConfigs()
    {
        return m_configs;
    }

    void BigDataServiceConfigManager::addConfig(const std::shared_ptr<BigDataServiceConfig> &config)
    {
        std::shared_ptr<BigDataServiceConfig> configTmp(config);

        if (configTmp != nullptr) {
            prodeServiceConfig(*configTmp);

            for (auto it = m_configs.begin(); it != m_configs.end(); ++it) {
                if ((*it)->configId == configTmp->configId) {
                    *it = configTmp;
                    return;
                }
            }

            m_configs.push_back(configTmp);
        }
    }

    void BigDataServiceConfigManager::removeConfig(const std::string &configId)
    {
        for (auto it = m_configs.begin(); it != m_configs.end(); ++it) {
            if ((*it)->configId == configId) {
                m_configs.erase(it);
                break;
            }
        }
    }

    void BigDataServiceConfigManager::addServiceConfigProdeFunc(ServiceConfigProdeFunc func)
    {
        m_prodeFuncs.push_back(func);
    }

    void BigDataServiceConfigManager::removeAllServiceConfigProdeFuncs()
    {
        m_prodeFuncs.clear();
    }

    bool BigDataServiceConfigManager::isConfigValid(const BigDataServiceConfig &config)
    {
        bool ret = false;
        BigDataServiceConfig configTemp = config;

        for (ServiceConfigProdeFunc &func : m_prodeFuncs) {
            if (func != nullptr) {
                if (func(configTemp)) {
                    ret = true;
                    break;
                }
            }
        }

        return ret;
    }

    bool BigDataServiceConfigManager::prodeServiceConfig(BigDataServiceConfig &config)
    {
        bool ret = false;

        for (ServiceConfigProdeFunc &func : m_prodeFuncs) {
            if (func != nullptr) {
                if (func(config)) {
                    ret = true;
                    break;
                }
            }
        }

        return ret;
    }

    std::string BigDataServiceConfigManager::trim(const std::string &str, const std::string& whitespace /*= " \t"*/)
    {
        const auto strBegin = str.find_first_not_of(whitespace);
        if (strBegin == std::string::npos) {
            return std::string("");
        }
        
        const auto strEnd = str.find_last_not_of(whitespace);
        const auto strRange = strEnd - strBegin + 1;
        
        return str.substr(strBegin, strRange);
    }

    std::vector<std::string> BigDataServiceConfigManager::split(const std::string& s, char delimiter)
    {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);

        while (std::getline(tokenStream, token, delimiter))
        {
            tokens.push_back(token);
        }

        return tokens;
    }

    std::vector<std::string> BigDataServiceConfigManager::splitWithTrim(const std::string& s, char delimiter, const std::string& whitespace /*= " \t"*/)
    {
        std::vector<std::string> resultSS;
        std::vector<std::string> ss = split(s, delimiter);

        for (std::string &s : ss) {
            s = trim(s, whitespace);
            if (!s.empty()) {
                resultSS.push_back(s);
            }
        }

        return resultSS;
    }

} // namespace bds {
} // namespace volvo_on_call {
