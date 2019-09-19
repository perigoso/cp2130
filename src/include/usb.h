#ifndef __USB_H
#define __USB_H

#include <libusb-1.0/libusb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "debug_macros.h"
#include "utils.h"

// OTP ROM WRITE PROTECT
#define USB_OTP_ROM_WRITE_PROTECT    // prevents unwanted writes to the one time programmable prom

// Product and vendor ids
#define USB_DEFAULT_VID  0x10C4      // Silicon Labs VID
#define USB_DEFAULT_PID  0x0000      // default pid Silabs allocated for usb

// EndPoints
#define USB_EP_OUT      0x01
#define USB_EP_IN       0x81

// Request types
#define USB_REQ_VENDOR   0x40

// memory Key
#define USB_MEM_KEY  0xA5F1

// Command ID's
// Data Transfer Commands (Bulk Transfers)
#define USB_CMDID_READ           0x00
#define USB_CMDID_WRITE          0x01
#define USB_CMDID_WRITEREAD      0x02
#define USB_CMDID_READRTR        0x04
// Configuration and Control Commands (Control Transfers)
#define USB_CMDID_GET_CLK_DIV    0x46
#define USB_CMDID_SET_CLK_DIV    0x47
#define USB_CMDID_GET_EVNT_CNT   0x44
#define USB_CMDID_SET_EVNT_CNT   0x45
#define USB_CMDID_GET_FULL_TH    0x34
#define USB_CMDID_SET_FULL_TH    0x35
#define USB_CMDID_GET_GPIO_CS    0x24
#define USB_CMDID_SET_GPIO_CS    0x25
#define USB_CMDID_GET_GPIO_MDLVL 0x22
#define USB_CMDID_SET_GPIO_MDLVL 0x23
#define USB_CMDID_GET_GPIO_VAL   0x20
#define USB_CMDID_SET_GPIO_VAL   0x21
#define USB_CMDID_GET_RTR_STATE  0x36
#define USB_CMDID_RTR_STOP       0x37
#define USB_CMDID_GET_SPI_WORD   0x30
#define USB_CMDID_SET_SPI_WORD   0x31
#define USB_CMDID_GET_SPI_DELAY  0x32
#define USB_CMDID_SET_SPI_DELAY  0x33
#define USB_CMDID_GET_VERSION    0x11
#define USB_CMDID_RESET          0x01
#define SI_USBXPRESS_REQUEST     0x02
#define SI_USBXPRESS_CLEAR_TO_SEND     0x02
#define SI_USBXPRESS_NOT_CLEAR_TO_SEND     0x04

// device handle
typedef struct usb_dev_t
{
    uint8_t ubKernelAttached;
    uint16_t usTimeout;
    libusb_device_handle *pDev;
} usb_dev_t;

// initializer and destructor
usb_dev_t *usb_init(libusb_context *ctx, uint16_t usVid, uint16_t usPid);
void usb_free(usb_dev_t *pCpDev);

inline void usb_set_timeout(usb_dev_t *pCpDev, uint16_t usTimeout)
{
    pCpDev->usTimeout = usTimeout;
}

// Data Transfer Commands (Bulk Transfers)
void usb_spi_write(usb_dev_t *pCpDev, uint8_t *pubData, uint32_t ulLen);
void usb_spi_read(usb_dev_t *pCpDev, uint8_t *pubData, uint32_t ulLen);

// Configuration and Control Commands (Control Transfers)
void usb_reset(usb_dev_t *pCpDev);
void usb_enable(usb_dev_t *pCpDev);
void usb_disable(usb_dev_t *pCpDev);

#endif // __USB_H