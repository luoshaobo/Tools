  /**
 * \file
 *          entNavigation.cpp
 * \brief
 *          Access Library to NavBL/Engine.
 *
 * \par Author (last changes):
 *          - Jame Ng
 *          - Phone +65 67799 630
 *          - ng.jame@continental-corporation.com
 *
 * \par Responsible Developer:
 *          - Jame Ng
 *          - Phone +65 67799 630
 *          - ng.jame@continental-corporation.com
 *
 * \par Project Leader:
 *          - Mandy Sin
 *          - Phone +65 67799 819
 *          - Mandy.Sin@continental-corporation.com
 *
 * \par Responsible Architect:
 *          - Goh Hoe Chun
 *          - Phone +65 67799 586
 *          - Hoe.Chun.Goh@continental-corporation.com
 *
 * \par Project:
 *      DPCA B73 Navi
 *
 * \par SW-Component:
 *      -
 *
 * \par SW-Package:
 *      -
 *
 * \par SW-Module:
 *      -
 *
 * \par SW-Architecture Specification:
 *      -
 *
 * \par SW-Design Specification:
 *      -
 *
 * \par SW-Test Case Specification:
 *      -
 *
 * \par CM-Info:
 *      -
 *
 * \note
 *      This is a note.
 *
 * \bug
 *      - there are no known bugs
 *
 * \par Module-History:
 *
 * \verbatim
 * Date                Author                  Reason
 * 08 Nov 2012         Jianwei Zhang           Update according to DD 08a released
 * 27 Aug 2012         Jame ng                 update according to DD 7b released
 * 27 Aug 2012         Jame ng                 Code Clean up and SCC fix
 * 30 May 2012         Jame Ng                 Doxygen warning Fix
 * 20 Sept 2011        Wong Chee Vui           Initial Creation
 * The latest entry at the head of the history list.
 * \endverbatim
 *
 * \par Copyright Notice:
 * Copyright (C) 2011 Continental Automotive Singapore Pte Ltd.
 * Alle Rechte vorbehalten. All Rights Reserved.\n
 * The reproduction, transmission or use of this document or its contents is
 * not permitted without express written authority.\n
 * Offenders will be liable for damages. All rights, including rights created
 * by patent grant or registration of a utility model or design, are reserved.
 *
 */

#include "stdafx.h"
#include "string.h"
#include "stdio.h"
#include "entNavigation.h"
#include "entNavigation_user.h"
#define USER_TEST
#include "navLogTrace.h"
#include "navIoctl.h"

HMINavigationAccessLib::HMINavigationAccessLib()
{
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-Constructor");

    //check if the handle has been created
    if(ghINav == NIL)
    {
        ghINav = CreateFile( TEXT("NAV0:")
                            , (GENERIC_READ | GENERIC_WRITE)
                            , (FILE_SHARE_READ | FILE_SHARE_WRITE)
                            , NIL, NIL, NIL, NIL
                            );
        if(ghINav == NIL)
        {
          LOG_PRINT(DPCA_NAV_MAIN_ERROR,"NAV:AL-ghINav CreateFile returned INVALID_HANDLE_VALUE, GetLastError: 0x%X\n",GetLastError());
        }
        //check if the handle has been created
        if(NavIntitilisedReady == NIL)
        {
            //String created for the event. It is the same as the one in Navserver.cpp where NavBL will set this event.
            wchar_t EVT_READY_NAV_INITIALISED_READY [] = L"EVT_READY_NAV_INITIALISED_READY ";
            NavIntitilisedReady = CreateEvent(
                                               NIL                              //security attributes, must be NULL
                                             , TRUE                            //ManualReset True for Manual reset, False for autoreset
                                             , FALSE                            //Initial state, TRUE means signaled
                                             , EVT_READY_NAV_INITIALISED_READY  //LPTSTR lpName, the name of the event
                                                );
        }
        //check if the handle has problem being created
        if(NavIntitilisedReady == NIL)
        {
          LOG_PRINT(DPCA_NAV_MAIN_ERROR,"NAV:AL-ghINav CreateEvent returned INVALID_HANDLE_VALUE, GetLastError: 0x%X\n",GetLastError());
        }
    }
}

HMINavigationAccessLib::~HMINavigationAccessLib()
{
    try
    {
        LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-Destructor\n");
    }

    catch(...)
    {

    }
}

HMINavigationAccessLib& HMINavigationAccessLib::getInstance()
{
  static HMINavigationAccessLib navAccessLib;
  return navAccessLib;
}


BOOL HMINavigationAccessLib::entINavigation_CheckTestResult(BOOL* testResult) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
    BOOL  fRetVal = TRUE;
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-entINavigation_CheckTestResult\n");

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_CHECK_TEST_RESULT,
                                NIL, NIL,
                                testResult, sizeof(BOOL),
                                NIL, NIL
                                );
    return fRetVal;
}

BOOL HMINavigationAccessLib::entINavigation_InitTest(void) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
    BOOL  fRetVal = TRUE;
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-entINavigation_InitTest\n");

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_INIT_TEST,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );
    return fRetVal;
}

BOOL HMINavigationAccessLib::entINavigation_Initialize(void) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
    BOOL  fRetVal = TRUE;

    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-entINavigation_Initializing NOT USED\n");

    return fRetVal;
}

BOOL HMINavigationAccessLib::entNavCheckNullPointer(PBYTE pointerCheck) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
    BOOL retValue= TRUE;

    if (pointerCheck==NIL)
    {
        LOG_PRINT(DPCA_NAV_MAIN_ERROR,"NAV:AL-Pointer Passed from HMI Null\n");
        retValue = FALSE;
    }

    return retValue;
}


BOOL HMINavigationAccessLib::command_StartRouteCalculation() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_StartRouteCalculation();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-StartRouteCalculation\n");

    BOOL fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_START_ROUTE_CALCULATION,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_AbortRouteCalculation() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_AbortRouteCalculation();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-AbortRouteCalculation\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                            ghINav, IOCTL_NAV_ABORT_ROUTE_CALCULATION,
                            NIL, NIL,
                            NIL, NIL,
                            NIL, NIL
                            );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetSimulationSpeed(enum_SimulationSpeed speed) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetSimulationSpeed(speed);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetSimulationSpeed\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                            ghINav, IOCTL_NAV_SET_SIMULATION_SPEED,
                            static_cast<LPVOID>(&speed), static_cast<DWORD>(sizeof(speed)),
                            NIL, NIL,
                            NIL, NIL
                            );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetSimulationSpeed() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetSimulationSpeed();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetSimulationSpeed\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_SIMULATION_SPEED,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetDestination(struct_DestinationInfo* destinationInfo) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetDestination(destinationInfo);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetDestination\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(destinationInfo))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_SET_DESTINATION,
                                static_cast<LPVOID>(destinationInfo), static_cast<DWORD>(sizeof(struct_DestinationInfo)),
                                NIL, NIL,
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetDestinationInfo() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetDestinationInfo();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetDestinationInfo\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_DESTINATION_INFO,
                      NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetDestinationInfoData(struct_LocationInfo* destination) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetDestinationInfoData(destination);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetDestinationInfoData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(destination))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_DESTINATION_INFO_DATA,
                                NIL, NIL,
                      static_cast<LPVOID>(destination), static_cast<DWORD>(sizeof(struct_LocationInfo)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetRouteInfo(enum_RouteOptions routeOption) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetRouteInfo(routeOption);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetRouteInfo\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_ROUTE_INFO,
                                static_cast<LPVOID>(&routeOption),static_cast<DWORD>(sizeof(enum_RouteOptions)),
                                NIL, NIL,
                                NIL, NIL
                              );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetRouteInfoData(struct_RouteInfo* routeInfo) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetRouteInfoData(routeInfo);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetRouteInfoData\n");

    BOOL  fRetVal = TRUE;

    if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(routeInfo))==TRUE)
    {
        fRetVal = DeviceIoControl(
                                    ghINav, IOCTL_NAV_GET_ROUTE_INFO_DATA,
                                    NIL, NIL,
                                    static_cast<LPVOID>(routeInfo), static_cast<DWORD>(sizeof(struct_RouteInfo)),
                                    NIL, NIL
                                  );
    }
    else
    {
        fRetVal = FALSE;
    }

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_StartGuidance(BOOL simulation) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_StartGuidance(simulation);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-StartGuidance\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_START_GUIDANCE,
                    static_cast<LPVOID>(&simulation), static_cast<DWORD>(sizeof(simulation)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_StopGuidance() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_StopGuidance();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-StopGuidance\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_STOP_GUIDANCE,
                      NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

//SWDD07i
BOOL HMINavigationAccessLib::command_GetGuidanceStatus(enum_GuidanceStatus * guidanceStatus) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetGuidanceStatus(guidanceStatus);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetGuidanceStatus\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_GUIDANCE_STATUS,
                              NIL, NIL,
                              static_cast<LPVOID>(guidanceStatus), static_cast<DWORD>(sizeof(enum_GuidanceStatus)),
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetManeuverData(struct_ManeuverInfo* maneuver) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetManeuverData(maneuver);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetManeuverData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(maneuver))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_MANUVER_DATA,
                                NIL, NIL,
                        static_cast<LPVOID>(maneuver), static_cast<DWORD>(sizeof(struct_ManeuverInfo)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetMultiManeuverData(struct_MultipleManeuverInfo* multiManeuver) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetMultiManeuverData(multiManeuver);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetMultiManeuverData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(multiManeuver))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_MULTI_MANUVER_DATA,
                                NIL, NIL,
                        static_cast<LPVOID>(multiManeuver), static_cast<DWORD>(sizeof(struct_MultipleManeuverInfo)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_RepeatLastAnnouncement() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_RepeatLastAnnouncement();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-RepeatLastAnnouncement\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_REPEAT_LAST_ANNOUNCEMENT,
                      NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetManeuverInfo() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetManeuverInfo();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetManeuverInfo\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_MANEUVER_INFO,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}


