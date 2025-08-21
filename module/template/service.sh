resetprop -w sys.boot_completed 0
setprop ctl.restart logd

MODDIR=${0%/*}
INJECTOR_PATH="${MODDIR}/bin/Injector"
LIB_PATH="/system/lib64/libauditpatch.so"
TARGET_PROC="/system/bin/logd"

chmod +x $INJECTOR_PATH
$INJECTOR_PATH -p $TARGET_PROC -l $LIB_PATH
