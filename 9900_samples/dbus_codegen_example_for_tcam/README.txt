#
# How to build and run on PC Linux (Unbuntu 16 64bit)
#
1.  Exectue:
        cd dbus_codegen_example_tcam
        make
    to build.
2.  Run:
        ./gserver
        ./gclient
        
#
# How to build on PC Linux (Unbuntu 16 64bit) and run on TCAM target
#
1.  Download a whole copy of code of gly-tcam, and build.
2.  Exectue on PC Linux:
        cd dbus_codegen_example_tcam
        make BUILD_TARGET=1 SYS_BUILD_ROOT=/home/uidv4956/tcam_root/tcam_src_root_03.19.16.01
    to build, in which the value of:
        SYS_BUILD_ROOT
    should be changed to your gly-tcam souce code root directory.
3.  Copy:
        net.corp.MyApp.conf
    to TCAM target:
        /etc/dbus-1/system.d/
4.  Run on TCAM:
        ./gserver
        ./gclient
        