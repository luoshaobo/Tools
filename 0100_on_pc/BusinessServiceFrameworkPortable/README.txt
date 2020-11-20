##########################################################################################
## How to build and run in Win 7 (64 bit) with Visual Studio Express 2107
##
1.  The steps as below:
    1)  Open main.sln in Visual Studio Express 2107.
    2)  Build.
    3)  Run.
    4)  Check the log file:
            main\log.txt
    
##########################################################################################
## How to build and run in Unbutu 16 (64 bit) with gcc
##
1.  The steps as below:
    1)  Excute:
            make ServiceFramework
        to build and install ServiceFramework.
        NOTE: 
        A) The root permision (sudo) is necessary to install ServiceFramework.
    2)  Excute:
            make
        to build and generate the binary:
            sample_app
    3)  Excute:
            ./sample_app
        to run the test application.
    4)  Check the log file:
            ./log.txt   

##########################################################################################
## Folders and files
##
1.  Listed as below:
    .
    |-- BussinessServiceFramework/                                                        # All of the files of BussinessServiceFramework are in this folder.
    |   |-- bsfwk_BusinessJobStateMachine.cpp
    |   |-- bsfwk_BusinessJobStateMachine.h
    |   |-- bsfwk_BusinessServer.cpp
    |   |-- bsfwk_BusinessServer.h
    |   |-- bsfwk_BusinessService.cpp
    |   |-- bsfwk_BusinessService.h
    |   |-- bsfwk_BusinessServiceStateMachine.cpp
    |   |-- bsfwk_BusinessServiceStateMachine.h
    |   |-- bsfwk_Common.cpp
    |   |-- bsfwk_Common.h
    |   |-- bsfwk_EntityFactoryBase.cpp
    |   |-- bsfwk_EntityFactoryBase.h
    |   |-- bsfwk_Global.cpp
    |   |-- bsfwk_Global.h
    |   |-- bsfwk_IEntityFactory.h
    |   |-- bsfwk_IJobEntity.h
    |   |-- bsfwk_IJobStateMachine.h
    |   |-- bsfwk_IServiceEntity.h
    |   |-- bsfwk_IServiceStateMachine.h
    |   |-- bsfwk_JobEntityBase.cpp
    |   |-- bsfwk_JobEntityBase.h
    |   |-- bsfwk_ServiceEntityBase.cpp
    |   |-- bsfwk_ServiceEntityBase.h
    |   `-- bsfwk_Types.h
    |-- Makefile                                                                          # Makefile to build in PC linux.
    |-- README.txt                                                                        # This file.
    |-- clean_vs.sh                                                                       # Script to clean the generated files by Visual Studio Express 2017.
    |-- main/                                                                             
    |   |-- main.cpp                                                                      # main() is in this file.
    |   |-- main.vcxproj
    |   |-- main.vcxproj.filters
    |   `-- main.vcxproj.user
    |-- main.sln                                                                          # The solution file of Visual Studio Express 2017.
    |-- misc/                                                                             # Some miscellaneous files. To be removed in formal project.
    |   |-- misc.cpp
    |   `-- misc.h
    |-- port/                                                                             # The files ported from TCAM project. To be removed in formal project.
    |   |-- 03.19.16.01.txt
    |   |-- FoundationServiceManager/
    |   |-- ServiceFramework/
    |   |-- UserManager/
    |   |-- VehicleComm/
    |   |-- VolvoOnCall/
    |   |-- VolvoPositioningManager/
    |   `-- misc/
    |-- port.orig/                                                                        # The orignal files from TCAM project. To be removed in formal project.
    |   |-- 03.19.16.01.txt
    |   |-- FoundationServiceManager/
    |   |-- ServiceFramework/
    |   |-- UserManager/
    |   |-- VehicleComm/
    |   |-- VolvoOnCall/
    |   |-- VolvoPositioningManager/
    |   `-- misc/
    |-- rmc/                                                                              # The sample code of Remote Control. Able to run in PC Win/Linux and TCAM target. Just for demo.
    |   |-- rmc_common.cpp
    |   |-- rmc_common.h
    |   |-- rmc_door_tailgate.cpp
    |   |-- rmc_door_tailgate.h
    |   |-- rmc_flash_horn.cpp
    |   |-- rmc_flash_horn.h
    |   |-- rmc_global.cpp
    |   |-- rmc_global.h
    |   |-- rmc_pm25.cpp
    |   |-- rmc_pm25.h
    |   |-- rmc_window_roof.cpp
    |   `-- rmc_window_roof.h
    |-- simulator/                                                                        # Some API simulator. To be removed in formal project.
    |   |-- rmc_simulator.cpp
    |   |-- rmc_simulator.h
    |   |-- simulator_common.cpp
    |   |-- simulator_common.h
    |   |-- vc_simulator.cpp
    |   |-- vc_simulator.h
    |   |-- vpom_simulator.cpp
    |   `-- vpom_simulator.h
    `-- test/                                                                             # Some test code for BussinessServiceFramework. To be removed in formal project.
        |-- test_app_0001.cpp
        |-- test_app_0001.h
        |-- test_app_0002.cpp
        |-- test_app_0002.h
        |-- test_app_0003.cpp
        |-- test_app_0003.h
        |-- test_app_0004.cpp
        |-- test_app_0004.h
        |-- test_app_0005.cpp
        |-- test_app_0005.h
        |-- test_app_0006.cpp
        |-- test_app_0006.h
        |-- test_app_0101.cpp
        |-- test_app_0101.h
        |-- test_app_0102.cpp
        |-- test_app_0102.h
        |-- test_app_0103.cpp
        |-- test_app_0103.h
        |-- test_app_0104.cpp
        |-- test_app_0104.h
        |-- test_app_0201.cpp
        |-- test_app_0201.h
        |-- test_app_0202.cpp
        |-- test_app_0202.h
        |-- test_app_0203.cpp
        |-- test_app_0203.h
        |-- test_app_0204.cpp
        |-- test_app_0204.h
        |-- test_app_5001.cpp
        |-- test_app_5001.h
        |-- test_app_global.cpp
        `-- test_app_global.h       
    