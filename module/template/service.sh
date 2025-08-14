# FIXME: logd starts very early in boot process, and ZN can only handle services which is started after post-fs-data for now
resetprop -w sys.boot_completed 0
setprop ctl.restart logd

# Path to your injector binary (compiled for your device's arch)
INJECTOR_PATH="/data/adb/modules/auditpatch/bin/Injector"
chmod +x $INJECTOR_PATH

# Path to your .so
LIB_PATH="/data/local/tmp/libauditpatch.so"
cp "/data/adb/modules/auditpatch/lib/libauditpatch.so" $LIB_PATH


# Name of the target process to inject into (must be running)
TARGET_PROC="logd"

# Wait until target process starts
while [ -z "$(pidof $TARGET_PROC)" ]; do
    sleep 1
done

# Get PID
PID=$(pidof $TARGET_PROC)

# Run injector (requires ptrace permissions, Magisk provides this in post-boot)
$INJECTOR_PATH -p $PID -l $LIB_PATH
