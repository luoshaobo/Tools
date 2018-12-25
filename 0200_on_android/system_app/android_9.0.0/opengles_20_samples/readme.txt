The source code should be put to:
    $ANDROID_SRC_ROOT/frameworks/base/cmds/
in Android 9.0.0_rc2 source tree.

Execute the command lines as below:
    cd $ANDROID_SRC_ROOT
    . build/envsetup.sh
    lunch full-eng
    cd $ANDROID_SRC_ROOT/frameworks/base/cmds/opengles_20_samples
    mm
to build the binary.
Then push the generated binary to the target by the below command line:
    adb push out/target/product/generic/system/bin/opengles_20_samples /data/
Then execute the binary on target:
    /data/opengles_20_samples
	