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

-   Real Time AGNSS (see section STM proprietary libraries)

-   Predictive Assisted GPS 7 (see section STM proprietary libraries)

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

### STM Teseo HAL source code download
You must place the source in the correct location in the android source tree. Then you have to download the source from the repositories. The following commands will do all:

```bash
$ mkdir <AOSP_DIR>/hardware/stm

$ cd <AOSP_DIR>/hardware/stm


<AOSP_DIR>/hardware/stm $ git clone https://github.com/STMicroelectronics/STADG_Teseo_Android_HAL.git teseo

<AOSP_DIR>/hardware/stm $ cd teseo
```

### Configure the target device


Once you have downloaded all the source code, you have to customize your target device build configuration to integrate the STM Teseo HAL. You need at least to:

-   Require the `gps.$(TARGET_BOARD_PLATFORM)` library in your `PRODUCT_PACKAGES` variable,

-   Add the gps xml permission file and the gps configuration file in the `PRODUCT_COPY_FILES` variable.

To do so, add the following lines in your target device `device-hikey960.mk` file:
```makefile
PRODUCT_COPY_FILES += \\
frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \\
device/linaro/hikey/etc/gps.conf:system/etc/gps.conf

PRODUCT_PACKAGES += gps.hikey960
```

If you are not using Hikey, you must adapt the gps.hikey960 name to your board and the path to your `gps.conf` file.

You should use the template available in `hardware/stm/teseo/etc/gps.conf` to write your own `gps.conf` file.

For the Hikey you also need to configure SELinux to allow access to the tty device from the HAL. To do so, add the following line to `device/linaro/hikey/sepolicy/file_contexts`:

```
/dev/ttyXX u:object_r:console_device:s0
```

And the following line to `device/linaro/hikey/ueventd.hikey.rc`:

```
/dev/ttyXX 0660 gps system
```

where `/dev/ttyXX` is the serial console where the Teseo chip sits.

Finally to **ease development**, add this line to `device/linaro/hikey/hikey960/BoardConfig.mk`:

```
BOARD_KERNEL_CMDLINE := $(BOARD_KERNEL_CMDLINE) androidboot.selinux=permissive
```

And this line to `device/linaro/hikey/vendorsetup.sh`:

```
add_lunch_combo hikey-eng
```


Build Android with the HAL
--------------------------

You just have to issue the following command in the android environment:

```
<AOSP_DIR> $ make -j32
```

The build should contain the following file `/system/lib64/hw/gps.hikey960.so`.


STM proprietary libraries
=========================

### Real Time Assisted GNSS
> The real-time AGNSS is able to provide the approximate current time, the ephemerides, the almanacs to the _Teseo chip_ GNSS engine in a time frame less than the usual time (about 30 seconds) needed to download real ephemeris from the sky. This reduces considerably the time to get fix especially in critical environments when the ephemeris download time could be very long. 
Real-time AGNSS requires a network connection to download assistance data from the server.

### Predictive Assisted GPS 7
> This server based assistance allows fast and accurate GPS performances thanks to a 8KB bi-weekly data transfer. Starting from this downloaded payload, the _Teseo chip_ is capable of computing the ephemeris for up to 14 days, with very high accuracy, for the complete GPS and GLONASS constellations.
Predictive AGPS requires a network connection to download assistance data from the server.

### RAW GNSS measurements and navigation messages
> The _Teseo chip_ is able to report raw measurements data and navigation messages through the HAL GNSS measurements interface. This is available for GPS signals. Please note that the _Teseo chip_ must use a dedicated binary image enabling Carrier Phase measurements.

Please note that the release of STM proprietary libraries is subject to signature of a Software License Agreement (SLA) or of a Non Disclosure Agreement (NDA); please contact an STMicroelectronics sales office and representatives for further information.


Copyright
========
Copyright (C) 2018 STMicroelectronics

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
