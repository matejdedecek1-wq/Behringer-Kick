/*
 * Copyright (c) 2021 MUSIC Tribe
 *
 * This file is part of Kick.
 *
 * Kick is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Kick is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Kick. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#pragma once

#include <iostream>

#define SDCARD_PATH  "/mnt/sdcard"
#define USBDISK_PATH "/mnt/flash"
#define PCMODE_PATH  "/home/Kick/Firmware_Update"

#define INFO         ( ( 'I' ) | ( 'N' << 8 ) | ( 'F' << 16 ) | ( 'O' << 24 ) )
#define FAIL         ( ( 'F' ) | ( 'A' << 8 ) | ( 'I' << 16 ) | ( 'L' << 24 ) )
#define OKAY         ( ( 'O' ) | ( 'K' << 8 ) | ( 'A' << 16 ) | ( 'Y' << 24 ) )
#define DATA         ( ( 'D' ) | ( 'A' << 8 ) | ( 'T' << 16 ) | ( 'A' << 24 ) )

namespace mt
{

    enum class SwUpdateMode_t
    {
        PC,
        SDCARD,
        USBDISK,
        OTA,
        NONE
    };

    typedef enum
    {
        SW_SUCCESS = 0,
        SW_FAILED,
        USB_FILEOPEN_E,
        IMAGE_NOTFOUND,
        IMAGE_FOUND,
        IMAGE_DYCRYPTION_E,
        IMAGE_EXTRACTION_E,
        MCU_IMAGE_NOTFOUND,
        PC_CONNECTION_E
    } SwUpdateErrorCode_t;

    typedef enum
    {
        IDLE,
        START,
        RUN,
        SUCCESS,
        FAILURE,
        DOWNLOAD,
        DONE,
        SUBPROCESS,
        PROGRESS,
        LINUX_SUCCESS,
        LINUX_FAILURE,
    } RECOVERY_STATUS;

    typedef enum
    {
        U_INIT,
        U_DOWNLOAD,
        U_DONE,
        U_SUCCESS,
        U_FAILED,
        U_TIMEOUT
    } USB_STATUS;

    typedef enum
    {
        LINUXUPDATE,
        MCUUPDATE,
        NONE
    } SwupdateType_t;

    enum MCUUpdateAction // subcommands of software update;
    {
        EN_SW_UPDATE_INITIATE = 0, // send to mcu when user initiates transfer
        EN_SW_UPDATE_BEGIN_TRANSFER, // sent from mcu to app when its ready to receive data
        EN_SW_UPDATE_DATA, //data transfer params totalDataBytes
        EN_SW_UPDATE_DATABASE,
        EN_SW_UPDATE_END, // sent when all the software update data is transfered
        EN_SW_UPDATE_ACK,
        EN_SW_UPDATE_FAILURE,
        EN_SW_UPDATE_IDLE
    };

    struct progress_msg
    {
        unsigned int    magic; /* Magic Number */
        RECOVERY_STATUS status; /* Update Status (Running, Failure) */
        unsigned int    dwl_percent; /* % downloaded data */
        unsigned int    nsteps; /* No. total of steps */
        unsigned int    cur_step; /* Current step index */
        unsigned int    cur_percent; /* % in current step */
        char            cur_image[256]; /* Name of image to be installed */
        char            hnd_name[64]; /* Name of running hanler */
        unsigned int    infolen; /* Len of data valid in info */
        char            info[2048]; /* additional information about install */
    };

    typedef struct swupdate_msg
    {
        SwupdateType_t  imageType = NONE;
        RECOVERY_STATUS command_status;
        unsigned int    nsteps;
        char            curimage[256];
        unsigned int    totalper;
        unsigned int    cur_percent;
        unsigned int    curstep;
    } swupdatemsg_t;

    typedef struct usb_msg
    {
        USB_STATUS command_status;
        size_t     totalfilesize;
        size_t     downloadfilesize;
        double     percentage;

    } usbmsg_t;

    typedef std::function<void( swupdatemsg_t& )> SwupdateStatus;
    typedef std::function<void( usbmsg_t& )>      UsbStatus;

}
