★★★★★
文件说明
1.  列举如下：
        StateMachineStencil.vss     # 创建状态机vsd文件时，需要打开此文件，并用其面板中的部件进行绘图；
        SMCodeGenBase.bas           # 在创建的状态机vsd文件中，需要导入此模块，并执行其中的宏来生成xml和cpp文件；
        SMCodeGenBase.bat           # 生成cpp文件的基础bat脚本；
        StateMachine.vsd            # 一个用户绘制的vsd状态图；
        StateMachine.bat            # 对应上述vsd文件的生成cpp文件的bat脚本，需要调用SMCodeGenBase.bat和StateMachine_inc.bat；
        StateMachine_inc.bat        # 在vsd状态图中调用SMCodeGenBase.bas中的宏生成此bat包含文件；
        