#/bin/bash

(
    cd 0001_ThreadStateMachines/visio/tmp && rm -fr *
)

(
    cd 0001_ThreadStateMachines/StateMachineFramework && rm -fr Debug *.ncb *.suo *.user
)

(
    cd 0002_TestSendEvent/visio/tmp && rm -fr *
)

(
    cd 0002_TestSendEvent/StateMachineFramework && rm -fr Debug *.ncb *.suo *.user
)

(
    cd 0002_TestSendEvent/StateMachineFramework.CodeBlocksProj.cygwin32/StateMachineFramework && rm -fr bin obj *.layout *.depend
)

(
    cd 0002_TestSendEvent/StateMachineFramework.CodeBlocksProj.cygwin32_for_osx64/StateMachineFramework && rm -fr bin obj *.layout *.depend
)

