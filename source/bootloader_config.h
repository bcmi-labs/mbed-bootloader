// ----------------------------------------------------------------------------
// Copyright 2018 ARM Ltd.
//
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------------------------------------------------------

#ifndef BOOTLOADER_CONFIG_H
#define BOOTLOADER_CONFIG_H

/* MAX_FIRMWARE_LOCATIONS */
#if defined(MBED_CONF_UPDATE_CLIENT_STORAGE_LOCATIONS) && \
    MBED_CONF_UPDATE_CLIENT_STORAGE_LOCATIONS > 0
#define MAX_FIRMWARE_LOCATIONS MBED_CONF_UPDATE_CLIENT_STORAGE_LOCATIONS
#endif

#if !defined(MAX_FIRMWARE_LOCATIONS) || MAX_FIRMWARE_LOCATIONS <= 0
#error "configure update-client.storage-locations or MAX_FIRMWARE_LOCATIONS in mbed_app.json\n" \
"To use pre configured profiles: mbed compile --app-config configs/<config>.json"
#endif

/* FIRMWARE_METADATA_HEADER_ADDRESS */
#if defined(MBED_CONF_UPDATE_CLIENT_APPLICATION_DETAILS)
#define FIRMWARE_METADATA_HEADER_ADDRESS MBED_CONF_UPDATE_CLIENT_APPLICATION_DETAILS
#endif

#if !defined(FIRMWARE_METADATA_HEADER_ADDRESS)
#error "configure update-client.application-details or FIRMWARE_METADATA_HEADER_ADDRESS in mbed_app.json\n" \
"To use pre configured profiles: mbed compile --app-config configs/<config>.json"
#endif

#define BOOTLOADER_CONFIG_MAGIC   0xA0
#define BOOTLOADER_VERSION        15

#define PORTENTA_USB_SPEED_HIGH   1
#define PORTENTA_USB_SPEED_FULL   2

#ifndef PORTENTA_USB_SPEED
#ifdef USE_USB_HS
#define PORTENTA_USB_SPEED    PORTENTA_USB_SPEED_HIGH
#else
#define PORTENTA_USB_SPEED    PORTENTA_USB_SPEED_FULL
#endif
#endif

#define USE_PLL_HSE_EXTC     0x8  // Use external clock (ST Link MCO)
#define USE_PLL_HSE_XTAL     0x4  // Use external xtal (X3 on board - not provided by default)
#define USE_PLL_HSI          0x2  // Use HSI internal clock

#ifndef PORTENTA_HAS_ETHERNET
#define PORTENTA_HAS_ETHERNET   1
#endif

#ifndef PORTENTA_HAS_WIFI
#define PORTENTA_HAS_WIFI       1
#endif

#ifndef PORTENTA_RAM_SIZE
#define PORTENTA_RAM_SIZE       8
#endif

#ifndef PORTENTA_QSPI_SIZE
#define PORTENTA_QSPI_SIZE      16
#endif

#ifndef PORTENTA_HAS_VIDEO
#define PORTENTA_HAS_VIDEO      1
#endif

#ifndef PORTENTA_HAS_CRYPTO
#define PORTENTA_HAS_CRYPTO     1
#endif

#ifndef PORTENTA_EXTCLOCK
#define PORTENTA_EXTCLOCK       25
#endif

#endif // BOOTLOADER_CONFIG_H
