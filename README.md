Index
=====
```
* Introduction
* Hardware
* Software architecture and Integration details
* STM proprietary libraries
* Copyright

```
Introduction
============

The STM Android Teseo Hardware Abstraction Layer (HAL) defines a standard interface for the STMicroelectronics (STM) Teseo family of Global Navigation System ICs. The Teseo HAL library is packaged into modules files (.so) and loaded by the Android system at the appropriate time.

Currently supported GNSS receivers are:
STA8088, STA8089/STA8090


The STM Teseo HAL provides the following features:
-   Location
-   Satellite information
-   Geofencing
-   Support for raw measurements and navigation messages


Hardware
========
The [\[HiKey960 from Lenovator\]](http://www.96boards.org/product/hikey960/) has been used as the reference development platform for the Teseo HAL. If you are using another development platform, be sure that you can connect it properly to the UART of your Teseo chip.

In the following sections, the terms *Hikey* and *Teseo Chip* refers, respectively, to the HiKey960 development platform and to any GNSS IC of the STMicroelectronics  Teseo family.


Software architecture and Integration details
=============================================
The Teseo HAL is written in C++ language using object-oriented design. The _HAL Manager_ module is called during the boot phase to instantiate and initialize objects, signals and slots. The _HAL Location Service Proxy_ module handles the events sent by the _Android Location Service_ and by the other HAL modules.

As soon as the navigation is started by an application, the _HAL Virtual device_ starts to parse and to decode the NMEA data coming from the Teseo chip. It updates its own data model, and then send updates to the _HAL location service proxy_ and to any other module which requires data updates as well.

When not used by any application the Teseo chip isn’t sleeping. It is up to you to wake up and suspend the Teseo chip when the navigation is started and stopped.

### Compatibility
|           | Android 8.1 | Android 9.0 | Android 10.0 |
|-----------|:-----------:|:-----------:|:------------:|
| Gnss v1.0 | Yes         | Yes         | Yes          |
| Gnss v1.1 | No          | Yes         | Yes          |
| Gnss v2.0 | No          | No          | Yes          |

### STM Teseo HAL source code download
You must place the source in the correct location in the Android source tree. Then you have to download the source from the repositories. The following commands will download Teseo HAL source code:

```bash
$ mkdir -p <AOSP_DIR>/vendor/st/teseo/
$ cd <AOSP_DIR>/vendor/st/teseo/

# For Android 8.1
<AOSP_DIR>/vendor/st/teseo $ git clone https://github.com/STMicroelectronics/STADG_Teseo_Android_HAL.git hal -b TESEO_ANDROID_8.1_HAL_M5
# For Android 9.0
<AOSP_DIR>/vendor/st/teseo $ git clone https://github.com/STMicroelectronics/STADG_Teseo_Android_HAL.git hal -b TESEO_ANDROID_9.0_HAL_M5
# For Android 10.0
<AOSP_DIR>/vendor/st/teseo $ git clone https://github.com/STMicroelectronics/STADG_Teseo_Android_HAL.git hal -b TESEO_ANDROID_10.0_HAL_M5

<AOSP_DIR>/vendor/st/teseo $ cd hal
```
### Additional libraries
The data assistance, the raw measurements and navigation messages are available in separate repositories. They are available upon request to your local ST Sales and under NDA.
**Please take note that STM Teseo HAL require those additional libraries to be able to be compiled.**
The data assistance interfaces source code and the raw measurement source code must be placed in the following directories respectively:
```bash
<AOSP_DIR>/vendor/st/teseo/hal/X.Y/libstagps
<AOSP_DIR>/vendor/st/teseo/hal/X.Y/libteseo.agnss
<AOSP_DIR>/vendor/st/teseo/hal/X.Y/libteseo.straw
```
where X and Y is the Gnss version number.
- Gnss v1.0	: X = 1, Y = 0
- Gnss v1.1	: X = 1, Y = 1
- Gnss v2.0	: X = 2, Y = 0
**Please adjust X and Y accordingly for the rest of this README.**

The version of the additional libraries also must be placed accordingly:
- libraries for Gnss v1.0 must be placed at <AOSP_DIR>/vendor/st/teseo/hal/1.0/
- libraries for Gnss v1.1 must be placed at <AOSP_DIR>/vendor/st/teseo/hal/1.1/
- libraries for Gnss v2.0 must be placed at <AOSP_DIR>/vendor/st/teseo/hal/2.0/

### Android GNSS HAL Interfaces
Starting from the release of Android 10.0 (and we implement it also to Android 8.1 and Android 9.0), we are using Full Binderized HAL, which require modified Android GNSS HAL Interfaces.
You must place the source in the correct location in the Android source tree. Then you have to download the source from the repositories. The following commands will download modified Android GNSS HAL Interfaces:
```bash
$ mkdir -p <AOSP_DIR>/vendor/st/teseo/
$ cd <AOSP_DIR>/vendor/st/teseo/

# For Android 8.1
<AOSP_DIR>/vendor/st/teseo $ git clone https://github.com/STMicroelectronics/STADG_Teseo_Android_Interfaces.git interfaces -b TESEO_ANDROID_8.1_HAL_M5
# For Android 9.0
<AOSP_DIR>/vendor/st/teseo $ git clone https://github.com/STMicroelectronics/STADG_Teseo_Android_Interfaces.git interfaces -b TESEO_ANDROID_9.0_HAL_M5
# For Android 10.0
<AOSP_DIR>/vendor/st/teseo $ git clone https://github.com/STMicroelectronics/STADG_Teseo_Android_Interfaces.git interfaces -b TESEO_ANDROID_10.0_HAL_M5

<AOSP_DIR>/vendor/st/teseo $ cd interfaces
```
### Configure the target device
Target device used on this README is [\[HiKey960 from Lenovator\]](http://www.96boards.org/product/hikey960/).
Once all the source available, please customize target device build configuration to integrate the Teseo Android HAL.
1. Add the gps xml permission file in `PRODUCT_COPY_FILES` variable.
2. Add the gps configuration file in `PRODUCT_COPY_FILES` variable.
Please use the template available in `<AOSP_DIR>/vendor/st/teseo/hal/X.Y/etc/gps.conf` to create customized gps configuration file.
3. Add GNSS HAL in `PRODUCT_PACKAGES` variable.

To do so, add the following lines in target device `<AOSP_DIR>/device/linaro/hikey/hikey960/device-hikey960.mk` file
```makefile
# Add the gps xml permission file
PRODUCT_COPY_FILES += \
frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml

# Add the gps configuration file
PRODUCT_COPY_FILES += \
vendor/st/teseo/hal/X.Y/etc/gps.conf:$(TARGET_COPY_OUT_VENDOR)/etc/gps/gps.conf

# Add GNSS HAL
PRODUCT_PACKAGES += \
android.hardware.gnss@X.Y-service-st
```
4. Create new SELinux policy for vendor service.

To do so, create new file `<AOSP_DIR>/device/linaro/hikey/sepolicy/hal_gnss_st.te` in target device with content below
```
type hal_gnss_st, domain;
type hal_gnss_st_exec, exec_type, vendor_file_type, file_type;

hal_server_domain(hal_gnss_st, hal_gnss)
init_daemon_domain(hal_gnss_st)
```
5. Configure SELinux to allow execution of vendor service.
6. Configure SELinux to allow access to the tty device.

To do so add the following lines in target device SELinux policy file `<AOSP_DIR>/device/linaro/hikey/sepolicy/file_contexts`
```
# Allow execution of vendor service
/(vendor|system/vendor)/bin/hw/android\.hardware\.gnss@X\.Y-service-st      u:object_r:hal_gnss_st_exec:s0

# Allow access to the tty device
/dev/ttyXX           u:object_r:console_device:s0
```
And add the following line to ueventd script file `<AOSP_DIR>/device/linaro/hikey/ueventd.common.rc`
```
# Allow access to the tty device
/dev/ttyXX     0660 system root
```
where `/dev/ttyXX` is the serial console where the Teseo chip sits.

7. **OPTIONAL**
To _ease development_ add this line to target device board configuration `<AOSP_DIR>/device/linaro/hikey/hikey960/BoardConfig.mk`
```
# To ease development
BOARD_KERNEL_CMDLINE += androidboot.selinux=permissive
```
8. **OPTIONAL**
To enable ADB over network on port 5555, add the following lines in target device
`<AOSP_DIR>/device/linaro/hikey/hikey960/device-hikey960.mk` file
```
# Enable ADB over network on port 5555
PRODUCT_PROPERTY_OVERRIDES += persist.adb.tcp.port=5555
```

Build Android with the HAL
--------------------------
You just have to issue the following command in the android environment:

```
<AOSP_DIR> $ source build/envsetup.sh
<AOSP_DIR> $ lunch hikey960-userdebug
<AOSP_DIR> $ make –j$(nproc --all)
```

The build should contain the following file
- For Android 8.1
  - `/system/lib/android.hardware.gnss@1.0.so`
  - `/system/lib64/android.hardware.gnss@1.0.so`
  - `/system/etc/permissions/android.hardware.location.gps.xml`
  - `/vendor/bin/hw/android.hardware.gnss@1.0-service-st`
  - `/vendor/etc/init/android.hardware.gnss@1.0-service-st.rc`
  - `/vendor/etc/gps/gps.conf`
- For Android 9.0
  - `/system/lib/android.hardware.gnss@1.0.so`
  - `/system/lib/android.hardware.gnss@1.1.so`
  - `/system/lib64/android.hardware.gnss@1.0.so`
  - `/system/lib64/android.hardware.gnss@1.1.so`
  - `/system/etc/permissions/android.hardware.location.gps.xml`
  - `/vendor/bin/hw/android.hardware.gnss@1.1-service-st`
  - `/vendor/etc/init/android.hardware.gnss@1.1-service-st.rc`
  - `/vendor/etc/gps/gps.conf`
- For Android 10.0
  - `/system/lib/android.hardware.gnss@1.0.so`
  - `/system/lib/android.hardware.gnss@1.1.so`
  - `/system/lib/android.hardware.gnss@2.0.so`
  - `/system/lib64/android.hardware.gnss@1.0.so`
  - `/system/lib64/android.hardware.gnss@1.1.so`
  - `/system/lib64/android.hardware.gnss@2.0.so`
  - `/system/etc/permissions/android.hardware.location.gps.xml`
  - `/vendor/bin/hw/android.hardware.gnss@2.0-service-st`
  - `/vendor/etc/init/android.hardware.gnss@2.0-service-st.rc`
  - `/vendor/etc/gps/gps.conf`

STM proprietary libraries
=========================

### RAW GNSS measurements and navigation messages
> The _Teseo chip_ is able to report raw measurements data and navigation messages through the dedicated HAL GNSS interface. This is available for GPS, GLONASS, BEIDOU and GALILEO constellations (except GALILEO navigation messages).
**Please note that the _Teseo chip_ must use a dedicated binary image enabling Carrier Phase measurements.**

Please note that the release of STM proprietary libraries is subject to signature of a Software License Agreement (SLA) or of a Non Disclosure Agreement (NDA); please contact an STMicroelectronics sales office and representatives for further information.


Copyright
========
Copyright (C) 2020 STMicroelectronics

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

