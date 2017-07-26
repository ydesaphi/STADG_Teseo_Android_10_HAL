LOCAL_PATH := $(call my-dir)


ifeq ($(shell test -d $(LOCAL_PATH)/libstagps && echo true),true)
TESEO_STAGPS_ENABLED := true
else
TESEO_STAGPS_ENABLED := false
endif

include $(call all-subdir-makefiles)
