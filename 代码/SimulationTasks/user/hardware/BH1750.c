#include "BH1750.h"

unsigned char BH1750_send_buffer[8];
unsigned char BH1750_read_buffer[8];

unsigned int brightness_cache = 0;
//BH1750初始化函数
void BH1750_Init() {
	BH1750_TX(BH1750_PowerDown, 1);//向BH1750发送重启命令
	HAL_Delay(100);//等待100ms以保证命令完成
	BH1750_send_buffer[0] = BH1750_PowerOn;//向发送队列装载启动命令
	BH1750_send_buffer[1] = BH1750_Continuously_H;//向发送队列装载模式设置命令
	BH1750_TX(&BH1750_send_buffer[0], 1);//发送队列里的第一条命令
	BH1750_TX(&BH1750_send_buffer[1], 1);//发送队列里的第二条命令
	//此处的发送队列本来是用于连续发送命令的，但实际连续发送命令会导致第二条命令无法识别，于是分开发送
	//这里就多出来一个没什么用的发送队列
}
//BH1750光照值获取函数
unsigned int BH1750_GetValue() {
	BH1750_RX(BH1750_read_buffer, 2);//从BH1750读取两个字节的数据
	brightness_cache = BH1750_read_buffer[0];//将第一个字节的数据复制到长度为2字节的变量中
	brightness_cache = brightness_cache << 8;//将变量左移8位，空出低8位
	brightness_cache += BH1750_read_buffer[1];//将第二个字节的数据加到变量中，组合成完整的16位数据
	return (unsigned int) (brightness_cache / 1.2);
	//将数据除以1.2（官方给出的系数），再强制转换为无符号整型去除小数部分，然后返回最终结果
}
