// SiLabs USB via LibUSB

#include "usb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief brief
 *
 * @param ctx libusb context pointer
 * @param usVid open device with this vid
 * @param usPid open device with this pid
 * @return usb_dev_t* returns pointer to device handler on success, NULL on failure
 */
usb_dev_t *usb_init(libusb_context *ctx, uint16_t usVid, uint16_t usPid)
{

    if(!ctx)
    {
        DBGPRINTLN_CTX("error: invalid libUSB context.");
        return NULL;
    }

    DBGPRINTLN_CTX("allocating space for device...");
    usb_dev_t *pUsb_dev = (usb_dev_t *)malloc(sizeof(usb_dev_t));
    if(!pUsb_dev)
    {
        DBGPRINTLN_CTX("error: device space allocation failed.");
        return NULL;
    }

    pUsb_dev->pDev = NULL;
    pUsb_dev->ubKernelAttached = 0;
    pUsb_dev->usTimeout = 0;

    DBGPRINTLN_CTX("opening device...");
    pUsb_dev->pDev = libusb_open_device_with_vid_pid(ctx, usVid, usPid);
    if(pUsb_dev->pDev == NULL)
    {
        DBGPRINTLN_CTX("error: libusb could not open device.");
        free(pUsb_dev);
        return NULL;
    }

    DBGPRINTLN_CTX("checking if attached to kernel...");
    if(libusb_kernel_driver_active(pUsb_dev->pDev, 0) != 0)
    {
        DBGPRINTLN_CTX("device was atached to kernel, detaching...");
        libusb_detach_kernel_driver(pUsb_dev->pDev, 0);
        pUsb_dev->ubKernelAttached = 1;
    }

    DBGPRINTLN_CTX("claiming interface...");
    if(libusb_claim_interface(pUsb_dev->pDev, 0) < 0)
    {
        DBGPRINTLN_CTX("error: failed to claim interface.");
        if(pUsb_dev->ubKernelAttached)
            libusb_attach_kernel_driver(pUsb_dev->pDev, 0);

        libusb_close(pUsb_dev->pDev);
        free(pUsb_dev);
        return NULL;
    }

    DBGPRINTLN_CTX("initialization success.");
    return pUsb_dev;
}
void usb_free(usb_dev_t *pCpDev)
{
    if(!pCpDev)
    {
        DBGPRINTLN_CTX("error: invalid device.");
        return;
    }

    libusb_release_interface(pCpDev->pDev, 0);

    if(pCpDev->ubKernelAttached)
    {
        DBGPRINTLN_CTX("reattaching kernel...");
        libusb_attach_kernel_driver(pCpDev->pDev, 0);
    }

    libusb_close(pCpDev->pDev);

    free(pCpDev);

    DBGPRINTLN_CTX("device freed...");
}

static void usb_control_transfer(usb_dev_t *pCpDev, uint8_t ubReqType, uint8_t ubRequest, uint8_t * pubData, uint16_t usLen, uint16_t usWvalue, uint16_t usWindex)
{
    if(!pCpDev)
    {
        DBGPRINTLN_CTX("error: invalid device");
        return;
    }

    int32_t ulR = libusb_control_transfer(pCpDev->pDev, ubReqType, ubRequest, usWvalue, usWindex, pubData, usLen, pCpDev->usTimeout);
    if(ulR < 0)
        DBGPRINTLN_CTX("error: failed with return code %d.", ulR);
}
static void usb_bulk_transfer(usb_dev_t *pCpDev, uint8_t ubEp, uint8_t * pubData, uint32_t ulLen)
{
    if(!pCpDev)
    {
        DBGPRINTLN_CTX("error: invalid device");
        return;
    }

    uint32_t ulTransferred;
    uint32_t ulR = libusb_bulk_transfer(pCpDev->pDev, ubEp, pubData, ulLen, (int *)(&ulTransferred), pCpDev->usTimeout);
    if(ulR < 0)
    {
        DBGPRINTLN_CTX("error: failed with return code %d", ulR);
        return;
    }

    if(ulTransferred < ulLen)
        DBGPRINTLN_CTX("error: failed short, got %d bytes of %d requested", ulTransferred, ulLen);
}