BOOL HMINavigationAccessLib::command_ResumeGuidance() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_ResumeGuidance();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-ResumeGuidance\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_RESUME_GUIDANCE,
                      NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}


BOOL HMINavigationAccessLib::command_GetRouteListElements(short index, short no_items) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetRouteListElements(index, no_items);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetRouteListElements\n");

  BOOL  fRetVal = TRUE;

  //TODO: Temporary solution till DD is defined
  struct_Pack packRouteListElement;
  packRouteListElement.index= index;
  packRouteListElement.noOfItems= no_items;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_ROUTE_LIST_ELEMENTS,
                              static_cast<LPVOID>(&packRouteListElement),static_cast<DWORD>(sizeof(packRouteListElement)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetRouteListElementsData(struct_RouteList* routeList) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetRouteListElementsData(routeList);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetRouteListElementsData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(routeList))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_ROUTE_LIST_ELEMENTS_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(routeList),static_cast<DWORD>(sizeof(struct_RouteList)),
                                NIL, NIL
                                );
  }

  else
  {
    fRetVal = FALSE;
  }

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SelectRouteListElement(short index) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SelectRouteListElement(index);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SelectRouteListElement\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_SELECT_ROUTE_LIST_ELEMENT,
                                static_cast<LPVOID>(&index), static_cast<DWORD>(sizeof(index)),
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_StopUpdateRouteList() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_StopUpdateRouteList();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-StopUpdateRouteList\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_STOP_UPDATE_ROUTE_LIST,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

//SWDD_09b change param
BOOL HMINavigationAccessLib::command_CalculateDetour(enum_ListType listType) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_CalculateDetour(listType);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-CalculateDetour\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_CALCULATE_DETOUR,
                              static_cast<LPVOID>(&listType), static_cast<DWORD>(sizeof(enum_ListType)),
                              NIL, NIL,
                              NIL, NIL
                              );


  return fRetVal;
}

//SWDD_09b remove
//BOOL HMINavigationAccessLib::command_GetDetourStatus() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
//{
//  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetDetourStatus\n");
//
//  BOOL  fRetVal = TRUE;
//
//  fRetVal = DeviceIoControl(
//                              ghINav, IOCTL_NAV_GET_DETOUR_STATUS,
//                    NIL, NIL,
//                              NIL, NIL,
//                              NIL, NIL
//                              );
//
//
//  return fRetVal;
//}

BOOL HMINavigationAccessLib::command_GetElevatedRoadList() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetElevatedRoadList();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetElevatedRoadList\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_ELEVATED_ROAD_LIST,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetElevatedRoadListElementsData(struct_ElevatedRoadsList* ElevatedRouteList) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetElevatedRoadListElementsData(ElevatedRouteList);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetElevatedRoadListElementsData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(ElevatedRouteList))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_ELEVATED_ROAD_LIST_ELEMENTS_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(ElevatedRouteList),static_cast<DWORD>(sizeof(struct_ElevatedRoadsList)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SelectElevatedRoad(short index) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SelectElevatedRoad(index);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SelectElevatedRoad\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_SELECT_ELEVATED_ROAD,
                              static_cast<LPVOID>(&index),static_cast<DWORD>(sizeof(index)),
                              NIL, NIL,
                              NIL, NIL
                              );


  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetElevatedRoadStatus() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetElevatedRoadStatus();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetElevatedRoadStatus\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_ELEVATED_ROAD_STATUS,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_StartAdressInput() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_StartAdressInput();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-StartAdressInput\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_START_ADDRESS_INPUT,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetInputEngine(enum_InputEngineType engineType) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetInputEngine(engineType);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetInputEngine\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_SET_INPUT_ENGINE,
                              static_cast<LPVOID>(&engineType), static_cast<DWORD>(sizeof(engineType)),
                              NIL, NIL,
                              NIL, NIL
                              );


  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetInputEngine() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetInputEngine();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetInputEngine\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_INPUT_ENGINE,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );


  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetInputField(enum_InputFieldType inputField) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetInputField(inputField);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetInputField\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_SET_INPUT_FIELD,
                              static_cast<LPVOID>(&inputField), static_cast<DWORD>(sizeof(inputField)),
                              NIL, NIL,
                              NIL, NIL
                              );


  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetInputField() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetInputField();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetInputField\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_INPUT_FIELD,
                      NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );


  return fRetVal;
}

BOOL HMINavigationAccessLib::command_FinishInput() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_FinishInput();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-FinishInput\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_FINISH_INPUT,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetAddressInputLocation(struct_LocationInfo* location) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetAddressInputLocation(location);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetAddressInputLocation\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(location))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_ADDRESS_INPUT_LOCATION,
                                NIL, NIL,
                                static_cast<LPVOID>(location), static_cast<DWORD>(sizeof(struct_LocationInfo)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetAddressInputData(struct_AddressInputData* inputData) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetAddressInputData(inputData);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetAddressInputData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(inputData))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_ADDRESS_INPUT_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(inputData), static_cast<DWORD>(sizeof(struct_AddressInputData)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_StopAddressSearching() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_StopAddressSearching();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-StopAddressSearching\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_STOP_ADDRESS_SEARCHING,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetAddressSearchingStatus() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetAddressSearchingStatus();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetAddressSearchingStatus\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_ADDRESS_SEARCHING_STATUS,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}


BOOL HMINavigationAccessLib::command_SelectedCharacters(struct_InputCharacterList* inputCharacterList) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SelectedCharacters(inputCharacterList);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SelectedCharacters\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(inputCharacterList))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_SELECTED_CHARACTERS,
                                static_cast<LPVOID>(inputCharacterList), static_cast<DWORD>(sizeof(struct_InputCharacterList)),
                                NIL, NIL,
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetBestMatchList(short index, short noOfElements) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetBestMatchList(index, noOfElements);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetBestMatchList\n");

  BOOL  fRetVal = TRUE;

  //TODO: Temporary solution till DD is defined
  struct_Pack packGetBestMatchList;
  packGetBestMatchList.index= index;
  packGetBestMatchList.noOfItems= noOfElements;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_BEST_MATCH_LIST,
                              static_cast<LPVOID>(&packGetBestMatchList),static_cast<DWORD>(sizeof(packGetBestMatchList)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetBestMatchListData(struct_BestMatchList* bestMatchList) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetBestMatchListData(bestMatchList);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetBestMatchListData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(bestMatchList))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_BEST_MATCH_LIST_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(bestMatchList), static_cast<DWORD>(sizeof(struct_BestMatchList)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SelectBestMatchListElement(short index) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SelectBestMatchListElement(index);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SelectBestMatchListElement\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_SELECT_BEST_MATCH_LIST_ELEMENT,
                              static_cast<LPVOID>(&index), static_cast<DWORD>(sizeof(index)),
                              NIL, NIL,
                              NIL, NIL
                              );


  return fRetVal;
}


BOOL HMINavigationAccessLib::command_GetAlertPointConfiguration() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetAlertPointConfiguration();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetAlertPointConfiguration\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_ALERT_POINT_CONFIGURATION,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );


  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetAlertPointConfiguration(struct_AlertPointSettings* alertPointSettings) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetAlertPointConfiguration(alertPointSettings);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetAlertPointConfiguration\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(alertPointSettings))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_SET_ALERT_POINT_CONFIGURATION,
                                static_cast<LPVOID>(alertPointSettings), static_cast<DWORD>(sizeof(struct_AlertPointSettings)),
                                NIL, NIL,
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}


BOOL HMINavigationAccessLib::command_GetSpeedLimitConfiguration() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetSpeedLimitConfiguration();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetSpeedLimitConfiguration\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_SPEED_LIMIT_CONFIGURATION,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );


  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetSpeedLimitConfiguration(enum_SpeedLimitConfiguration SpeedLimitConfiguration) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetSpeedLimitConfiguration(SpeedLimitConfiguration);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetSpeedLimitConfiguration\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_SET_SPEED_LIMIT_CONFIGURATION,
                              static_cast<LPVOID>(&SpeedLimitConfiguration), static_cast<DWORD>(sizeof(SpeedLimitConfiguration)),
                              NIL, NIL,
                              NIL, NIL
                              );


  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetAlertPointFeedbackSettings() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetAlertPointFeedbackSettings();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetAlertPointFeedbackSettings\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_ALERT_POINT_FEEDBACK_SETTINGS,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );


  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetAlertPointFeedbackSettings(BOOL feedbackSettings) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetAlertPointFeedbackSettings(feedbackSettings);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetAlertPointFeedbackSettings\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_SET_ALERT_POINT_FEEDBACK_SETTINGS,
                              static_cast<LPVOID>(&feedbackSettings), static_cast<DWORD>(sizeof(feedbackSettings)),
                              NIL, NIL,
                              NIL, NIL
                              );


  return fRetVal;
}


BOOL HMINavigationAccessLib::command_DeleteStage(short index) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_DeleteStage(index);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-DeleteStage\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_DELETE_STAGE,
                              static_cast<LPVOID>(&index), static_cast<DWORD>(sizeof(index)),
                              NIL, NIL,
                              NIL, NIL
                              );



  return fRetVal;
}

