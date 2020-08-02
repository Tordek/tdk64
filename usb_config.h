/*
 * File:   usb_config.h
 * Author: Tordek
 *
 * Created on 22 de abril de 2017, 05:09
 */

#ifndef USB_CONFIG_H
#define	USB_CONFIG_H

#define USB_EP0_BUFF_SIZE   8

#define USB_MAX_NUM_INT 1 // Max number of interfaces on any config.
#define USB_MAX_EP_NUMBER 1

// Device descriptor variable
#define USB_USER_DEVICE_DESCRIPTOR &device_dsc
#define USB_USER_DEVICE_DESCRIPTOR_INCLUDE extern USB_DEVICE_DESCRIPTOR const device_dsc

// Config descriptor variable
#define USB_USER_CONFIG_DESCRIPTOR USB_CD_Ptr
#define USB_USER_CONFIG_DESCRIPTOR_INCLUDE extern uint8_t const * const USB_CD_Ptr[]

#define USB_PING_PONG_MODE USB_PING_PONG__FULL_PING_PONG

#define USB_FULL_PING_PONG
#define USB_EP0_OUT_ONLY

#define USB_POLLING

#define USB_PULLUP_OPTION USB_PULLUP_ENABLE
#define USB_TRANSCEIVER_OPTION USB_INTERNAL_TRANSCEIVER
#define USB_SPEED_OPTION USB_FULL_SPEED

// These has been chosen arbitrarily and should be replaced by a valid VID/PID.
#define MY_VID 0x04D8
#define MY_PID 0x005E

#define USB_ENABLE_STATUS_STAGE_TIMEOUTS    //Comment this out to disable this feature.

#define USB_STATUS_STAGE_TIMEOUT     (uint8_t)45   //Approximate timeout in milliseconds, except when

#define USB_SUPPORT_DEVICE

#define USB_NUM_STRING_DESCRIPTORS 3

//#define USB_INTERRUPT_LEGACY_CALLBACKS
#define USB_ENABLE_ALL_HANDLERS
//#define USB_ENABLE_SUSPEND_HANDLER
//#define USB_ENABLE_WAKEUP_FROM_SUSPEND_HANDLER
//#define USB_ENABLE_SOF_HANDLER
//#define USB_ENABLE_ERROR_HANDLER
//#define USB_ENABLE_OTHER_REQUEST_HANDLER
//#define USB_ENABLE_SET_DESCRIPTOR_HANDLER
//#define USB_ENABLE_INIT_EP_HANDLER
//#define USB_ENABLE_EP0_DATA_HANDLER
//#define USB_ENABLE_TRANSFER_COMPLETE_HANDLE

#define USB_USE_HID // We have a HID, bois.

#define HID_INTF_ID             0x00
#define HID_EP 					1
#define HID_INT_OUT_EP_SIZE     32
#define HID_INT_IN_EP_SIZE      64
#define HID_NUM_OF_DSC          1
#define HID_RPT01_SIZE          50

#endif	/* USB_CONFIG_H */