// Data Transfer Commands (Bulk Transfers)
void usb_spi_write(usb_dev_t *pCpDev, uint8_t *pubData, uint32_t ulLen)
{
    if(!pubData)
    {
        DBGPRINTLN_CTX("error: null data buffer.");
        return;
    }

    uint8_t *pubCmdBuf = (uint8_t *)malloc(MIN(ulLen + 8, 64u));
    if(!pubCmdBuf)
    {
        DBGPRINTLN_CTX("error: failed to allocate Buffer. ");
        return;
    }
    memset(pubCmdBuf, 0x00, MIN(ulLen + 8, 64u));

	pubCmdBuf[0] = 0x00;  // reserved must be 0x00
    pubCmdBuf[1] = 0x00;  // reserved must be 0x00
    pubCmdBuf[2] = USB_CMDID_WRITE;  // Command ID
    pubCmdBuf[3] = 0x00;  // reserved
    pubCmdBuf[7] = (uint8_t)((ulLen >> 24) & 0xFF);    // Length
    pubCmdBuf[6] = (uint8_t)((ulLen >> 16) & 0xFF);    // Length
    pubCmdBuf[5] = (uint8_t)((ulLen >> 8) & 0xFF);     // Length
    pubCmdBuf[4] = (uint8_t)(ulLen & 0xFF);        // Length Little Endian

    memcpy((pubCmdBuf + 8u), pubData, MIN(ulLen, 56u));

	// send the command and first 56 or fewer bytes
	usb_bulk_transfer(pCpDev, USB_EP_OUT, pubCmdBuf, MIN(ulLen + 8u, 64u));
	// send the remainder
	for(uint32_t ulTransfered = 56; ulTransfered < ulLen; ulTransfered += 64u)
    {
        usb_bulk_transfer(pCpDev, USB_EP_OUT, pubData + ulTransfered, MIN(ulLen - ulTransfered, 64u));
    }
}
void usb_spi_read(usb_dev_t *pCpDev, uint8_t *pubData, uint32_t ulLen)
{
    usb_bulk_transfer(pCpDev, USB_EP_IN, pubData, ulLen);

    return;

    if(!pubData)
    {
        DBGPRINTLN_CTX("error: null data buffer.");
        return;
    }

    uint8_t *pubCmdBuf = (uint8_t *)malloc(MIN(ulLen + 8, 64u));
    if(!pubCmdBuf)
    {
        DBGPRINTLN_CTX("error: failed to allocate Buffer. ");
        return;
    }
    memset(pubCmdBuf, 0x00, MIN(ulLen + 8, 64u));

	pubCmdBuf[0] = 0x00;  // reserved must be 0x00
    pubCmdBuf[1] = 0x00;  // reserved must be 0x00
    pubCmdBuf[2] = USB_CMDID_READ;  // Command ID
    pubCmdBuf[3] = 0x00;  // reserved
    pubCmdBuf[7] = (uint8_t)((ulLen >> 24) & 0xFF);    // Length
    pubCmdBuf[6] = (uint8_t)((ulLen >> 16) & 0xFF);    // Length
    pubCmdBuf[5] = (uint8_t)((ulLen >> 8) & 0xFF);     // Length
    pubCmdBuf[4] = (uint8_t)(ulLen & 0xFF);        // Length Little Endian

	usb_bulk_transfer(pCpDev, USB_EP_OUT, pubCmdBuf, 8u);   // send the command
	// fetch the data
	for(uint32_t ulTransfered = 0; ulTransfered < ulLen; ulTransfered += 64u)
    {
		usb_bulk_transfer(pCpDev, USB_EP_IN, pubData + ulTransfered, MIN(ulLen - ulTransfered, 64u));
	}
}

// Configuration and Control Commands (Control Transfers)
void usb_reset(usb_dev_t *pCpDev)
{
    usb_control_transfer(pCpDev, USB_REQ_VENDOR, SI_USBXPRESS_REQUEST, NULL, 0, USB_CMDID_RESET, 0);
}

void usb_enable(usb_dev_t *pCpDev)
{
    usb_control_transfer(pCpDev, USB_REQ_VENDOR, SI_USBXPRESS_REQUEST, NULL, 0, SI_USBXPRESS_CLEAR_TO_SEND, 0);
}

void usb_disable(usb_dev_t *pCpDev)
{
    usb_control_transfer(pCpDev, USB_REQ_VENDOR, SI_USBXPRESS_REQUEST, NULL, 0, SI_USBXPRESS_NOT_CLEAR_TO_SEND, 0);
}

#ifdef __cplusplus
}
#endif