##########################################################################################
## ˵��
##
1.  �ļ��б�       
        dirs
        README.txt                                          # ���ļ���  
        XmlTestDriver                                       # ��***����������Ŀ¼��
        XmlTestDriver/Build.log
        XmlTestDriver/makefile
        XmlTestDriver/sock.cpp                              # ����socket���ܣ�
        XmlTestDriver/sock.h
        XmlTestDriver/sources
        XmlTestDriver/XmlTestDriver.bib
        XmlTestDriver/XmlTestDriver.cpp                     # ��XTD_Open()�м���socket UDP server��
        XmlTestDriver/XmlTestDriver.def
        XmlTestDriver/XmlTestDriver.reg                     # ���ļ���������Ҫ��ӵ�OSDesign.reg��platform.reg�У�
        XmlTestDriverTest                                   # ��****��Ӧ�ó���Ŀ¼�����ڲ�����������
        XmlTestDriverTest/Build.log
        XmlTestDriverTest/makefile
        XmlTestDriverTest/sources
        XmlTestDriverTest/stdafx.cpp
        XmlTestDriverTest/stdafx.h
        XmlTestDriverTest/stocks.xml                        # ���ļ���������Ҫ������releaseĿ¼�У�
        XmlTestDriverTest/XmlTestDriverTest.cpp
2.  ��װ�ͱ��룺
    1�� ����ѹ���ļ���ѹ��������Ŀ¼��
            C:\WINCE700\platform\CEPC\src\drivers
        �޸ĸ�Ŀ¼�µ��ļ���
            dir
        ��ο�����Ϊ��
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
    2�� ʹ��vs2008������Solution��
            C:\WINCE700\OSDesigns\VirtualPCCE\VirtualPCCE.sln
        ��Solution Explorer��չ�������¹��̣�
            C:\WINCE700\platform\CEPC\src\drivers\XmlTestDriver\XmlTestDriver
        �Ҽ�����ù��̣��ڵ����˵���ѡ�����²˵��
            Open Build Window
        �ڵ�����Build Window��ִ�У�
            build -c
        ������������ı��룻
        ��Solution Explorer��չ�������¹��̣�
            C:\WINCE700\platform\CEPC\src\drivers\XmlTestDriver\XmlTestDriverTest
        �ʼ�����ù��̣��ڵ����˵���ѡ�����²˵��
            Open Build Window
        �ڵ�����Build Window��ִ�У�
            build -c
        ����Ӧ�ó���ı��룻
    3�� ��Solution Explorer�еġ�VirtualCEPC�����Ҽ��������ڵ����˵���ѡ��
            Make Run-time Image
        �����µ�image��
            nk.bin
        ע�⣺
        A�� ������������ȫ����ʱ����ͬʱ�ֶ��޸�releaseĿ¼�еģ�
                OSDesign.bib
                OSDesign.data
                OSDesign.reg
                platform.bib
                platform.data
                platform.reg
            ���ļ�����Ϊ���ֱ���ʱ�������ļ������Զ������£�
3.  ���ص�Virtual PC 2008�У����е��ԣ�
    ע�⣺
    A�� ��Ҫ��KITL���ܣ