BOOL HMINavigationAccessLib::command_ReorderStageList(int order[MAX_NO_STAGES]) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_ReorderStageList(order);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-ReorderStageList\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(order))==TRUE)
  {
  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_REORDER_STAGE_LIST,
                              static_cast<LPVOID>(order), static_cast<DWORD>(sizeof(order)),
                              NIL, NIL,
                              NIL, NIL
                              );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_StopUpdateStageList(BOOL stop) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_StopUpdateStageList(stop);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-StopUpdateStageList\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_STOP_UPDATE_STAGE_LIST,
                              static_cast<LPVOID>(&stop), static_cast<DWORD>(sizeof(stop)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetStageListElements(short index, short noOfElements) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetStageListElements(index, noOfElements);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetStageListElements\n");

  BOOL  fRetVal = TRUE;

  //TODO: Temporary solution till DD is defined
  struct_Pack packGetStageListElements;
  packGetStageListElements.index= index;
  packGetStageListElements.noOfItems= noOfElements;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_STAGE_LIST_ELEMENTS,
                              static_cast<LPVOID>(&packGetStageListElements),static_cast<DWORD>(sizeof(packGetStageListElements)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetStageListElementsData(struct_StageList* stages) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetStageListElementsData(stages);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetStageListElementsData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(stages))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_STAGE_LIST_ELEMENTS_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(stages), static_cast<DWORD>(sizeof(struct_StageList)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetStageListElementDetails(short index) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetStageListElementDetails(index);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetStageListElementDetails\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_STAGE_LIST_ELEMENT_DETAILS,
                              static_cast<LPVOID>(&index),static_cast<DWORD>(sizeof(index)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetStageListElementDetailsData(struct_StageListElement* element) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetStageListElementDetailsData(element);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetStageListElementDetailsData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(element))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_STAGE_LIST_ELEMENT_DETAILS_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(element), static_cast<DWORD>(sizeof(struct_StageListElement)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}


BOOL HMINavigationAccessLib::command_SetLanguageSettings(enum_LanguageSettings language) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetLanguageSettings(language);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetLanguageSettings\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_SET_LANGUAGE_SETTING,
                              static_cast<LPVOID>(&language), static_cast<DWORD>(sizeof(language)),
                              NIL, NIL,
                              NIL, NIL
                              );


  return fRetVal;
}


BOOL HMINavigationAccessLib::command_AddFavourite(struct_FavouriteElement* element) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_AddFavourite(element);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-AddFavourite\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(element))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_ADD_FAVOURITE,
                                static_cast<LPVOID>(element), static_cast<DWORD>(sizeof(struct_FavouriteElement)),
                                NIL, NIL,
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}


BOOL HMINavigationAccessLib::command_GetFavouriteListElements(short index, short noOfElements) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetFavouriteListElements(index, noOfElements);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetFavouriteListElements\n");

  BOOL  fRetVal = TRUE;

  //TODO: Temporary solution till DD is defined
  struct_Pack packGetStageListElements;
  packGetStageListElements.index= index;
  packGetStageListElements.noOfItems= noOfElements;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_FAVOURITE_LIST_ELEMENTS,
                              static_cast<LPVOID>(&packGetStageListElements),static_cast<DWORD>(sizeof(packGetStageListElements)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}


BOOL HMINavigationAccessLib::command_GetFavouriteListElementsData(struct_FavouriteList* favouriteListElements) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetFavouriteListElementsData(favouriteListElements);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetFavouriteListElementsData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(favouriteListElements))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_FAVOURITE_LIST_ELEMENTS_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(favouriteListElements), static_cast<DWORD>(sizeof(struct_FavouriteList)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetFavouriteListElementDetails(short index) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetFavouriteListElementDetails(index);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetFavouriteListElementDetails\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_FAVOURITE_LIST_ELEMENT_DETAILS,
                              static_cast<LPVOID>(&index), static_cast<DWORD>(sizeof(index)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetFavouriteListElementDetailsData(struct_FavouriteElement* element) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetFavouriteListElementDetailsData(element);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetFavouriteListElementDetailsData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(element))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_FAVOURITE_LIST_ELEMENT_DETAILS_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(element), static_cast<DWORD>(sizeof(struct_FavouriteElement)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_RenameFavouriteListElement(struct_renameFavouriteListElement* renameStructure) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_RenameFavouriteListElement(renameStructure);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-RenameFavouriteListElement\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(renameStructure))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_RENAME_FAVOURITE_LIST_ELEMENT,
                                static_cast<LPVOID>(renameStructure), static_cast<DWORD>(sizeof(struct_renameFavouriteListElement)),
                                NIL, NIL,
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_DeleteFavouriteListElements(BOOL deletedElements[FAVOURITE_LIST_SIZE]) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_DeleteFavouriteListElements(deletedElements);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-DeleteFavouriteListElements\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(deletedElements))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_DELETE_FAVOURITE_LIST_ELEMENTS,
                                static_cast<LPVOID>(deletedElements), static_cast<DWORD>(sizeof(BOOL)*FAVOURITE_LIST_SIZE),
                                NIL, NIL,
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetFavouriteListStatus() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetFavouriteListStatus();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetFavouriteListStatus\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_FAVOURITE_LIST_STATUS,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );


  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetHistoryListElements(short index,short noOfElements) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetHistoryListElements(index, noOfElements);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetHistoryListElement\n");

  BOOL  fRetVal = TRUE;

  //Temporary solution till DD is defined
  struct_Pack packGetStageListElements;
  packGetStageListElements.index= index;
  packGetStageListElements.noOfItems= noOfElements;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_HISTORY_LIST_ELEMENTS,
                              static_cast<LPVOID>(&packGetStageListElements),static_cast<DWORD>(sizeof(packGetStageListElements)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetHistoryListElementsData(struct_HistoryList* historyListElements) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetHistoryListElementsData(historyListElements);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetHistoryListElementsData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(historyListElements))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_HISTORY_LIST_ELEMENTS_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(historyListElements), static_cast<DWORD>(sizeof(struct_HistoryList)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetHistoryListElementDetails(short index) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetHistoryListElementDetails(index);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetHistoryListElementDetails\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_HISTORY_LIST_ELEMENT_DETAILS,
                              static_cast<LPVOID>(&index), static_cast<DWORD>(sizeof(index)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetHistoryListElementDetailsData(struct_HistoryElement* element) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetHistoryListElementDetailsData(element);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetHistoryListElementDetailsData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(element))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_HISTORY_LIST_ELEMENT_DETAILS_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(element), static_cast<DWORD>(sizeof(struct_HistoryElement)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_DeleteHistoryListElements(BOOL deletedElements[HISTORY_LIST_SIZE]) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_DeleteHistoryListElements(deletedElements);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-DeleteHistoryListElements\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(deletedElements))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_DELETE_HISTORY_LIST_ELEMENTS,
                                static_cast<LPVOID>(deletedElements), static_cast<DWORD>(sizeof(BOOL)*HISTORY_LIST_SIZE),
                                NIL, NIL,
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}


