
#include <libusb-1.0/libusb.h>
#include <stdlib.h>
#include <stdio.h>

#include "cp2130.h"

int main(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    libusb_context *ctx = NULL;
    if(libusb_init(&ctx) < 0)
    {
		printf("\n\rERROR: failed in libusb init");
	}
	libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_WARNING);

    cp2130_device_t *spi = cp2130_init(ctx);

    uint8_t ubMajor, ubMinor;
    cp2130_get_version(spi, &ubMajor, &ubMinor);
    printf("\n\rVersion: %0d.%0d", ubMajor, ubMinor);

    uint16_t usVid, usPid;
    uint8_t ubMaxPow, ubPowMode, ubTransferPriority;
    cp2130_get_usb_cfg(spi, &usVid, &usPid, &ubMaxPow, &ubPowMode, &ubMajor, &ubMinor, &ubTransferPriority);
    printf("\n\rVID: 0x%04X", usVid);
    printf("\n\rPID: 0x%04X", usPid);
    printf("\n\rMax Power: %dma", ubMaxPow * 2);
    printf("\n\rPower Mode: 0x%02X", ubPowMode);
    printf("\n\rTransfer Priority: 0x%02X", ubTransferPriority);

    uint8_t ubStr[63];
    memset(ubStr, 0x00, 63);
    cp2130_get_manufacturer_string(spi, ubStr);
    printf("\n\rManufacturer String: %s", ubStr);
    memset(ubStr, 0x00, 63);
    cp2130_get_prod_string(spi, ubStr);
    printf("\n\rProduct String: %s", ubStr);
    memset(ubStr, 0x00, 63);
    cp2130_get_serial(spi, ubStr);
    printf("\n\rSerial: %s", ubStr);

    uint8_t ubPinCfg[20];
    cp2130_get_pin_cfg(spi, ubPinCfg);
    for(uint8_t i = 0; i < 11; i++)
    {
        printf("\n\rGPIO%d OTP ROM cfg: 0x%02X", i, ubPinCfg[i]);
    }

    cp2130_set_gpio_mode_level(spi, CP2130_GPIO0, CP2130_GPIO_OUT_PP, CP2130_GPIO_HIGH);
    cp2130_set_gpio_mode_level(spi, CP2130_GPIO5, CP2130_GPIO_OUT_PP, CP2130_GPIO_LOW);
    cp2130_set_gpio_mode_level(spi, CP2130_GPIO7, CP2130_GPIO_OUT_PP, CP2130_GPIO_LOW);

    cp2130_set_gpio_Values(spi, CP2130_GPIO7_MSK, CP2130_GPIO7_MSK);

    cp2130_set_clockdiv(spi, 3); // 24 MHz / 3 = 8MHz

    cp2130_set_spi_word(spi, CP2130_SPI_CH0, CP2130_SPI_WRD_CS_MODE_PP | CP2130_SPI_WRD_CLK_750K);

    cp2130_set_gpio_cs(spi, CP2130_CS_CH0, CP2130_CS_MD_EN_DIS_OTHERS);

    printf("\n\rTransfering 100 Bytes...");
    uint8_t ubBuf[256];
    for(uint8_t i = 0; i < 100; i++)
    {
        ubBuf[i] = i;
    }
    cp2130_spi_transfer(spi, ubBuf, 100);
    printf("\n\rgot:");
    for(uint8_t i = 0; i < 100; i++)
    {
        printf(" 0x%02X", ubBuf[i]);
    }

    printf("\n\rWriting 256 Bytes...");
    for(uint8_t i = 0; i < 100; i++)
    {
        ubBuf[i] = i;
    }
    cp2130_spi_write(spi, ubBuf, 256);

    printf("\n\rReading 256 Bytes...");
    memset(ubBuf, 0x00, 256);
    cp2130_spi_read(spi, ubBuf, 100);
    printf("\n\rgot:");
    for(uint8_t i = 0; i < 256; i++)
    {
        printf(" 0x%02X", ubBuf[i]);
    }

    printf("\n\r");

    cp2130_free(spi);

	if(ctx)
		libusb_exit(ctx);
}