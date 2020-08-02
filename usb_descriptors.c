/*
 * Author: Guillermo O. «Tordek» Freschi
 * License: GNU GPL v3.0
 */

#include <usb.h>
#include <usb_device_hid.h>

#if defined(COMPILER_MPLAB_C18)
#pragma romdata
#endif

USB_DEVICE_DESCRIPTOR const device_dsc = {
    sizeof (USB_DEVICE_DESCRIPTOR),
    USB_DESCRIPTOR_DEVICE,
    0x0200, // USB 2.0
    0x00, 0x00, 0x00, // Use class/sub/protocol from interfaces
    USB_EP0_BUFF_SIZE,
    MY_VID,
    MY_PID,
    0x0101, // TDK64 v1.01
    0x01, // Index of string containing manufacturer info.
    0x02, // " product info.
    0x00, // " product serial code.
    0x01, // Amount of configurations.
};

uint8_t const configDescriptor1[] = {
    sizeof (USB_CONFIGURATION_DESCRIPTOR),
    USB_DESCRIPTOR_CONFIGURATION,
    DESC_CONFIG_WORD(34), // CARE
    1, // Interfaces
    1, // Index of this config
    0, // Index of string for this config.
    _DEFAULT,
    100, // Max mA consumption.

    /* Interface for this config */
    sizeof (USB_INTERFACE_DESCRIPTOR),
    USB_DESCRIPTOR_INTERFACE,
    0, // Interface number
    0, // Alt settings
    1, // Endpoints
    HID_INTF, // HID class
    0, 0, // Non-boot.
    0, // Interface string index.

    /* HID specific descriptor */
    sizeof (USB_HID_DSC) + sizeof (USB_HID_DSC_HEADER) * HID_NUM_OF_DSC,
    DSC_HID,
    DESC_CONFIG_WORD(0x0111), // HID 1.12
    0, // No country code
    HID_NUM_OF_DSC,
    DSC_RPT, //
    DESC_CONFIG_WORD(HID_RPT01_SIZE),

    /* Endpoint */
    sizeof (USB_ENDPOINT_DESCRIPTOR),
    USB_DESCRIPTOR_ENDPOINT,
    HID_EP | EP_DIR_IN,
    _INTERRUPT,
    DESC_CONFIG_WORD(64), // max packet size
    10, // Poll time in ms
};

const struct {
    uint8_t bLength;
    uint8_t bDscType;
    uint16_t string[1];
}

sd000 = {
    sizeof (sd000), USB_DESCRIPTOR_STRING,
    {
        0x0409
    } // en_US
};

//Manufacturer string descriptor

const struct {
    uint8_t bLength;
    uint8_t bDscType;
    uint16_t string[25];
}

sd001 = {
    sizeof (sd001), USB_DESCRIPTOR_STRING,
    {
        'T', 'o', 'r', 'd', 'e', 'k'
    }
};

//Product string descriptor

const struct {
    uint8_t bLength;
    uint8_t bDscType;
    uint16_t string[13];
}

sd002 = {
    sizeof (sd002), USB_DESCRIPTOR_STRING,
    {
        'N', '6', '4', ' ', 'A', 'd', 'a', 'p', 't', 'e', 'r'
    }
};

//Array of configuration descriptors
uint8_t const * const USB_CD_Ptr[] = {
    (const uint8_t * const) configDescriptor1
};

//Array of string descriptors
uint8_t const * const USB_SD_Ptr[] = {
    (uint8_t const * const) &sd000,
    (uint8_t const * const) &sd001,
    (uint8_t const * const) &sd002
};

const struct {
    uint8_t report[HID_RPT01_SIZE];
} hid_rpt01 = {
    {
        0x05, 0x01, // USAGE_PAGE (Generic Desktop)
        0x09, 0x05, // USAGE (Game Pad)
        0xa1, 0x01, // COLLECTION (Application)
        0x85, 0x01, //   REPORT_ID (1)

        0x05, 0x09, //   USAGE_PAGE (Button)
        0x19, 0x01, //   USAGE_MINIMUM (Button 1)
        0x29, 0x0e, //   USAGE_MAXIMUM (Button 14)
        0x25, 0x01, //   LOGICAL_MAXIMUM (1)
        0x15, 0x00, //   LOGICAL_MINIMUM (0)
        0x75, 0x01, //   REPORT_SIZE (1)
        0x95, 0x0e, //   REPORT_COUNT (14)
        0x81, 0x02, //   INPUT (Data,Var,Abs)

        0x95, 0x02, //   REPORT_COUNT (2)
        0x81, 0x03, //   INPUT (Cnst,Var,Abs)

        0x05, 0x01, //   USAGE_PAGE (Generic Desktop)
        0x09, 0x01, //   USAGE (Pointer)
        0xa1, 0x00, //   COLLECTION (Physical)
        0x09, 0x30, //     USAGE (X)
        0x09, 0x31, //     USAGE (Y)
        0x25, 0x50, //     LOGICAL_MAXIMUM (80)
        0x15, 0xB0, //     LOGICAL_MINIMUM (-80)
        0x75, 0x08, //     REPORT_SIZE (8)
        0x95, 0x02, //     REPORT_COUNT (2)
        0x81, 0x02, //     INPUT (Data,Var,Abs)
        0xc0, //         END_COLLECTION
        0xc0, //       END_COLLECTION
    }
};
