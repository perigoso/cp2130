#include <libusb-1.0/libusb.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "debug_macros.h"
#include "usb.h"

int main(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    libusb_context *ctx = NULL;
    if(libusb_init(&ctx) < 0)
    {
		printf("error: failed in libusb init\n\r");
        return -1;
	}
	libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_WARNING);

    usb_dev_t *pUsb_dev = usb_init(ctx, USB_DEFAULT_VID, USB_DEFAULT_PID);

    if(!pUsb_dev)
    {
        printf("error: failed to initialize device\n\r");
        return -1;
    }

    uint8_t ubBuf[255] = {};

    usb_reset(pUsb_dev);
    usb_enable(pUsb_dev);

    printf("\n\rWriting 8 Bytes...\n\r");
    for(uint8_t ubCount = 0; ubCount < 8; ubCount++)
    {
        ubBuf[ubCount] = ubCount;
    }
    //usb_spi_write(pUsb_dev, ubBuf, 50);
    //usb_spi_write(pUsb_dev, ubBuf, 0);
    //usb_spi_write(pUsb_dev, ubBuf, 8);

    printf("\n\rReading 64 Bytes...\n\r");
    memset(ubBuf, 0x00, 256);
    usb_spi_read(pUsb_dev, ubBuf, 64);
    printf("got:\n\r");
    for(uint8_t ubCount = 0; ubCount < 64; ubCount++)
    {
        printf(" 0x%02X", ubBuf[ubCount]);
    }
    printf("\n\r\n\r");

    printf("\n\rReading 64 Bytes...\n\r");
    memset(ubBuf, 0x00, 256);
    usb_spi_read(pUsb_dev, ubBuf, 64);
    printf("got:\n\r");
    for(uint8_t ubCount = 0; ubCount < 64; ubCount++)
    {
        printf(" 0x%02X", ubBuf[ubCount]);
    }
    printf("\n\r\n\r");

    usb_disable(pUsb_dev);
    usb_free(pUsb_dev);

	if(ctx)
		libusb_exit(ctx);
}