BOOL HMINavigationAccessLib::command_GetRouteOptions() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetRouteOptions();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetRouteOptions\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_ROUTE_OPTIONS,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetRouteOptions(enum_RouteOptions routeOption) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetRouteOptions(routeOption);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetRouteOption\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_SET_ROUTE_OPTIONS,
                              static_cast<LPVOID>(&routeOption), static_cast<DWORD>(sizeof(routeOption)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetRouteAvoidance() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetRouteAvoidance();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetRouteAvoidance\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_ROUTE_AVOIDANCE,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetRouteAvoidance(DWORD routeAvoidance) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetRouteAvoidance(routeAvoidance);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetRouteAvoidance\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_SET_ROUTE_AVOIDANCE,
                              static_cast<LPVOID>(&routeAvoidance), static_cast<DWORD>(sizeof(routeAvoidance)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetDayNightMode() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetDayNightMode();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetDayNightMode\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_DAY_NIGHT_MODE,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetDayNightMode(enum_DayNightMode dayNightMode) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetDayNightMode(dayNightMode);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetDayNightMod\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_SET_DAY_NIGHT_MODE,
                              static_cast<LPVOID>(&dayNightMode), static_cast<DWORD>(sizeof(dayNightMode)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetMapDisplayMode() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetMapDisplayMode();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetMapDisplayMode\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_MAP_DISPLAY_MODE,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetMapDisplayMode(enum_MapDisplayMode mapDisplayMode) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetMapDisplayMode(mapDisplayMode);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetMapDisplayMode\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_SET_MAP_DISPLAY_MODE,
                              static_cast<LPVOID>(&mapDisplayMode), static_cast<DWORD>(sizeof(mapDisplayMode)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetJunctionViewSettings() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetJunctionViewSettings();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetJunctionViewSettings\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_JUNCTION_VIEW_SETTINGS,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetJunctionViewSettings(BOOL junctionView) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetJunctionViewSettings(junctionView);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetJunctionViewSettings\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_SET_JUNCTION_VIEW_SETTINGS,
                              static_cast<LPVOID>(&junctionView), static_cast<DWORD>(sizeof(junctionView)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetAutozoomSettings() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetAutozoomSettings();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetAutozoomSettings\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_AUTOZOOM_SETTINGS,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetAutozoomSettings(BOOL autozoom) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetAutozoomSettings(autozoom);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetAutozoomSettings\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_SET_AUTOZOOM_SETTINGS,
                              static_cast<LPVOID>(&autozoom), static_cast<DWORD>(sizeof(autozoom)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}


BOOL HMINavigationAccessLib::command_ShowMapView(enum_MapMode mapMode) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_ShowMapView(mapMode);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-ShowMapView\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_SHOW_MAP_VIEW,
                              static_cast<LPVOID>(&mapMode), static_cast<DWORD>(sizeof(mapMode)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_MoveMap(struct_MapCoordinates* coordinates) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_MoveMap(coordinates);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-MoveMap\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(coordinates))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_MOVE_MAP,
                                static_cast<LPVOID>(coordinates), static_cast<DWORD>(sizeof(struct_MapCoordinates)),
                                NIL, NIL,
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_StopMoveMap() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_StopMoveMap();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-StopMoveMap\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_STOP_MOVE_MAP,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SelectPointOnMap(struct_MapCoordinates* coordinate) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SelectPointOnMap(coordinate);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST

LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SelectPointOnMap\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(coordinate))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_SELECT_POINT_ON_MAP,
                                static_cast<LPVOID>(coordinate), static_cast<DWORD>(sizeof(struct_MapCoordinates)),
                                NIL, NIL,
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetPointOnMapData(struct_LocationInfo* location) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetPointOnMapData(location);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetPointOnMapData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(location))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_POINT_ON_MAP_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(location), static_cast<DWORD>(sizeof(struct_LocationInfo)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_ShowPointOnMap(struct_PointOnMap* point) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_ShowPointOnMap(point);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-ShowPointOnMap\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(point))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_SHOW_POINT_ON_MAP,
                                static_cast<LPVOID>(point), static_cast<DWORD>(sizeof(struct_PointOnMap)),
                                NIL, NIL,
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_ShowRouteOnMap(enum_RouteOptions routeOptions) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_ShowRouteOnMap(routeOptions);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-ShowRouteOnMap\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_SHOW_ROUTE_ON_MAP,
                              static_cast<LPVOID>(&routeOptions), static_cast<DWORD>(sizeof(routeOptions)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetCCPInfo() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetCCPInfo();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetCCPInfo\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_CCP_INFO,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_StartMoveMap() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_StartMoveMap();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-StartMoveMap\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_START_MOVE_MAP,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetMoveMapMode() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetMoveMapMode();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetMoveMapMode\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_MOVE_MAP_MODE,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetCCPInfoData(struct_LocationInfo* location) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetCCPInfoData(location);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetCCPInfoData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(location))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_CCP_INFO_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(location), static_cast<DWORD>(sizeof(struct_LocationInfo)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetShortCCPInfo() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetShortCCPInfo();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetShortCCPInfo\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_SHORT_CCP_INFO,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}


BOOL HMINavigationAccessLib::command_GetMapBounds() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetMapBounds();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetMapBounds\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_MAP_BOUNDS,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetMapBounds(struct_MapBounds* mapBounds) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetMapBounds(mapBounds);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetMapBounds\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(mapBounds))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_SET_MAP_BOUNDS,
                                static_cast<LPVOID>(mapBounds), static_cast<DWORD>(sizeof(struct_MapBounds)),
                                NIL, NIL,
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetZoomLevel() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetZoomLevel();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetZoomLevel\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_ZOOM_LEVEL,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetZoomLevel(enum_ZoomLevel zoomLevel) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetZoomLevel(zoomLevel);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetAutozoomSettings\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_SET_ZOOM_LEVEL,
                              static_cast<LPVOID>(&zoomLevel), static_cast<DWORD>(sizeof(zoomLevel)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetCCPDisplay(BOOL showCCP) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetCCPDisplay(showCCP);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetCCPDisplay\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_SET_CCP_DISPLAY,
                              static_cast<LPVOID>(&showCCP), static_cast<DWORD>(sizeof(showCCP)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}


BOOL HMINavigationAccessLib::command_GetSearchPoiCategories(unsigned short index,unsigned short noOfElements,short parentCategoryID) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetSearchPoiCategories(index, noOfElements, parentCategoryID);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetSearchPoiCategories\n");

  BOOL  fRetVal = TRUE;

    struct_Pack_PoiSearchCategories pack;
    pack.index=index;
    pack.noOfElements=noOfElements;
    pack.parentCategoryID=parentCategoryID;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_SEARCH_POI_CATEGORIES,
                              static_cast<LPVOID>(&pack), static_cast<DWORD>(sizeof(struct_Pack_PoiSearchCategories)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetSearchPoiCategoriesData(struct_SearchPoiCategories* elements) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetSearchPoiCategoriesData(elements);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetSearchPoiCategoriesData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(elements))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_SEARCH_POI_CATEGORIES_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(elements), static_cast<DWORD>(sizeof(struct_SearchPoiCategories)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetDisplayPoiCategories(unsigned short index,unsigned short noOfElements) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetDisplayPoiCategories(index, noOfElements);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetDisplayPoiCategories\n");

  BOOL  fRetVal = TRUE;

    struct_Pack_DisplayPoiCategories pack;
    pack.index=index;
    pack.noOfElements=noOfElements;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_DISPLAY_POI_CATEGORIES,
                              static_cast<LPVOID>(&pack), static_cast<DWORD>(sizeof(struct_Pack_DisplayPoiCategories)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetDisplayPoiCategoriesData(struct_DisplayPoiCategories* elements) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetDisplayPoiCategoriesData(elements);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetDisplayPoiCategoriesData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(elements))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_DISPLAY_POI_CATEGORIES_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(elements), static_cast<DWORD>(sizeof(struct_DisplayPoiCategories)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetPoiDisplaySettings(BOOL poiDisplaySettings[MAX_DISPLAY_POI_CATEGORIES]) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetPoiDisplaySettings(poiDisplaySettings);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetPoiDisplaySettings\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(poiDisplaySettings))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_SET_POI_DISPLAY_SETTINGS,
                                static_cast<LPVOID>(poiDisplaySettings), static_cast<DWORD>(sizeof(BOOL)*MAX_DISPLAY_POI_CATEGORIES),
                                NIL, NIL,
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}


BOOL HMINavigationAccessLib::command_CreatePoiSearch() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_CreatePoiSearch();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-CreatePoiSearch\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_CREATE_POI_SEARCH,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_DestroyPoiSearch() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_DestroyPoiSearch();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-DestroyPoiSearch\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_DESTROY_POI_SEARCH,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetPoiSearchType(enum_PoiSearchType poiSearchType) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetPoiSearchType(poiSearchType);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetPoiSearchType\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_SET_POI_SEARCH_TYPE,
                              static_cast<LPVOID>(&poiSearchType), static_cast<DWORD>(sizeof(poiSearchType)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetPoiSearchCategory(unsigned short poiCategoryID) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetPoiSearchCategory(poiCategoryID);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetPoiSearchCategory %d\n", poiCategoryID);

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_SET_POI_SEARCH_CATEGORY,
                              static_cast<LPVOID>(&poiCategoryID), static_cast<DWORD>(sizeof(unsigned short)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetPoiSearchString(TCHAR poiSearchString[MAX_POI_SEARCH_STRING]) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetPoiSearchString(poiSearchString);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetPoiSearchString\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(poiSearchString))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_SET_POI_SEARCH_STRING,
                                static_cast<LPVOID>(poiSearchString), static_cast<DWORD>(sizeof(TCHAR)*MAX_POI_SEARCH_STRING),
                                NIL, NIL,
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetPoiSearchAddress(struct_LocationInfo* poiSearchLocation) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetPoiSearchAddress(poiSearchLocation);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetPoiSearchAddress\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(poiSearchLocation))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_SET_POI_SEARCH_ADDRESS,
                                static_cast<LPVOID>(poiSearchLocation), static_cast<DWORD>(sizeof(struct_LocationInfo)),
                                NIL, NIL,
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_StartPoiSearch() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_StartPoiSearch();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-StartPoiSearch\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_START_POI_SEARCH,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_StopPoiSearch() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_StopPoiSearch();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-StopPoiSearch\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_STOP_POI_SEARCH,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetPoiSearchElements(short index, short noOfElements) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetPoiSearchElements(index, noOfElements);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetPoiSearchElements\n");

  BOOL  fRetVal = TRUE;

  //TODO: Temporary solution till DD is defined
  struct_Pack pack;
  pack.index= index;
  pack.noOfItems= noOfElements;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_POI_SEARCH_ELEMENTS,
                              static_cast<LPVOID>(&pack),static_cast<DWORD>(sizeof(pack)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetPoiSearchElementsData(struct_PoiSearchList* poiSearchElements) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetPoiSearchElementsData(poiSearchElements);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetPoiSearchElementsData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(poiSearchElements))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_POI_SEARCH_ELEMENTS_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(poiSearchElements), static_cast<DWORD>(sizeof(struct_PoiSearchList)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetPoiSearchElementDetails(short index) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetPoiSearchElementDetails(index);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetPoiSearchElementDetails\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_POI_SEARCH_ELEMENT_DETAILS,
                              static_cast<LPVOID>(&index),static_cast<DWORD>(sizeof(index)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetPoiSearchElementDetailsData(struct_PoiInfo* element) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetPoiSearchElementDetailsData(element);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetPoiSearchElementDetailsData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(element))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_POI_SEARCH_ELEMENT_DETAILS_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(element), static_cast<DWORD>(sizeof(struct_PoiInfo)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetVersionInfo(struct_NaviMapVersionInfo* sNaviMapVersion) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetVersionInfo(sNaviMapVersion);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_DEBUG,"NAV:AL-Get Version Info\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(sNaviMapVersion))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_VERSION_INFO,
                                NIL, NIL,
                                static_cast<LPVOID>(sNaviMapVersion), static_cast<DWORD>(sizeof(struct_NaviMapVersionInfo)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}




BOOL HMINavigationAccessLib::command_ResetGuidance() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_ResetGuidance();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_DEBUG,"NAV:AL-ResetGuidance\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_RESET_GUIDANCE,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetGuidanceResetStatus() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetGuidanceResetStatus();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_DEBUG,"NAV:AL-GetGuidanceResetStatus\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_GUIDANCE_RESET_STATUS,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetAlertPointSettingEnabledDisabled() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetAlertPointSettingEnabledDisabled();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_DEBUG,"NAV:AL-GetAlertPointSettingEnabledDisabled\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_ALERT_POINT_SETTING_STATUS,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetSpeedLimitSettingEnabledDisabled() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetSpeedLimitSettingEnabledDisabled();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_DEBUG,"NAV:AL-GetSpeedLimitSettingEnabledDisabled\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_SPEED_LIMIT_SETTING_STATUS,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetSpeedLimitStatus() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetSpeedLimitStatus();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_DEBUG,"NAV:AL-GetSpeedLimitStatus\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_SPEED_LIMIT_STATUS,
                              NIL, NIL,
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}


BOOL HMINavigationAccessLib::command_StartPosEngineeringMode() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_StartPosEngineeringMode();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-StartPosEngineeringMode\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_START_POS_ENGINEERING_MODE,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}


BOOL HMINavigationAccessLib::command_StopPosEngineeringMode() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_StopPosEngineeringMode();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-StopPosEngineeringMode\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_STOP_POS_ENGINEERING_MODE,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetNavigationCoreInfo() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetNavigationCoreInfo();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetNavigationCoreInfo\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_NAVIGATION_CORE_INFO,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetNavigationCoreInfoData(struct_CoreInfo* navigationCoreInfo) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetNavigationCoreInfoData(navigationCoreInfo);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_DEBUG,"NAV:AL-GetNavigationCoreInfoData\n");

    BOOL  fRetVal = TRUE;

    if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(navigationCoreInfo))==TRUE)
    {
        fRetVal = DeviceIoControl(
                                    ghINav, IOCTL_NAV_GET_NAVIGATION_CORE_INFO_DATA,
                                    NIL, NIL,
                                    static_cast<LPVOID>(navigationCoreInfo), static_cast<DWORD>(sizeof(struct_CoreInfo)),
                                    NIL, NIL
                                    );
    }

    else
    {
        fRetVal = FALSE;
    }
    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetDatabaseStatus() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetDatabaseStatus();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetDatabaseStatus\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_DATABASE_STATUS,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_SendActivationBypassFlag(char ActivationBypassFlag[ACTIVATION_BYPASS_FLAG_LENGTH]) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SendActivationBypassFlag(ActivationBypassFlag);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_DEBUG,"NAV:AL-SendActivationBypassFlag\n");

    BOOL  fRetVal = TRUE;

    if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(ActivationBypassFlag))==TRUE)
    {
        fRetVal = DeviceIoControl(
                                    ghINav, IOCTL_NAV_SEND_ACTIVATION_BYPASS_FLAG,
                                    static_cast<LPVOID>(ActivationBypassFlag), static_cast<DWORD>(sizeof(char)*ACTIVATION_BYPASS_FLAG_LENGTH),
                                    NIL, NIL,
                                    NIL, NIL
                                    );
    }

    else
    {
        fRetVal = FALSE;
    }
    return fRetVal;
}

