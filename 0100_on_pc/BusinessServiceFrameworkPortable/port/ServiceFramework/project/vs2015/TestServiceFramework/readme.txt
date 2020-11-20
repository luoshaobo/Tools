Description of the TestServiceFramework VisualStudio Solution
=============================================================

	The TestServiceFramework-Solution contains a project "ServiceFramework" which compiles the files under
	 |
	 - ServiceFramework
	          |
	          - src
	 to a shared library. This library contains threading, mutex, semaphore, messagequeues, timers,
	 statemachines, timestamps, shared pointer, atomic counters, string utils, socket support,
	 dynamic library support, logger, ringbuffer.

	 Additionally it contains a project "TestServiceFramework" which is contains test code for
	 the files contained in "ServiceFramework".


Dependencies
============

    The "TestServiceFramework" is depending on C++11 and the "ServiceFramework" shared library, Google Mock library,
    Tiny Unit Test.

Building with Visual C++
========================

   To build with VC++, you will of course have to first install VC++ which is
   part of Visual Studio. The project was created with Visual Studio 2015.

   Build with solution configuration "DEBUG"
   and solution platform "x86".

   NOTE: Other configurations are not set yet!


Execution
==========

     If the project is built, then a "TestServiceFramework.exe" is stored in "TestServiceFramework/Debug".
     After execution a testreport is printed on screen about failed and succeeded tests.


Mount directory
===============
In order to compile the project map ensure that you mapped the "package" folder to root like shown here:
Use e.g. this command:
subst x: \\10.214.55.2\exchange\home\uid05116\development\vcc-tcam-3.y-latest

