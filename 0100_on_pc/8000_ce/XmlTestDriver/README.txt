##########################################################################################
## 说明
##
1.  文件列表：       
        dirs
        README.txt                                          # 本文件；  
        XmlTestDriver                                       # （***）驱动程序目录；
        XmlTestDriver/Build.log
        XmlTestDriver/makefile
        XmlTestDriver/sock.cpp                              # 新增socket功能；
        XmlTestDriver/sock.h
        XmlTestDriver/sources
        XmlTestDriver/XmlTestDriver.bib
        XmlTestDriver/XmlTestDriver.cpp                     # 在XTD_Open()中激活socket UDP server；
        XmlTestDriver/XmlTestDriver.def
        XmlTestDriver/XmlTestDriver.reg                     # 该文件中内容需要添加到OSDesign.reg或platform.reg中；
        XmlTestDriverTest                                   # （****）应用程序目录；用于测试驱动程序；
        XmlTestDriverTest/Build.log
        XmlTestDriverTest/makefile
        XmlTestDriverTest/sources
        XmlTestDriverTest/stdafx.cpp
        XmlTestDriverTest/stdafx.h
        XmlTestDriverTest/stocks.xml                        # 该文件中内容需要拷贝到release目录中；
        XmlTestDriverTest/XmlTestDriverTest.cpp
2.  安装和编译：
    1） 将本压缩文件解压缩到如下目录：
            C:\WINCE700\platform\CEPC\src\drivers
        修改该目录下的文件：
            dir
        其参考内容为：
            DIRS=\
            # @CESYSGEN IF CE_MODULES_DEVICE
            # @CESYSGEN IF CE_MODULES_WAVEAPI
                sis7019\
            # @CESYSGEN ENDIF CE_MODULES_WAVEAPI
                VirtualPC{ifexist}\
            # @CESYSGEN IF CE_MODULES_UPDATEAPP
            	uldrui{ifexist}\
            # @CESYSGEN ENDIF CE_MODULES_UPDATEAPP
            # @CESYSGEN ENDIF CE_MODULES_DEVICE
                XmlTestDriver \
    2） 使用vs2008打开如下Solution：
            C:\WINCE700\OSDesigns\VirtualPCCE\VirtualPCCE.sln
        在Solution Explorer中展开到如下工程：
            C:\WINCE700\platform\CEPC\src\drivers\XmlTestDriver\XmlTestDriver
        右键打击该工程，在弹出菜单中选择如下菜单项：
            Open Build Window
        在弹出的Build Window中执行：
            build -c
        进行驱动程序的编译；
        在Solution Explorer中展开到如下工程：
            C:\WINCE700\platform\CEPC\src\drivers\XmlTestDriver\XmlTestDriverTest
        邮件打击该工程，在弹出菜单中选择如下菜单项：
            Open Build Window
        在弹出的Build Window中执行：
            build -c
        进行应用程序的编译；
    3） 在Solution Explorer中的“VirtualCEPC”上右键单击，在弹出菜单上选择：
            Make Run-time Image
        生成新的image：
            nk.bin
        注意：
        A） 采用上述不完全编译时，请同时手动修改release目录中的：
                OSDesign.bib
                OSDesign.data
                OSDesign.reg
                platform.bib
                platform.data
                platform.reg
            等文件，因为部分编译时，上述文件不会自动被更新；
3.  下载到Virtual PC 2008中，进行调试；
    注意：
    A） 需要打开KITL功能；