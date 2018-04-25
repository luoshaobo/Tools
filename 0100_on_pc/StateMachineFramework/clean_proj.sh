#/bin/bash

(
    cd 00_tests && bash clean_proj.sh
)

(
    cd IPCSender && rm -fr Debug *.ncb *.suo *.user
)

(
    cd SMCodeGen && rm -fr Debug *.ncb *.suo *.user
)

(
    cd visio/tmp && rm -fr *
)

(
    cd StateMachineFramework && rm -fr Debug *.ncb *.suo *.user
)

(
    cd StateMachineFramework.CodeBlocksProj.cygwin32/IPCSender && rm -fr bin obj *.layout *.depend
)

(
    cd StateMachineFramework.CodeBlocksProj.cygwin32/StateMachineFramework && rm -fr bin obj *.layout *.depend
)

(
    cd StateMachineFramework.CodeBlocksProj.linux32/IPCSender && rm -fr bin obj *.layout *.depend
)

(
    cd StateMachineFramework.CodeBlocksProj.linux32/StateMachineFramework && rm -fr bin obj *.layout *.depend
)

(
    cd StateMachineFramework.CodeBlocksProj.cygwin32_for_osx64/IPCSender && rm -fr bin obj *.layout *.depend
)

(
    cd StateMachineFramework.CodeBlocksProj.cygwin32_for_osx64/StateMachineFramework && rm -fr bin obj *.layout *.depend
)
