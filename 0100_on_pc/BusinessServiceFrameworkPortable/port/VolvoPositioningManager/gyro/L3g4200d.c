///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// @file L3g4200d.c
//	This file handles the Gyro chip L3G4200D.
// @project 	        GLY_TCAM
// @subsystem	Application
// @author		Lu Yang
// @Init date	        18-Feb-2019
///////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include "L3g4200d.h"
#include "dr_log.h"
#include <string.h>

#define MAX_GRYO_DATA_SIZE 15

static const char *device = "/dev/spidev1.0";
static uint32_t mode = 0;
static uint8_t bits = 8;
static uint32_t speed = 1000000;    //SPI speed : 1MHz
static uint16_t delay;
static int fd;

static void gyro_read_reg(uint8_t reg, uint8_t* data, uint16_t len, int bIncrement);
static void gyro_write_reg(uint8_t reg, uint8_t data);
static void spi_transfer(int fd, uint8_t *TxBuf, uint8_t *RxBuf, int length);

static void pabort(const char *s)
{
	DR_LOG_ERR("%s", s);
        return;
}

void gyro_dev_init()
{
	int ret = 0;
	fd = open(device, O_RDWR);
	if (fd < 0)
		pabort("can't open device");

	/*
	 * spi mode
	 */
	mode |= SPI_CPOL;
        mode |= SPI_CPHA;

	ret = ioctl(fd, SPI_IOC_WR_MODE32, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE32, &mode);
	if (ret == -1)
		pabort("can't get spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");


        gyro_write_reg(L3G4200D_CTRL_REG5, 0x03);
        gyro_write_reg(L3G4200D_CTRL_REG2, 0x03);
        gyro_write_reg(L3G4200D_CTRL_REG1, 0x3F);

	DR_LOG_INFO("spi mode: 0x%x\n", mode);
	DR_LOG_INFO("bits per word: %d\n", bits);
	DR_LOG_INFO("max speed: %d Hz\n", speed);
}

void gyro_read_yaw(int16_t*  yaw, GRYO_YAW_DIRECTION direction)
{
    axis3bit16_t raw_angular;
    int len =  sizeof(raw_angular);
    
    gyro_read_reg(L3G4200D_OUT_X_L, raw_angular.u8bit, len, 1);
    if( GRYO_YAW_X == direction)
    {
        *yaw = raw_angular.i16bit[0];
    }
    else if( GRYO_YAW_Y == direction)
    {
        *yaw = raw_angular.i16bit[1];
    }
    else if(GRYO_YAW_Z == direction)
    {
        *yaw = raw_angular.i16bit[2];
    }
    else
    { }

 #if 0   
    printf("x = %.4X\n", raw_angular.i16bit[0]);
    printf("y = %.4X\n", raw_angular.i16bit[1]);
    printf("z = %.4X\n", raw_angular.i16bit[2]);
 #endif
}


static void gyro_read_reg(uint8_t reg, uint8_t* data, uint16_t len, int bIncrement)
{
    uint8_t tx_buf[len+1];

    if(1 == bIncrement)
    {
        reg |= 0xC0;
    }
    else
    {
        reg |= 0x80;
    }

    tx_buf[0] = reg;
    len++;
    spi_transfer(fd,tx_buf,data,len);
}

static void gyro_write_reg(uint8_t reg, uint8_t data)
{
    uint8_t tx_buf[2] = {0};
    uint8_t rx_buf[2] = {0};
    tx_buf[0] = reg;
    tx_buf[1] = data;
    spi_transfer(fd, tx_buf, rx_buf, 2);
}

uint8_t get_device_id()
{
    uint8_t device_id = 0;
    int len =  sizeof(device_id);  
    
    gyro_read_reg(L3G4200D_WHO_AM_I, &device_id, len, 0);
    DR_LOG_INFO("get_device_id %.2X", device_id);
    return device_id;
}


static void spi_transfer(int fd, uint8_t *TxBuf, uint8_t *RxBuf, int length)
{
    int ret;
    uint8_t tx[length];
    uint8_t rx[length];    
    memset(rx, 0, sizeof(rx));
    
    int i = 0;
    for(i=0;i<length;i++)
    {
        tx[i] = TxBuf[i];
    }

    for (i = 1; i < sizeof(rx); i++) {
        RxBuf[i] = rx[i];
    }

    for (i = 1; i < length; i++) {
        RxBuf[i] = rx[i];
    }

    for (i = 1; i < sizeof(rx); i++) {
        RxBuf[i-1] = rx[i];
    }
    
    struct spi_ioc_transfer tr = {
    	.tx_buf = (unsigned long)tx,
    	.rx_buf = (unsigned long)rx,
    	.len = length,
    	.delay_usecs = delay,
    	.speed_hz = speed,
    	.bits_per_word = bits,
    };

    if (mode & SPI_TX_QUAD)
    	tr.tx_nbits = 4;
    else if (mode & SPI_TX_DUAL)
    	tr.tx_nbits = 2;
    if (mode & SPI_RX_QUAD)
    	tr.rx_nbits = 4;
    else if (mode & SPI_RX_DUAL)
    	tr.rx_nbits = 2;
    if (!(mode & SPI_LOOP)) {
    	if (mode & (SPI_TX_QUAD | SPI_TX_DUAL))
    		tr.rx_buf = 0;
    	else if (mode & (SPI_RX_QUAD | SPI_RX_DUAL))
    		tr.tx_buf = 0;
    }

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    for (i = 1; i < sizeof(rx); i++) {
        RxBuf[i-1] = rx[i];
    }
     
    if (ret < 1)
    	DR_LOG_ERR("can't send spi message");

    for (i = 1; i < sizeof(rx); i++) {
        RxBuf[i-1] = rx[i];
    }

#if 0
    for (i = 1; i < sizeof(tx); i++) { 
    	if (!(i % 7))
    		puts("");
    	printf("%.2X ", rx[i]);
    }
    puts("");
#endif

}

