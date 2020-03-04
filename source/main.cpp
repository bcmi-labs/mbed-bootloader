// ----------------------------------------------------------------------------
// Copyright 2016-2017 ARM Ltd.
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

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <inttypes.h>

#include "mbed.h"

#include "update-client-paal/arm_uc_paal_update_api.h"
#include "update-client-common/arm_uc_types.h"

#include "mbed_bootloader_info.h"
#include "bootloader_platform.h"
#include "active_application.h"
#include "bootloader_common.h"
#include "mbed_application.h"
#include "upgrade.h"

const arm_uc_installer_details_t bootloader = {
    .arm_hash = BOOTLOADER_ARM_SOURCE_HASH,
    .oem_hash = BOOTLOADER_OEM_SOURCE_HASH,
    .layout   = BOOTLOADER_STORAGE_LAYOUT
};

DigitalOut usb_reset(PJ_4, 0);
DigitalOut video_enable(PJ_2, 0);
DigitalOut video_reset(PJ_3, 0);
DigitalOut led(PK_6);
I2C i2c(PB_7, PB_6);

int main(void)
{
    // this forces the linker to keep bootloader object now that it's not
    // printed anymore
    *const_cast<volatile uint32_t *>(&bootloader.layout);

    /* Use malloc to allocate uint64_t version number on the heap */
    heapVersion = (uint64_t *) malloc(sizeof(uint64_t));
    bootCounter = (uint8_t *) malloc(1);

    /*************************************************************************/
    /* Print bootloader information                                          */
    /*************************************************************************/
    char data[2];

    // LDO1 to 1.0V
    data[0]=0x4c;
    data[1]=0x5;
    i2c.write(8 << 1, data, sizeof(data));
    data[0]=0x4d;
    data[1]=0x3;
    i2c.write(8 << 1, data, sizeof(data));

    // LDO3 to 1.2V
    data[0]=0x52;
    data[1]=0x9;
    i2c.write(8 << 1, data, sizeof(data));
    data[0]=0x53;
    data[1]=0xF;
    i2c.write(8 << 1, data, sizeof(data));

    HAL_Delay(10);

    data[0]=0x9C;
    data[1]=(1 << 7);
    i2c.write(8 << 1, data, sizeof(data));

    // Disable charger led
    data[0]=0x9E;
    data[1]=(1 << 5);
    i2c.write(8 << 1, data, sizeof(data));

    HAL_Delay(10);

    // SW3: set 2A as current limit
    // Helps keeping the rail up at wifi startup
    data[0]=0x42;
    data[1]=(2);
    i2c.write(8 << 1, data, sizeof(data));

    HAL_Delay(10);

    // Change VBUS INPUT CURRENT LIMIT to 1.5A
    data[0]=0x94;
    data[1]=(20 << 3);
    i2c.write(8 << 1, data, sizeof(data));

    // SW2 to 3.3V (SW2_VOLT)
    //I2Cx_Write(8, 0x38, 9);
    //HAL_Delay(1);
    //I2Cx_Write(8, 0x3B, 7);

    HAL_Delay(10);

    usb_reset = 0;
    HAL_Delay(10);
    usb_reset = 1;

    boot_debug("\r\nMbed Bootloader\r\n");

#if MBED_CONF_MBED_TRACE_ENABLE
    mbed_trace_init();
    mbed_trace_print_function_set(boot_debug);
#endif // MBED_CONF_MBED_TRACE_ENABLE

#if MBED_CONF_MBED_BOOTLOADER_STARTUP_DELAY
    ThisThread::sleep_for(MBED_CONF_MBED_BOOTLOADER_STARTUP_DELAY);
#endif // MBED_CONF_MBED_BOOTLOADER_STARTUP_DELAY

    /* Initialize PAL */
    int32_t ucp_result = MBED_CLOUD_CLIENT_UPDATE_STORAGE.Initialize();
    if (ucp_result != ERR_NONE) {
        boot_debug("Failed to initialize update storage\r\n");
        return -1;
    }

    /*************************************************************************/
    /* Update                                                                */
    /*************************************************************************/

    /* Initialize internal flash */
    if (!activeStorageInit()) {
        boot_debug("Failed to initialize active storage\r\n");
        return -1;
    }

    /* Try to update firmware from journal */
    if (upgradeApplicationFromStorage()) {
        /* deinit storage driver */
        activeStorageDeinit();

        /* forward control to ACTIVE application if it is deemed sane */
        boot_debug("booting...\r\n\r\n");
        mbed_start_application(MBED_CONF_APP_APPLICATION_JUMP_ADDRESS);
    }

    /* Reset bootCounter; this allows a user to reapply a new bootloader
       without having to power cycle the device.
    */
    if (bootCounter) {
        *bootCounter = 0;
    }

    boot_debug("Failed to jump to application!\r\n");
    return -1;
}
