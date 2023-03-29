#ifndef IS_VENDOR_CONFIG_AAW_V14X_H
#define IS_VENDOR_CONFIG_AAW_V14X_H

#define VENDER_PATH

/***** HW DEFENDANT DEFINE *****/
#define DISABLE_DUAL_SYNC

#define USE_HI1336B_SETFILE                                   /* 13MP setfile */

#define APPLY_MIRROR_VERTICAL_FLIP

/***** SUPPORT CAMERA DEFINE *****/
#define IS_VENDOR_SENSOR_COUNT 4                              /* REAR_0, FRONT_0, REAR_2, REAR_3 */

#define LOW_VOLTAGE_DURING_FLASH                              5
#define HIGH_VOLTAGE_DURING_NO_FLASH                          9

#define REAR_SUB_CAMERA            (SENSOR_POSITION_REAR2)    /* Supported Camera Type for rear bokeh */
#define REAR_MACRO_CAMERA          (SENSOR_POSITION_REAR3)    /* Supported Camera Type for rear Macro */

#define CAMERA_REAR2               (REAR_SUB_CAMERA)          /* For Rear2 of SYSFS */
#define CAMERA_REAR3               (REAR_MACRO_CAMERA)        /* For Rear3 of SYSFS */

#define CAMERA_REAR2_MODULEID
#define CAMERA_REAR3_MODULEID
#define CAMERA_REAR_DUAL_CAL

#define USE_CAMERA_ACT_DRIVER_SOFT_LANDING                    /* Using NRC based softlanding */

#define CAMERA_FRONT_FIXED_FOCUS

#define READ_DUAL_CAL_FIRMWARE_DATA

#ifdef USE_KERNEL_VFS_READ_WRITE
#define DUAL_CAL_DATA_PATH "/vendor/firmware/SetMultiCalInfo.bin"
#else
#define DUAL_CAL_DATA_PATH "/vendor/firmware/"
#define DUAL_CAL_DATA_BIN_NAME "SetMultiCalInfo.bin"
#endif

#define DUAL_CAL_DATA_SIZE_DEFAULT (0x080C)

#define CONFIG_SEC_CAL_ENABLE
#define IS_REAR_MAX_CAL_SIZE (0x531A)
#define IS_FRONT_MAX_CAL_SIZE (0x1C84)
#define IS_REAR4_MAX_CAL_SIZE (0x1DC0)

#define CONFIG_SECURE_CAMERA_USE 1

#define CAMERA_EEPROM_SUPPORT_FRONT
#define MODIFY_CAL_MAP_FOR_SWREMOSAIC_LIB

#define CAMERA_STANDARD_CAL_ISP_VERSION 'E'

#define USES_STANDARD_CAL_RELOAD
#define USE_PERSISTENT_DEVICE_PROPERTIES_FOR_CAL /* For cal reload */

#endif /* IS_VENDOR_CONFIG_AAW_V14X_H */
