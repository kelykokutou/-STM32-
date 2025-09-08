#ifndef __BH1750_H
#define __BH1750_H

#include "main.h"
#include "i2c.h"

#define BH1750_Address_H	0b1011100
#define BH1750_Address_L	0x46

#define BH1750_PowerDown	0x00
#define BH1750_PowerOn		0x01
#define BH1750_Reset			0x07

#define BH1750_Continuously_H	0x10
#define BH1750_Continuously_H	0x11
#define BH1750_Continuously_L	0x13

#define BH1750_Onetime_H			0x20
#define BH1750_Onetime_H			0x21
#define BH1750_Onetime_L			0x23

#define BH1750_TX(tx,num) 	HAL_I2C_Master_Transmit(&hi2c1, 0x46, tx, num, num)

#define BH1750_RX(rx,num) 	HAL_I2C_Master_Receive(&hi2c1, 0x46, rx, num, num)

void BH1750_Init();
unsigned int BH1750_GetValue();

#endif
