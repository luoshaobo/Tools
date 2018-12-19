The source code should be put to:
    $ANDROID_SRC_ROOT/frameworks/av/cmds/
in Android 4.2.2 source tree.

Execute the command lines as below:
    cd $ANDROID_SRC_ROOT
    . build/envsetup.sh
    lunch full-eng
    cd $ANDROID_SRC_ROOT/frameworks/av/cmds/draw_to_layer_raw_buf
    mm
to build the binary.
Then push the generated binary to the target by the below command line:
    adb push out/target/product/generic/system/bin/draw_to_layer_raw_buf /system/bin
Then execute the binary on target:
    /system/bin/draw_to_layer_raw_buf
	