\PACKAGE
+---vcc-tcam
    +---AssistanceCall
    ¦   +---build
    ¦   ¦   +---CMakeFiles
    ¦   ¦   ¦   +---2.8.12.2
    ¦   ¦   ¦   ¦   +---CompilerIdC
    ¦   ¦   ¦   ¦   +---CompilerIdCXX
    ¦   ¦   ¦   +---CMakeTmp
    ¦   ¦   +---src
    ¦   ¦   ¦   +---CMakeFiles
    ¦   ¦   ¦       +---assistancecaller.dir
    ¦   ¦   ¦           +---calls
    ¦   ¦   ¦           ¦   +---statemachines
    ¦   ¦   ¦           +---vehiclecomm_connector
    ¦   ¦   +---src-old
    ¦   ¦       +---CMakeFiles
    ¦   ¦           +---assicall-test.dir
    ¦   +---include
    ¦   ¦   +---calls
    ¦   ¦   ¦   +---statemachines
    ¦   ¦   +---vehiclecomm_connector
    ¦   +---include-old
    ¦   +---project
    ¦   ¦   +---vs2015
    ¦   ¦       +---AssistanceCall
    ¦   ¦           +---AssistanceCall
    ¦   +---src
    ¦   ¦   +---calls
    ¦   ¦   ¦   +---statemachines
    ¦   ¦   +---vehiclecomm_connector
    ¦   +---src-old
    +---BLEManager
    ¦   +---build
    ¦   ¦   +---CMakeFiles
    ¦   ¦       +---2.8.12.2
    ¦   ¦       ¦   +---CompilerIdC
    ¦   ¦       ¦   +---CompilerIdCXX
    ¦   ¦       +---ble_manager.dir
    ¦   ¦       +---CMakeTmp
    ¦   +---include
    +---CommunicationManager
    ¦   +---.git
    ¦   +---build
    ¦   ¦   +---CMakeFiles
    ¦   ¦   ¦   +---2.8.12.2
    ¦   ¦   ¦   ¦   +---CompilerIdC
    ¦   ¦   ¦   ¦   +---CompilerIdCXX
    ¦   ¦   ¦   +---CMakeTmp
    ¦   ¦   +---config
    ¦   ¦   ¦   +---CMakeFiles
    ¦   ¦   +---src
    ¦   ¦   ¦   +---CMakeFiles
    ¦   ¦   ¦   +---communicationmanager
    ¦   ¦   ¦   ¦   +---CMakeFiles
    ¦   ¦   ¦   ¦       +---coma-core.dir
    ¦   ¦   ¦   ¦           +---datacommunication
    ¦   ¦   ¦   ¦           ¦   +---mqtt
    ¦   ¦   ¦   ¦           +---instancemanager
    ¦   ¦   ¦   ¦           +---ipc
    ¦   ¦   ¦   ¦           +---main
    ¦   ¦   ¦   ¦           +---nadif
    ¦   ¦   ¦   ¦           +---psapcall
    ¦   ¦   ¦   ¦           +---statemachines
    ¦   ¦   ¦   +---libclientcommunicationmanager
    ¦   ¦   ¦   ¦   +---CMakeFiles
    ¦   ¦   ¦   ¦       +---comaclient.dir
    ¦   ¦   ¦   ¦           +---__
    ¦   ¦   ¦   ¦               +---communicationmanager
    ¦   ¦   ¦   +---libinterprocesscommunication
    ¦   ¦   ¦       +---CMakeFiles
    ¦   ¦   ¦           +---comaipc.dir
    ¦   ¦   ¦               +---protobuf
    ¦   ¦   +---test
    ¦   ¦       +---CMakeFiles
    ¦   ¦       +---misc
    ¦   ¦           +---CMakeFiles
    ¦   ¦               +---comaclient-test.dir
    ¦   +---config
    ¦   +---include
    ¦   ¦   +---communicationmanager
    ¦   ¦   ¦   +---datacommunication
    ¦   ¦   ¦   ¦   +---mqtt
    ¦   ¦   ¦   +---instancemanager
    ¦   ¦   ¦   +---ipc
    ¦   ¦   ¦   +---lifecycle
    ¦   ¦   ¦   +---nadif
    ¦   ¦   ¦   +---psapcall
    ¦   ¦   ¦   +---statemachines
    ¦   ¦   ¦   +---ipc
    ¦   ¦   ¦       +---protobuf
    ¦   ¦   +---libclientcommunicationmanager
    ¦   ¦   +---libinterprocesscommunication
    ¦   ¦       +---protobuf
    ¦   +---project
    ¦   ¦   +---vs2015
    ¦   ¦       +---build
    ¦   ¦       ¦   +---output
    ¦   ¦       +---CommunicationManager
    ¦   ¦       ¦   +---.vs
    ¦   ¦       ¦   ¦   +---CommunicationManager
    ¦   ¦       ¦   ¦       +---v14
    ¦   ¦       ¦   +---bugfix
    ¦   ¦       ¦   ¦   +---curl
    ¦   ¦       ¦   ¦   +---net
    ¦   ¦       ¦   +---CommunicationManager
    ¦   ¦       ¦   +---CommunicationManagerClient
    ¦   ¦       ¦   +---libClientCommunicationManager
    ¦   ¦       ¦   +---libCurl
    ¦   ¦       ¦   +---libFoundation
    ¦   ¦       ¦   +---libPaho-Mqtt3c
    ¦   ¦       ¦   +---libProtobuf
    ¦   ¦       ¦   +---packages
    ¦   ¦       ¦   ¦   +---VisualLeakDetector.2.5.0.0
    ¦   ¦       ¦   ¦       +---build
    ¦   ¦       ¦   ¦       ¦   +---native
    ¦   ¦       ¦   ¦       +---lib
    ¦   ¦       ¦   ¦           +---native
    ¦   ¦       ¦   ¦               +---address-model-32
    ¦   ¦       ¦   ¦               ¦   +---lib
    ¦   ¦       ¦   ¦               +---address-model-64
    ¦   ¦       ¦   ¦               ¦   +---lib
    ¦   ¦       ¦   ¦               +---include
    ¦   ¦       +---CommunicationManagerUnitTest
    ¦   ¦           +---CommunicationManagerUnitTest
    ¦   ¦               +---googletest
    ¦   ¦                   +---gmock
    ¦   ¦                   ¦   +---internal
    ¦   ¦                   ¦       +---custom
    ¦   ¦                   +---gtest
    ¦   ¦                       +---internal
    ¦   ¦                           +---custom
    ¦   +---src
    ¦   ¦   +---communicationmanager
    ¦   ¦   ¦   +---datacommunication
    ¦   ¦   ¦   ¦   +---mqtt
    ¦   ¦   ¦   +---instancemanager
    ¦   ¦   ¦   +---ipc
    ¦   ¦   ¦   +---main
    ¦   ¦   ¦   +---nadif
    ¦   ¦   ¦   +---psapcall
    ¦   ¦   ¦   +---statemachines
    ¦   ¦   +---foundation__
    ¦   ¦   ¦   +---ipc
    ¦   ¦   ¦       +---protobuf
    ¦   ¦   ¦           +---proto
    ¦   ¦   +---libclientcommunicationmanager
    ¦   ¦   +---libinterprocesscommunication
    ¦   ¦       +---protobuf
    ¦   ¦           +---proto
    ¦   +---test
    ¦       +---misc
    ¦       +---src
    ¦       ¦   +---communicationmanager
    ¦       ¦   ¦   +---datacommunication
    ¦       ¦   ¦   +---statemachines
    ¦       ¦   +---foundation
    ¦       ¦   +---tut
    ¦       +---tcam_keys
    ¦           +---test1
    +---DoIP_Edge
    ¦   +---include
    ¦   +---src
    ¦   +---test
    ¦       +---python
    +---FoundationServiceManager
    ¦   +---doc
    ¦   +---fsm_ccm
    ¦   ¦   +---include
    ¦   ¦   +---src
    ¦   ¦       +---generated
    ¦   ¦       +---schemas
    ¦   ¦           +---asn
    ¦   ¦               +---common
    ¦   ¦               +---message
    ¦   ¦               +---sms
    ¦   +---fsm_persist_data_mgr
    ¦   ¦   +---include
    ¦   ¦   +---src
    ¦   +---service_discovery
    ¦   ¦   +---daemon
    ¦   ¦   ¦   +---generated
    ¦   ¦   +---include
    ¦   ¦   ¦   +---fssd
    ¦   ¦   +---interface
    ¦   ¦   +---lib
    ¦   ¦   ¦   +---generated
    ¦   ¦   +---mock_cloud
    ¦   ¦   ¦   +---TCAM_cloud_mock
    ¦   ¦   ¦       +---mock_content
    ¦   ¦   ¦           +---BasicCarControl
    ¦   ¦   ¦           +---CarAccess
    ¦   ¦   +---test
    ¦   +---signal_service_manager
    ¦   ¦   +---include
    ¦   ¦   +---src
    ¦   +---test
    ¦   ¦   +---ccm_test
    ¦   ¦   +---unittest
    ¦   ¦       +---ccm
    ¦   ¦       ¦   +---dlt
    ¦   ¦       ¦   +---pems
    ¦   ¦       +---transfermanager
    ¦   ¦       ¦   +---debs
    ¦   ¦       ¦   +---soapui
    ¦   ¦       +---user_manager
    ¦   ¦           +---dlt
    ¦   ¦           +---stubs
    ¦   ¦               +---persistence
    ¦   +---transfermanager
    ¦   ¦   +---include
    ¦   ¦   +---src
    ¦   ¦       +---build_libfsm_transfermanager
    ¦   ¦           +---CMakeFiles
    ¦   ¦               +---2.8.12.2
    ¦   ¦               ¦   +---CompilerIdCXX
    ¦   ¦               +---CMakeTmp
    ¦   ¦               +---libfsm_transfermanager.dir
    ¦   +---user_manager
    ¦   ¦   +---config
    ¦   ¦   +---include
    ¦   ¦   +---src
    ¦   ¦   +---usermanager_tool
    ¦   +---voc_framework
    ¦       +---include
    ¦       ¦   +---voc_framework
    ¦       ¦       +---features
    ¦       ¦       +---signals
    ¦       ¦       +---signal_sources
    ¦       ¦       +---transactions
    ¦       +---src
    ¦           +---features
    ¦           +---signals
    ¦           ¦   +---ccm_parsing
    ¦           ¦       +---generated
    ¦           ¦       +---schemas
    ¦           ¦           +---asn
    ¦           ¦               +---carLocator
    ¦           ¦               +---catalogue
    ¦           ¦               +---common
    ¦           ¦               +---delegate
    ¦           ¦               +---device_pairing
    ¦           ¦               +---discovery
    ¦           ¦               +---en-standards
    ¦           ¦               +---functions
    ¦           ¦               +---locking
    ¦           ¦               +---rfc
    ¦           ¦               +---routing
    ¦           ¦               +---ticket
    ¦           +---signal_sources
    ¦           +---transactions
    +---IPCommandBroker
    ¦   +---build
    ¦   ¦   +---CMakeFiles
    ¦   ¦   ¦   +---2.8.12.2
    ¦   ¦   ¦   ¦   +---CompilerIdC
    ¦   ¦   ¦   ¦   +---CompilerIdCXX
    ¦   ¦   ¦   +---CMakeTmp
    ¦   ¦   +---gdbus
    ¦   ¦   ¦   +---client_proxy
    ¦   ¦   ¦   ¦   +---CMakeFiles
    ¦   ¦   ¦   ¦       +---ipcb_gdbus_client_proxy.dir
    ¦   ¦   ¦   ¦           +---__
    ¦   ¦   ¦   +---client_test
    ¦   ¦   ¦   ¦   +---CMakeFiles
    ¦   ¦   ¦   ¦       +---ipcb_gdbus_client.dir
    ¦   ¦   ¦   +---CMakeFiles
    ¦   ¦   ¦       +---ipcb_gdbus.dir
    ¦   ¦   +---libasn1
    ¦   ¦   ¦   +---CMakeFiles
    ¦   ¦   ¦       +---asn1.dir
    ¦   ¦   ¦           +---asn_base
    ¦   ¦   ¦           +---generated
    ¦   ¦   ¦           +---src
    ¦   ¦   +---libipcommandbus
    ¦   ¦   ¦   +---CMakeFiles
    ¦   ¦   ¦       +---ipcommandbus.dir
    ¦   ¦   ¦           +---src
    ¦   ¦   +---tcam_ipcb_service
    ¦   ¦       +---CMakeFiles
    ¦   ¦           +---IPCommandBroker.dir
    ¦   ¦               +---src
    ¦   ¦                   +---services
    ¦   ¦                   +---util
    ¦   +---cmake
    ¦   +---gdbus
    ¦   ¦   +---client_proxy
    ¦   ¦   +---client_test
    ¦   +---libasn1
    ¦   ¦   +---asn_base
    ¦   ¦   +---examples
    ¦   ¦   +---generated
    ¦   ¦   +---include
    ¦   ¦   ¦   +---pl
    ¦   ¦   ¦   ¦   +---asn_base
    ¦   ¦   ¦   +---tem3_interface
    ¦   ¦   +---spec
    ¦   ¦   +---src
    ¦   ¦   +---test
    ¦   ¦       +---ut
    ¦   +---libipcommandbus
    ¦   ¦   +---include
    ¦   ¦   ¦   +---ipcommandbus
    ¦   ¦   +---src
    ¦   +---tcam_ipcb_service
    ¦       +---src
    ¦           +---services
    ¦           +---util
    +---ServiceFramework
    ¦   +---.git
    ¦   +---include
    ¦   +---project
    ¦   ¦   +---vs2015
    ¦   ¦       +---ServiceFramework
    ¦   ¦       ¦   +---.vs
    ¦   ¦       ¦   ¦   +---ServiceFramework
    ¦   ¦       ¦   ¦       +---v14
    ¦   ¦       ¦   +---Debug
    ¦   ¦       ¦   +---ServiceFramework
    ¦   ¦       ¦       +---Debug
    ¦   ¦       ¦       ¦   +---ServiceFramework.tlog
    ¦   ¦       ¦       +---include
    ¦   ¦       +---TestServiceFramework
    ¦   ¦           +---.vs
    ¦   ¦           ¦   +---TestServiceFramework
    ¦   ¦           ¦       +---v14
    ¦   ¦           +---Debug
    ¦   ¦           +---googletest
    ¦   ¦           ¦   +---gmock
    ¦   ¦           ¦   ¦   +---internal
    ¦   ¦           ¦   ¦   ¦   +---custom
    ¦   ¦           ¦   ¦   +---lib
    ¦   ¦           ¦   ¦       +---dll
    ¦   ¦           ¦   ¦       +---static
    ¦   ¦           ¦   +---gtest
    ¦   ¦           ¦       +---internal
    ¦   ¦           ¦       ¦   +---custom
    ¦   ¦           ¦       +---lib
    ¦   ¦           ¦           +---dll
    ¦   ¦           ¦           +---static
    ¦   ¦           +---TestServiceFramework
    ¦   ¦               +---Debug
    ¦   ¦               ¦   +---TestServ.9F5CB5F4.tlog
    ¦   ¦               +---tut
    ¦   +---src
    ¦   +---test
    ¦       +---src
    +---tcam-diag
    ¦   +---config
    ¦   +---diag
    ¦   ¦   +---dtc_lib
    ¦   ¦   ¦   +---config
    ¦   ¦   ¦   +---include
    ¦   ¦   ¦   +---interface
    ¦   ¦   ¦   +---src
    ¦   ¦   ¦   +---test
    ¦   ¦   +---include
    ¦   ¦   +---src
    ¦   +---sample
    ¦   +---swl
    ¦       +---dc
    ¦       ¦   +---tcam-dc-ethernet
    ¦       ¦       +---include
    ¦       ¦       +---src
    ¦       +---tcam-ua-lc
    ¦       ¦   +---daemon
    ¦       ¦   ¦   +---include
    ¦       ¦   ¦   +---src
    ¦       ¦   +---test
    ¦       +---tcam-ua-notifier
    ¦       ¦   +---config
    ¦       ¦   +---include
    ¦       ¦   +---interface
    ¦       ¦   +---src
    ¦       +---tcam-ua-vuc
    ¦       ¦   +---daemon
    ¦       ¦   ¦   +---include
    ¦       ¦   ¦   +---src
    ¦       ¦   ¦   +---util
    ¦       ¦   ¦       +---include
    ¦       ¦   +---test
    ¦       ¦       +---script
    ¦       +---test-util
    ¦           +---tcam-test-util
    ¦           ¦   +---include
    ¦           ¦   +---src
    ¦           +---tty0tty
    +---VehicleComm
    ¦   +---build
    ¦   ¦   +---CMakeFiles
    ¦   ¦   ¦   +---2.8.12.2
    ¦   ¦   ¦   ¦   +---CompilerIdC
    ¦   ¦   ¦   ¦   +---CompilerIdCXX
    ¦   ¦   ¦   +---CMakeTmp
    ¦   ¦   ¦   +---vehicle_comm.dir
    ¦   ¦   +---lan_comm
    ¦   ¦   ¦   +---CMakeFiles
    ¦   ¦   ¦       +---lan_comm.dir
    ¦   ¦   +---ts_comm
    ¦   ¦   ¦   +---CMakeFiles
    ¦   ¦   ¦       +---ts_comm.dir
    ¦   ¦   +---ts_server
    ¦   ¦   ¦   +---CMakeFiles
    ¦   ¦   ¦       +---ts_server.dir
    ¦   ¦   ¦           +---__
    ¦   ¦   +---vehicle_comm_client
    ¦   ¦   ¦   +---CMakeFiles
    ¦   ¦   ¦       +---vehicle_comm_client.dir
    ¦   ¦   ¦           +---__
    ¦   ¦   ¦               +---ts_comm
    ¦   ¦   +---vuc_comm
    ¦   ¦       +---CMakeFiles
    ¦   ¦           +---vuc_comm.dir
    ¦   +---docs
    ¦   +---include
    ¦   +---interface
    ¦   +---lan_comm
    ¦   +---lan_server
    ¦   +---ts_comm
    ¦   ¦   +---cfg
    ¦   +---ts_server
    ¦   ¦   +---tests
    ¦   +---vehicle_comm_client
    ¦   +---vuc_comm
    +---VolvoConnectionManager
    ¦   +---gdbus
    ¦   ¦   +---client_proxy
    ¦   ¦   +---client_test
    ¦   +---include
    ¦   +---src
    ¦   +---vcnm_persist_data_mgr
    ¦       +---include
    ¦       +---src
    +---VolvoOnCall
        +---include
        ¦   +---features
        ¦   +---signals
        ¦   +---transactions
        +---src
        ¦   +---features
        ¦   +---signals
        ¦   ¦   +---ccm_parsing
        ¦   ¦       +---generated
        ¦   ¦       +---schemas
        ¦   ¦           +---asn
        ¦   ¦               +---carLocator
        ¦   ¦               +---catalogue
        ¦   ¦               +---common
        ¦   ¦               +---delegate
        ¦   ¦               +---device_pairing
        ¦   ¦               +---discovery
        ¦   ¦               +---en-standards
        ¦   ¦               +---functions
        ¦   ¦               +---locking
        ¦   ¦               +---rfc
        ¦   ¦               +---routing
        ¦   ¦               +---ticket
        ¦   +---transactions
        +---test
            +---certs
            ¦   +---ca
            ¦   ¦   +---certs
            ¦   ¦   +---csr
            ¦   ¦   +---newcerts
            ¦   ¦   +---private
            ¦   +---clients
            ¦   ¦   +---1
            ¦   +---tcam
            ¦       +---1
            +---messages
            +---unittest
                +---src
                    +---dlt