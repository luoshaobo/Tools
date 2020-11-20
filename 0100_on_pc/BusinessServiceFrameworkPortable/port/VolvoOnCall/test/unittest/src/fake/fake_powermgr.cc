#include "powermgr.h"

tpPM_ReturnType_t tpPM_getPowerMode(tpVCS_PowerState_t *powerMode)
{
  *powerMode = E_VCS_POWER_STATE_NORMAL;
  return E_PM_RET_SUCCESS;
}

