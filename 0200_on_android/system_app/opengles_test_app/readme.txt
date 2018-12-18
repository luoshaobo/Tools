The source code should be put to:
    $ANDROID_SRC_ROOT/frameworks/base/cmds/
in Android 4.2.2 source tree.

Execute the command lines as below:
    cd $ANDROID_SRC_ROOT
    . build/envsetup.sh
    lunch full-eng
    cd $ANDROID_SRC_ROOT/frameworks/base/cmds/opengles_test_app
    mm
to build the binary.
Then push the generated binary to the target by the below command line:
    adb push out/target/product/generic/system/bin/opengles_test_app /system/bin
Then execute the binary on target:
    /system/bin/opengles_test_app
	