BOOL HMINavigationAccessLib::command_StartMapActivationSequence() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_StartMapActivationSequence();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-StartMapActivationSequence\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_START_MAP_ACTIVATION_SEQUENCE,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_SendActivationCode(char activationCode[ACTIVATION_CODE_LENGTH]) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SendActivationCode(activationCode);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_DEBUG,"NAV:AL-SendActivationCode\n");

    BOOL  fRetVal = TRUE;

    if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(activationCode))==TRUE)
    {
        fRetVal = DeviceIoControl(
                                    ghINav, IOCTL_NAV_SEND_ACTIVATION_CODE,
                                    static_cast<LPVOID>(activationCode), static_cast<DWORD>(sizeof(char)*ACTIVATION_CODE_LENGTH),
                                    NIL, NIL,
                                    NIL, NIL
                                    );
    }

    else
    {
        fRetVal = FALSE;
    }
    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetMapCode() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetMapCode();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetMapCode\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_MAP_CODE,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetACCode() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetACCode();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetACCode\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_AC_CODE,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetDeviceID() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetDeviceID();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetDeviceID\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_DEVICE_ID,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_RestoreFactorySettings() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_RestoreFactorySettings();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-RestoreFactorySettings\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_RESTORE_FACTORY_SETTINGS,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_SendSystemHourFormat(enum_HourFormat format) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SendSystemHourFormat(format);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SendSystemHourFormat\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_SEND_SYSTEM_HOUR_FORMAT,
                                static_cast<LPVOID>(&format),static_cast<DWORD>(sizeof(enum_HourFormat)),
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetTMCListElements(short index, short noOfElements) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetTMCListElements(index, noOfElements);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetTMCListElements\n");

    BOOL  fRetVal = TRUE;

    //TODO: Temporary solution till DD is defined
    struct_Pack pack;
    pack.index= index;
    pack.noOfItems= noOfElements;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_TMC_LIST_ELEMENTS,
                                static_cast<LPVOID>(&pack),static_cast<DWORD>(sizeof(pack)),
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetTMCListElementsData(struct_TMCEventList* TMCListElements) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetTMCListElementsData(TMCListElements);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetTMCListElementsData\n");

      BOOL  fRetVal = TRUE;

      if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(TMCListElements))==TRUE)
      {
        fRetVal = DeviceIoControl(
                                    ghINav, IOCTL_NAV_GET_TMC_LIST_ELEMENTS_DATA,
                                    NIL, NIL,
                                    static_cast<LPVOID>(TMCListElements), static_cast<DWORD>(sizeof(struct_TMCEventList)),
                                    NIL, NIL
                                    );
       }

      else
      {
        fRetVal = FALSE;
      }
      return fRetVal;

}

BOOL HMINavigationAccessLib::command_GetTMCListElementDetails(short index) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetTMCListElementDetails(index);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetTMCListElementDetails\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_TMC_LIST_ELEMENT_DETAILS,
                                static_cast<LPVOID>(&index), static_cast<DWORD>(sizeof(index)),
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetTMCListElementDetailsData(struct_TMCEventDetails* element) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetTMCListElementDetailsData(element);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetTMCListElementDetailsData\n");

    BOOL  fRetVal = TRUE;

    if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(element))==TRUE)
    {
        fRetVal = DeviceIoControl(
                                    ghINav, IOCTL_NAV_GET_TMC_LIST_ELEMENT_DETAILS_DATA,
                                    NIL, NIL,
                                    static_cast<LPVOID>(element), static_cast<DWORD>(sizeof(struct_TMCEventDetails)),
                                    NIL, NIL
                                    );
    }

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetTMCCategoryList(short index, short noOfElements) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetTMCCategoryList(index, noOfElements);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetTMCCategoryList\n");

    BOOL  fRetVal = TRUE;

    //TODO: Temporary solution till DD is defined
    struct_Pack pack;
    pack.index= index;
    pack.noOfItems= noOfElements;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_TMC_CATEGORY_LIST,
                                static_cast<LPVOID>(&pack),static_cast<DWORD>(sizeof(pack)),
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}
BOOL HMINavigationAccessLib::command_GetTMCCategoryListData(struct_TMCCategoryList* TMCCategoryListElements) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetTMCCategoryListData(TMCCategoryListElements);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetTMCCategoryListData\n");

    BOOL  fRetVal = TRUE;

    if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(TMCCategoryListElements))==TRUE)
    {
        fRetVal = DeviceIoControl(
                                    ghINav, IOCTL_NAV_GET_TMC_CATEGORY_LIST_DATA,
                                    NIL, NIL,
                                    static_cast<LPVOID>(TMCCategoryListElements), static_cast<DWORD>(sizeof(struct_TMCCategoryList)),
                                    NIL, NIL
                                    );
    }

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_SelectTMCCategories(BOOL selectedCategories[MAX_NO_OF_TMC_CATEGORIES]) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SelectTMCCategories(selectedCategories);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SelectTMCCategories\n");

    BOOL  fRetVal = TRUE;

    if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(selectedCategories))==TRUE)
    {
        fRetVal = DeviceIoControl(
                                    ghINav, IOCTL_NAV_SELECT_TMC_CATEGORIES,
                                    static_cast<LPVOID>(selectedCategories), static_cast<DWORD>(sizeof(BOOL)*MAX_NO_OF_TMC_CATEGORIES),
                                    NIL, NIL,
                                    NIL, NIL
                                );
    }

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetTMCFilter() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetTMCFilter();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetTMCFilter\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_TMC_FILTER,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetTMCFilter(enum_TMCFilter tFilter) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetTMCFilter(tFilter);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetTMCFilter\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_SET_TMC_FILTER,
                                static_cast<LPVOID>(&tFilter), static_cast<DWORD>(sizeof(tFilter)),
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetTMCMode() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetTMCMode();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetTMCMode\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_TMC_MODE,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetTMCMode(enum_TMCMode mode) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetTMCMode(mode);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetTMCMode\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_SET_TMC_MODE,
                                static_cast<LPVOID>(&mode), static_cast<DWORD>(sizeof(mode)),
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetTMCRadius() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetTMCRadius();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetTMCRadius\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_TMC_RADIUS,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetTMCRadius(enum_TMCRadius TMCRadius) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetTMCRadius(TMCRadius);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetTMCRadius\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_SET_TMC_RADIUS,
                                static_cast<LPVOID>(&TMCRadius), static_cast<DWORD>(sizeof(TMCRadius)),
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetTMCReadoutSettings() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetTMCReadoutSettings();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetTMCReadoutSettings\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_TMC_READOUT_SETTINGS,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetTMCReadoutSettings(BOOL TMCReadout) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetTMCReadoutSettings(TMCReadout);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetTMCReadoutSettings\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_SET_TMC_READOUT_SETTINGS,
                                static_cast<LPVOID>(&TMCReadout), static_cast<DWORD>(sizeof(TMCReadout)),
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

