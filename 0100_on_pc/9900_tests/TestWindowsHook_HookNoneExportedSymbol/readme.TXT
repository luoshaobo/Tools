■  支持如下平台：
        Win32 X86                  # 在64位Win7上
        WinCE ARM32                # 在WinCE 7上
        
■  开发环境：
        Visual Studio 2008
        
■  请用debug版本；
    release版本生成的符号可能不可识别；
    
■  配置：
    □  TestLoadExternalSymbal_debugee：
        △  生成map文件；
        △  程序加载到固定位置；
        △  保留未被引用的符号；
    □  TestLoadExternalSymbal_DllToInject：
        △  生成map文件；DLL导出符号也生成到map文件中；
        △  生成的DLL中地址仅依赖于DLL加载的基址；
        △  生成DLL导入节；
        △  保留未被引用的符号；