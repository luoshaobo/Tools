#ifndef BSFWK_ENTITY_FACTORY_BASE_H
#define BSFWK_ENTITY_FACTORY_BASE_H

#include "bsfwk_Common.h"
#include "bsfwk_IEntityFactory.h"

namespace bsfwk {

    class EntityFactoryBase : public IEntityFactory
    {
    public:
        virtual std::shared_ptr<IServiceEntity> CreateServiceEntity(IServiceStateMachine &/*rServiceStateMachine*/) { return 0; }
        virtual std::shared_ptr<IJobEntity> CreateJobEntity(IServiceStateMachine &/*rServiceStateMachine*/, IJobStateMachine &/*rJobStateMachine*/, uint32_t /*nIndex*/) { return 0; }
        virtual const uint32_t GetJobCount() const { return static_cast<uint32_t>(0); }
        virtual const std::string GetServiceStateMachineName() { return std::string("UnknownServiceStateMachine"); }
        virtual const std::string GetJobStateMachineName(const uint32_t /*nIndex*/) { return std::string("UnknownJobStateMachine"); }
    };

    template <typename ServiceEntityClass>
    class EntityFactoryWithoutJob : public IEntityFactory
    {
    public:
        EntityFactoryWithoutJob(const std::string &sServiceName = "UnknownService") : m_sServiceName(sServiceName) {}

        virtual std::shared_ptr<IServiceEntity> CreateServiceEntity(IServiceStateMachine &rServiceStateMachine) { m_pServiceEntity = std::make_shared<ServiceEntityClass>(rServiceStateMachine, m_sServiceName); return m_pServiceEntity; }
        virtual std::shared_ptr<IJobEntity> CreateJobEntity(IServiceStateMachine &/*rServiceStateMachine*/, IJobStateMachine &/*rJobStateMachine*/, uint32_t /*nIndex*/) { return 0; }
        virtual const uint32_t GetJobCount() const { return static_cast<uint32_t>(0); }
        virtual const std::string GetServiceStateMachineName() { return m_sServiceName + std::string("UnknownServiceStateMachine"); }
        virtual const std::string GetJobStateMachineName(const uint32_t /*nIndex*/) { return std::string("UnknownJobStateMachine"); }

        std::shared_ptr<ServiceEntityClass> GetServiceEntity() { return m_pServiceEntity; }

    private:
        const std::string m_sServiceName;
        std::shared_ptr<ServiceEntityClass> m_pServiceEntity;
    };

} // namespace bsfwk {

#endif // #ifndef BSFWK_ENTITY_FACTORY_BASE_H