//BOOL HMINavigationAccessLib::command_SetTMCEnableSetting(enum_TMCEnableSetting setting) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
/*{
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetTMCEnableSetting\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_SET_TMC_ENABLE_SETTINGS,
                                static_cast<LPVOID>(&setting), static_cast<DWORD>(sizeof(enum_TMCEnableSetting)),
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}*/

BOOL HMINavigationAccessLib::command_GetTMCEnableSetting() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetTMCEnableSetting();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetTMCEnableSetting\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_TMC_ENABLE_SETTINGS,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetDynaMode() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetDynaMode();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetDynaMode\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_DYNA_MODE,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetDynaMode(enum_DynaMode dynaMode) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetDynaMode(dynaMode);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetDynaMode\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_SET_DYNA_MODE,
                                static_cast<LPVOID>(&dynaMode), static_cast<DWORD>(sizeof(dynaMode)),
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_CancelJunctionView() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_CancelJunctionView();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-CancelJunctionView\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_CANCEL_JUNCTION_VIEW,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetDayNightStatus() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetDayNightStatus();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetDayNightStatus\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_DAY_NIGHT_STATUS,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

//SWDD07i
BOOL HMINavigationAccessLib::command_RecoverLastRoute() const/*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_RecoverLastRoute();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
        LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-RecoverLastRoute\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_RECOVER_LAST_ROUTE,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetCarPositionInfo(TCHAR name[MAX_SHORT_LOCATION_NAME_STRING_LENGTH]) const/*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetCarPositionInfo(name);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetCarPositionInfo\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_CAR_POSITION_INFO,
                                NIL, NIL,
                                static_cast<LPVOID>(name), static_cast<DWORD>(sizeof(TCHAR)*MAX_SHORT_LOCATION_NAME_STRING_LENGTH),
                                NIL, NIL
                                );

    return fRetVal;
}

#if (CODE_ONLY_FOR_STEP2)
BOOL HMINavigationAccessLib::command_SetNetworkPOIParameter(struct_NetPOIParam* param) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetNetworkPOIParameter(param);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetNetworkPOIParameter\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(param))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_SET_NETWORK_POI_PARAMETER,
                                static_cast<LPVOID>(param), static_cast<DWORD>(sizeof(struct_NetPOIParam)),
                                NIL, NIL,
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetNetworkPOIParameter() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetNetworkPOIParameter();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetNetworkPOIParameter\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_NETWORK_POI_PARAMETER,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetNetworkPOIParameterData(struct_NetPOIParam* param) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetNetworkPOIParameterData(param);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetNetworkPOIParameterData\n");
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetNetworkPOIParameterData = 0x%x\n",param); // Zhang Jianwei

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(param))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_NETWORK_POI_PARAMETER_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(param), static_cast<DWORD>(sizeof(struct_NetPOIParam)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_StartNetworkSearch(enum_NetSearchType nsType) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_StartNetworkSearch(nsType);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-StartNetworkSearch\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_START_NETWORK_SEARCH,
                                static_cast<LPVOID>(&nsType), static_cast<DWORD>(sizeof(nsType)),
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_AbortNetworkSearch(enum_NetSearchType type) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_AbortNetworkSearch(type);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-AbortNetworkSearch\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_ABORT_NETWORK_SEARCH,
                                static_cast<LPVOID>(&type), static_cast<DWORD>(sizeof(type)),
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetNetPOISearchElement(short index, short noOfElements) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetNetPOISearchElement(index, noOfElements);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetNetPOISearchElement\n");

  BOOL  fRetVal = TRUE;

  //TODO: Temporary solution till DD is defined
  struct_Pack packNetPOISearchElement;
  packNetPOISearchElement.index= index;
  packNetPOISearchElement.noOfItems= noOfElements;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_NET_POI_SEARCH_ELEMENT,
                              static_cast<LPVOID>(&packNetPOISearchElement),static_cast<DWORD>(sizeof(packNetPOISearchElement)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetNetPOISearchElementData(struct_NetPoiSearchList* poiSearchElements) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetNetPOISearchElementData(poiSearchElements);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetNetPOISearchElementData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(poiSearchElements))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_NET_POI_SEARCH_ELEMENT_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(poiSearchElements), static_cast<DWORD>(sizeof(struct_NetPoiSearchList)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetNetPOISearchElementDetails(short index, enum_NetSearchType poiType) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetNetPOISearchElementDetails(index, poiType);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetNetPOISearchElementDetails\n");

  BOOL  fRetVal = TRUE;

  //TODO: Temporary solution till DD is defined
  struct_Pack_GetNetPoiSearchElementDetails Pack;
  ZeroMemory(&Pack,sizeof(struct_Pack_GetNetPoiSearchElementDetails));
  Pack.index= index;
  Pack.poiType= poiType;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_NET_POI_SEARCH_ELEMENT_DETAILS,
                              static_cast<LPVOID>(&Pack),static_cast<DWORD>(sizeof(struct_Pack_GetNetPoiSearchElementDetails)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetNetPOISearchElementDetailsData(struct_NetPoiInfo* element) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetNetPOISearchElementDetailsData(element);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetNetPOISearchElementDetailsData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(element))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_NET_POI_SEARCH_ELEMENT_DETAILS_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(element), static_cast<DWORD>(sizeof(struct_NetPoiInfo)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetNetParkingSearchElementDetailsData(struct_NetParkingInfo* element) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetNetParkingSearchElementDetailsData(element);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetNetParkingSearchElementDetailsData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(element))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_NET_PARKING_SEARCH_ELEMENT_DETAILS_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(element), static_cast<DWORD>(sizeof(struct_NetParkingInfo)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetNetGasStationSearchElementDetailsData(struct_NetGasStationInfo* element) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetNetGasStationSearchElementDetailsData(element);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetNetGasStationSearchElementDetailsData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(element))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_NET_GAS_STATION_SEARCH_ELEMENT_DETAILS_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(element), static_cast<DWORD>(sizeof(struct_NetGasStationInfo)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetWeatherMode(BOOL mode) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetWeatherMode(mode);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetWeatherMode\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_SET_WEATHER_MODE,
                              static_cast<LPVOID>(&mode), static_cast<DWORD>(sizeof(mode)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetWeatherMode() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetWeatherMode();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetWeatherMode\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_WEATHER_MODE,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetWeatherData() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetWeatherData();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetWeatherData\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_WEATHER_DATA,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetWeatherDetails() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetWeatherDetails();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetWeatherDetails\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_WEATHER_DETAILS,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetWeatherDetailsData(struct_WeatherElementDetail* element) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetWeatherDetailsData(element);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetWeatherDetailsData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(element))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_WEATHER_DETAILS_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(element), static_cast<DWORD>(sizeof(struct_WeatherElementDetail)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetRouteWeather(short index, short noOfElements) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetRouteWeather(index, noOfElements);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetRouteWeather\n");

  BOOL  fRetVal = TRUE;

  //TODO: Temporary solution till DD is defined
  struct_Pack packRouteWeather;
  packRouteWeather.index= index;
  packRouteWeather.noOfItems= noOfElements;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_ROUTE_WEATHER,
                              static_cast<LPVOID>(&packRouteWeather),static_cast<DWORD>(sizeof(packRouteWeather)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetRouteWeatherData(struct_RouteWeatherList* weatherList) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetRouteWeatherData(weatherList);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetRouteWeatherData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(weatherList))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_ROUTE_WEATHER_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(weatherList), static_cast<DWORD>(sizeof(struct_RouteWeatherList)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetRouteWeatherDetails(short index) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetRouteWeatherDetails(index);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetRouteWeatherDetails\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_ROUTE_WEATHER_DETAILS,
                              static_cast<LPVOID>(&index),static_cast<DWORD>(sizeof(index)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetRouteWeatherDetailsData(struct_WeatherElementDetail* element) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetRouteWeatherDetailsData(element);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetRouteWeatherDetailsData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(element))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_ROUTE_WEATHER_DETAILS_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(element), static_cast<DWORD>(sizeof(struct_WeatherElementDetail)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_StopUpdateWeatherList() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_StopUpdateWeatherList();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-StopUpdateWeatherList\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_STOP_UPDATE_WEATHER_LIST,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_StopUpdateWeatherDetail() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_StopUpdateWeatherDetail();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-StopUpdateWeatherDetail\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_STOP_UPDATE_WEATHER_DETAIL,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetTPEGMode(enum_TPEGMode * mode) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetTPEGMode(mode);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetTPEGMode\n");

    BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(mode))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_TPEG_MODE,
                                NIL, NIL,
                                static_cast<LPVOID>(mode), static_cast<DWORD>(sizeof(enum_TPEGMode)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetTPEGMode(enum_TPEGMode mode) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetTPEGMode(mode);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetTPEGMode\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_SET_TPEG_MODE,
                                static_cast<LPVOID>(&mode), static_cast<DWORD>(sizeof(mode)),
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

//SWDD_09b change param
BOOL HMINavigationAccessLib::command_GetTPEGFilter(enum_TPEGFilter * filter) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetTPEGFilter(filter);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetTPEGFilter\n");

    BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(filter))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_TPEG_FILTER,
                                NIL, NIL,
                                static_cast<LPVOID>(filter), static_cast<DWORD>(sizeof(enum_TPEGFilter)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetTPEGFilter(enum_TPEGFilter filter) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetTPEGFilter(filter);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetTPEGFilter\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_SET_TPEG_FILTER,
                                static_cast<LPVOID>(&filter), static_cast<DWORD>(sizeof(filter)),
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

//SWDD_09b change param
BOOL HMINavigationAccessLib::command_GetTPEGRadius(enum_TPEGRadius * radius) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetTPEGRadius(radius);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetTPEGRadius\n");

    BOOL  fRetVal = TRUE;

    if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(radius))==TRUE)
    {
        fRetVal = DeviceIoControl(
                                    ghINav, IOCTL_NAV_GET_TPEG_RADIUS,
                                    NIL, NIL,
                                    static_cast<LPVOID>(radius), static_cast<DWORD>(sizeof(enum_TPEGRadius)),
                                    NIL, NIL
                                    );
    }

    else
    {
        fRetVal = FALSE;
    }

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetTPEGRadius(enum_TPEGRadius TPEGRadius) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetTPEGRadius(TPEGRadius);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetTPEGRadius\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_SET_TPEG_RADIUS,
                                static_cast<LPVOID>(&TPEGRadius), static_cast<DWORD>(sizeof(TPEGRadius)),
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

//SWDD_09b change param
BOOL HMINavigationAccessLib::command_GetTPEGReadoutSettings(BOOL * TPEGReadout) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetTPEGReadoutSettings(TPEGReadout);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetTPEGReadoutSettings\n");

    BOOL  fRetVal = TRUE;

    if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(TPEGReadout))==TRUE)
    {
        fRetVal = DeviceIoControl(
                                    ghINav, IOCTL_NAV_GET_TPEG_READOUT_SETTINGS,
                                    NIL, NIL,
                                    static_cast<LPVOID>(TPEGReadout), static_cast<DWORD>(sizeof(BOOL)),
                                    NIL, NIL
                                    );
    }

    else
    {
        fRetVal = FALSE;
    }

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetTPEGReadoutSettings(BOOL TPEGReadout) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetTPEGReadoutSettings(TPEGReadout);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetTPEGReadoutSettings\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_SET_TPEG_READOUT_SETTINGS,
                              static_cast<LPVOID>(&TPEGReadout), static_cast<DWORD>(sizeof(TPEGReadout)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetTPEGListElements(short index, short noOfElements) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetTPEGListElements(index, noOfElements);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetTPEGListElements\n");

  BOOL  fRetVal = TRUE;

  //TODO: Temporary solution till DD is defined
  struct_Pack packTPEGListElements;
  packTPEGListElements.index= index;
  packTPEGListElements.noOfItems= noOfElements;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_TPEG_LIST_ELEMENTS,
                              static_cast<LPVOID>(&packTPEGListElements),static_cast<DWORD>(sizeof(packTPEGListElements)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetTPEGListElementsData(struct_TPEGEventList* TPEGListElements) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetTPEGListElementsData(TPEGListElements);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetTPEGListElementsData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(TPEGListElements))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_TPEG_LIST_ELEMENTS_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(TPEGListElements), static_cast<DWORD>(sizeof(struct_TPEGEventList)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetTPEGListElementDetails(short index) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetTPEGListElementDetails(index);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetTPEGListElementDetails\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_TPEG_LIST_ELEMENT_DETAILS,
                              static_cast<LPVOID>(&index),static_cast<DWORD>(sizeof(index)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetTPEGListElementDetailsData(struct_TPEGEventDetails* element) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetTPEGListElementDetailsData(element);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetTPEGListElementDetailsData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(element))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_TPEG_LIST_ELEMENT_DETAILS_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(element), static_cast<DWORD>(sizeof(struct_TPEGEventDetails)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetTPEGCategoryList(short index, short noOfElements) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetTPEGCategoryList(index, noOfElements);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetTPEGCategoryList\n");

  BOOL  fRetVal = TRUE;

  //TODO: Temporary solution till DD is defined
  struct_Pack packTPEGCategoryList;
  packTPEGCategoryList.index= index;
  packTPEGCategoryList.noOfItems= noOfElements;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_TPEG_CATEGORY_LIST,
                              static_cast<LPVOID>(&packTPEGCategoryList),static_cast<DWORD>(sizeof(packTPEGCategoryList)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetTPEGCategoryListData(struct_TPEGCategoryList* TPEGCategoryListElements) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetTPEGCategoryListData(TPEGCategoryListElements);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetTPEGCategoryListData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(TPEGCategoryListElements))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_TPEG_CATEGORY_LIST_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(TPEGCategoryListElements), static_cast<DWORD>(sizeof(struct_TPEGCategoryList)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SelectTPEGCategories(BOOL selectedCategories[MAX_NO_OF_TPEG_CATEGORIES]) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SelectTPEGCategories(selectedCategories);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SelectTPEGCategories\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_SELECT_TPEG_CATEGORIES,
                              static_cast<LPVOID>(selectedCategories), static_cast<DWORD>(sizeof(BOOL)*MAX_NO_OF_TPEG_CATEGORIES),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_AcceptRoute(BOOL bAccept) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_AcceptRoute(bAccept);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-AcceptRoute\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_ACCEPT_ROUTE,
                              static_cast<LPVOID>(&bAccept), static_cast<DWORD>(sizeof(bAccept)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetTrafficInfoDynaMode(enum_DynaMode dynaMode) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetTrafficInfoDynaMode(dynaMode);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetTrafficInfoDynaMode\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_SET_TRAFFIC_INFO_DYNA_MODE,
                              static_cast<LPVOID>(&dynaMode), static_cast<DWORD>(sizeof(dynaMode)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetTrafficInfoDynaMode() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetTrafficInfoDynaMode();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetTrafficInfoDynaMode\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_TRAFFIC_INFO_DYNA_MODE,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetPCamListElements(short index, short noOfElements) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetPCamListElements(index, noOfElements);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetPCamListElements\n");

  BOOL  fRetVal = TRUE;

  //TODO: Temporary solution till DD is defined
  struct_Pack packPCamListElements;
  packPCamListElements.index= index;
  packPCamListElements.noOfItems= noOfElements;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_PCAM_LIST_ELEMENTS,
                              static_cast<LPVOID>(&packPCamListElements),static_cast<DWORD>(sizeof(packPCamListElements)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetPCamListElementsData(struct_PCamList* PCamList) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetPCamListElementsData(PCamList);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetPCamListElementsData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(PCamList))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_PCAM_LIST_ELEMENTS_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(PCamList), static_cast<DWORD>(sizeof(struct_PCamList)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetPCamListElementDetails(short index) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetPCamListElementDetails(index);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetPCamListElementDetails\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_PCAM_LIST_ELEMENT_DETAILS,
                              static_cast<LPVOID>(&index),static_cast<DWORD>(sizeof(index)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetPCamListElementDetailsData(struct_PCamDetails* element) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetPCamListElementDetailsData(element);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetPCamListElementDetailsData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(element))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_PCAM_LIST_ELEMENT_DETAILS_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(element), static_cast<DWORD>(sizeof(struct_PCamDetails)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_AddPCam(struct_PCamDetails* element) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_AddPCam(element);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-AddPCam\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(element))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_ADD_PCAM,
                                static_cast<LPVOID>(element), static_cast<DWORD>(sizeof(struct_PCamDetails)),
                                NIL, NIL,
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_DeletePCam(short index) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_DeletePCam(index);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-DeletePCam\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_DELETE_PCAM,
                              static_cast<LPVOID>(&index),static_cast<DWORD>(sizeof(index)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_RenamePCam(short index, PCamName name) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_RenamePCam(index, name);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-RenamePCam\n");

  BOOL  fRetVal = TRUE;

  //TODO: Temporary solution till DD is defined
  struct_Pack_RenamePCam Pack;
  ZeroMemory(&Pack,sizeof(struct_Pack_RenamePCam));
  Pack.index= index;
  memcpy(Pack.name,name,sizeof(name));

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_RENAME_PCAM,
                              static_cast<LPVOID>(&Pack),static_cast<DWORD>(sizeof(struct_Pack_RenamePCam)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_AdjustPCamElement(short index, struct_PCamDetails* element) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_AdjustPCamElement(index, element);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-AdjustPCamElement\n");

  BOOL  fRetVal = TRUE;

  //TODO: Temporary solution till DD is defined
  struct_Pack_AdjustPCamElement Pack;
  ZeroMemory(&Pack,sizeof(struct_Pack_AdjustPCamElement));
  Pack.index= index;
  Pack.details= element;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_ADJUST_PCAM_ELEMENT,
                              static_cast<LPVOID>(&Pack),static_cast<DWORD>(sizeof(struct_Pack_AdjustPCamElement)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

BOOL HMINavigationAccessLib::command_RefreshWeather(enum_WeatherRefreshType refreshType) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_RefreshWeather(refreshType);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-RefreshWeather\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_REFRESH_WEATHER,
                                static_cast<LPVOID>(&refreshType), static_cast<DWORD>(sizeof(refreshType)),
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetPushObjectDynaMode(enum_DynaMode * dynaMode) const/*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetPushObjectDynaMode(dynaMode);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-command_GetPushObjectDynaMode\n");

    BOOL  fRetVal = TRUE;

    if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(dynaMode))==TRUE)
    {
        fRetVal = DeviceIoControl(
                                    ghINav, IOCTL_NAV_GET_PUSH_OBJECT_DYNA_MODE,
                                    NIL, NIL,
                                    static_cast<LPVOID>(dynaMode), static_cast<DWORD>(sizeof(enum_DynaMode)),
                                    NIL, NIL
                                    );
    }

    else
    {
        fRetVal = FALSE;
    }

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetPushObjectDynaMode(enum_DynaMode dynaMode) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetPushObjectDynaMode(dynaMode);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetPushObjectDynaMode\n");

  BOOL  fRetVal = TRUE;

  fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_SET_PUSH_OBJECT_DYNA_MODE,
                              static_cast<LPVOID>(&dynaMode), static_cast<DWORD>(sizeof(enum_DynaMode)),
                              NIL, NIL,
                              NIL, NIL
                              );

  return fRetVal;
}

//SWDD_09b change
BOOL HMINavigationAccessLib::command_GetPushObjectShortName(struct_PushObject * po) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetPushObjectShortName(po);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-command_GetPushObjectShortName\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_PUSH_OBJECT_SHORT_NAME,
                                NIL, NIL,
                                static_cast<LPVOID>(po), static_cast<DWORD>(sizeof(struct_PushObject)),
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetPushObjectDetail() const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetPushObjectDetail();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-command_GetPushObjectDetail\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_PUSH_OBJECT_DETAIL,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetPOIPushObjectDetailData(struct_POIPushObjectData * element) const/*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetPOIPushObjectDetailData(element);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-command_GetPOIPushObjectDetailData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(element))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_POI_PUSH_OBJECT_DETAIL_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(element), static_cast<DWORD>(sizeof(struct_POIPushObjectData)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

 BOOL HMINavigationAccessLib::command_GetGuidancePushObjectDetailData(struct_GuidancePushObjectData * element) const/*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetGuidancePushObjectDetailData(element);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
  LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-command_GetGuidancePushObjectDetailData\n");

  BOOL  fRetVal = TRUE;

  if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(element))==TRUE)
  {
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_GET_GUIDANCE_PUSH_OBJECT_DETAIL_DATA,
                                NIL, NIL,
                                static_cast<LPVOID>(element), static_cast<DWORD>(sizeof(struct_GuidancePushObjectData)),
                                NIL, NIL
                                );
   }

  else
  {
    fRetVal = FALSE;
  }
  return fRetVal;
}

BOOL HMINavigationAccessLib::command_DeletePushObject() const/*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_DeletePushObject();
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-command_DeletePushObject\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_DELETE_PUSH_OBJECT,
                                NIL, NIL,
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetTrafficBargraph(short index,short noOfElements) const/*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetTrafficBargraph(index, noOfElements);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetTrafficBargraph\n");

    BOOL  fRetVal = TRUE;

    //TODO: Temporary solution till DD is defined
    struct_Pack packTrafficBargraph;
    packTrafficBargraph.index= index;
    packTrafficBargraph.noOfItems= noOfElements;

    fRetVal = DeviceIoControl(
                              ghINav, IOCTL_NAV_GET_TRAFFIC_BARGRAPH,
                              static_cast<LPVOID>(&packTrafficBargraph),static_cast<DWORD>(sizeof(packTrafficBargraph)),
                              NIL, NIL,
                              NIL, NIL
                              );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetTrafficBargraphData(struct_TrafficFlowList * flowList) const/*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetTrafficBargraphData(flowList);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetTrafficBargraphData\n");

    BOOL  fRetVal = TRUE;

    if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(flowList))==TRUE)
    {
        fRetVal = DeviceIoControl(
                                    ghINav, IOCTL_NAV_GET_TRAFFIC_BARGRAPH_DATA,
                                    NIL, NIL,
                                    static_cast<LPVOID>(flowList), static_cast<DWORD>(sizeof(struct_TrafficFlowList)),
                                    NIL, NIL
                                    );
    }

    else
    {
        fRetVal = FALSE;
    }

    return fRetVal;
}

//SWDD_09b add
BOOL HMINavigationAccessLib::command_SelectDetourRoute(struct_RouteSelection * routeSelection) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SelectDetourRoute(routeSelection);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SelectDetourRoute\n");

    BOOL  fRetVal = TRUE;
    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_SELECT_DETOUR_ROUTE,
                                static_cast<LPVOID>(routeSelection), static_cast<DWORD>(sizeof(struct_RouteSelection)),
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetDetourRouteInfoData(struct_RouteInfo routeInfo[MAX_DETOUR_ROUTE_INFO_SIZE]) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetDetourRouteInfoData(routeInfo);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetDetourRouteInfoData\n");

    BOOL  fRetVal = TRUE;

    if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(routeInfo))==TRUE)
    {
        fRetVal = DeviceIoControl(
                                    ghINav, IOCTL_NAV_GET_DETOUR_ROUTE_INFO_DATA,
                                    NIL, NIL,
                                    static_cast<LPVOID>(routeInfo), static_cast<DWORD>(sizeof(struct_RouteInfo) * MAX_DETOUR_ROUTE_INFO_SIZE),
                                    NIL, NIL
                                    );
    }

    else
    {
        fRetVal = FALSE;
    }

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_SelectRouteOnMap(enum_RouteType routeType) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SelectRouteOnMap(routeType);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SelectRouteOnMap\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                ghINav, IOCTL_NAV_SELECT_ROUTE_ON_MAP,
                                static_cast<LPVOID>(&routeType), static_cast<DWORD>(sizeof(enum_RouteType)),
                                NIL, NIL,
                                NIL, NIL
                                );

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetDetourRoadName(TCHAR roadName[MAX_SHORT_LOCATION_NAME_STRING_LENGTH]) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetDetourRoadName(roadName);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetDetourRoadName\n");

    BOOL  fRetVal = TRUE;

    if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(roadName))==TRUE)
    {
        fRetVal = DeviceIoControl(
                                    ghINav, IOCTL_NAV_GET_DETOUR_ROAD_NAME,
                                    NIL, NIL,
                                    static_cast<LPVOID>(roadName), static_cast<DWORD>(sizeof(TCHAR) * MAX_SHORT_LOCATION_NAME_STRING_LENGTH),
                                    NIL, NIL
                                    );
    }

    else
    {
        fRetVal = FALSE;
    }

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetConnNavEnableSetting(struct_ConnNavState * connNavState) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetConnNavEnableSetting(connNavState);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetConnNavEnableSetting\n");

    BOOL  fRetVal = TRUE;

    if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(connNavState))==TRUE)
    {
        fRetVal = DeviceIoControl(
                                    ghINav, IOCTL_NAV_GET_CONN_NAV_ENABLE_SETTING,
                                    NIL, NIL,
                                    static_cast<LPVOID>(connNavState), static_cast<DWORD>(sizeof(struct_ConnNavState)),
                                    NIL, NIL
                                    );
    }

    else
    {
        fRetVal = FALSE;
    }

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_GetJourneyInfoData(struct_JourneyInfo * pstrJourneyInfo) const/*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_GetJourneyInfoData(pstrJourneyInfo);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-GetJourneyInfoData\n");

    BOOL  fRetVal = TRUE;

    if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(pstrJourneyInfo))==TRUE)
    {
        fRetVal = DeviceIoControl(
                                    ghINav, IOCC_NAV_GET_JOURNEYINFO_DATA,
                                    NIL, NIL,
                                    static_cast<LPVOID>(pstrJourneyInfo), static_cast<DWORD>(sizeof(struct_JourneyInfo)),
                                    NIL, NIL
                                    );
    }

    else
    {
        fRetVal = FALSE;
    }

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_SetDestinationByGeo(struct_GeoArray * pstGeoBuffer) const/*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_SetDestinationByGeo(pstGeoBuffer);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-SetDestinationByGeo\n");

    BOOL  fRetVal = TRUE;

    if(entNavCheckNullPointer(reinterpret_cast<PBYTE>(pstGeoBuffer))==TRUE)
    {
        fRetVal = DeviceIoControl(
                                    ghINav, IOCC_NAV_SET_DESTINATION_BY_GEO,
                                    static_cast<LPVOID>(pstGeoBuffer), static_cast<DWORD>(sizeof(struct_GeoArray)),
                                    NIL, NIL,
                                    NIL, NIL
                                    );
    }

    else
    {
        fRetVal = FALSE;
    }

    return fRetVal;
}

BOOL HMINavigationAccessLib::command_ClearListSelection(enum_ListType listType) const/*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */
{
#ifdef USER_TEST
    uint32_t retUser = HMINavigationAccessLibUser::getInstance().command_ClearListSelection(listType);
    if (retUser != RET_NOT_PROCESSED) {
        return retUser;
    }
    
#endif // #ifdef USER_TEST
    LOG_PRINT(DPCA_NAV_MAIN_INFO,"NAV:AL-ClearListSelection\n");

    BOOL  fRetVal = TRUE;

    fRetVal = DeviceIoControl(
                                    ghINav, IOCC_NAV_CLEAR_LIST_SELECTION,
                                    static_cast<LPVOID>(&listType), static_cast<DWORD>(sizeof(enum_ListType)),
                                    NIL, NIL,
                                    NIL, NIL
                                    );

    return fRetVal;
}
#endif
