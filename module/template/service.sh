# FIXME: logd starts very early in boot process, and ZN can only handle services which is started after post-fs-data for now
resetprop -w sys.boot_completed 0
setprop ctl.restart logd

INJECTOR_PATH="/data/adb/modules/auditpatch/bin/Injector"
LIB_PATH="/system/lib64/libauditpatch.so"
TARGET_PROC="logd"

# Wait until target process starts
while [ -z "$(pidof $TARGET_PROC)" ]; do
    sleep 1
done

# Get PID
PID=$(pidof $TARGET_PROC)

chmod +x $INJECTOR_PATH
$INJECTOR_PATH -p $PID -l $LIB_PATH
