#ifndef BSFWK_I_ENTITY_FACTORY_H
#define BSFWK_I_ENTITY_FACTORY_H

#include "bsfwk_Common.h"

namespace bsfwk {

    class IEntityFactory
    {
    public:

        virtual ~IEntityFactory() {}

        /**
        * @brief                                  Function called by ServiceStateMachine when to create a ServiceEntity instance.
        *
        * @param[in] pServiceStateMachine         The ServiceStateMachine instance.
        *
        * @return                                 The ServiceEntity instance.
        */
        virtual std::shared_ptr<IServiceEntity> CreateServiceEntity(IServiceStateMachine &/*rServiceStateMachine*/) = 0;

        /**
        * @brief                                  Function called by JobStateMachine when to create a JobEntity instance.
        *
        * @param[in] pServiceStateMachine         The ServiceStateMachine instance.
        * @param[in] pJobStateMachine             The JobStateMachine instance.
        * @param[in] nIndex                       The index of the JobEntity to create.
        *
        * @return                                 The JobEntity instance.
        */
        virtual std::shared_ptr<IJobEntity> CreateJobEntity(IServiceStateMachine &/*rServiceStateMachine*/, IJobStateMachine &/*rJobStateMachine*/, uint32_t /*nIndex*/) = 0;

        /**
        * @brief                                  Function called by ServiceStateMachine when to create the JobEntity instances.
        *
        * @return                                 The count of the JobEntity instance.
        */
        virtual const uint32_t GetJobCount() const = 0;

        /**
        * @brief                                  Function called to get the service state machine name.
        *
        * @return                                 The service state machine name.
        */
        virtual const std::string GetServiceStateMachineName() = 0;

        /**
        * @brief                                  Function called to get the job state machine name.
        * @param[in] nIndex                       The index of the JobEntity to create.
        *
        * @return                                 The job state machine name.
        */
        virtual const std::string GetJobStateMachineName(const uint32_t /*nIndex*/) = 0;
    };

} // namespace bsfwk {

#endif // #ifndef BSFWK_I_ENTITY_FACTORY_H
