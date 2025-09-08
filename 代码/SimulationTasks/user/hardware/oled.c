#include "main.h"
#include "i2c.h"
#include "oled.h"
#include "oledfont.h"

unsigned char OLED_GRAM[4][144];

unsigned char OLED_send_buffer[256];

//反显函数
void OLED_ColorTurn(unsigned char i) {
	if (i == 0)
		OLED_WR_Byte(0xA6, OLED_CMD);//正常显示
	if (i == 1)
		OLED_WR_Byte(0xA7, OLED_CMD);//反色显示
}

//屏幕旋转180度
void OLED_DisplayTurn(unsigned char i) {
	if (i == 0) {
		OLED_WR_Byte(0xC8, OLED_CMD);//正常显示
		OLED_WR_Byte(0xA1, OLED_CMD);
	}
	if (i == 1) {
		OLED_WR_Byte(0xC0, OLED_CMD);//反转显示
		OLED_WR_Byte(0xA0, OLED_CMD);
	}
}

//发送一个字节
//mode:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(unsigned char dat, unsigned char mode) {
	// OLED_send_buffer[0] = 0x78;
	if (mode)
		OLED_send_buffer[0] = 0x40;
	else
		OLED_send_buffer[0] = 0x00;
	OLED_send_buffer[1] = dat;
	OLED_TX(OLED_send_buffer, 2);
}

//开启OLED显示
void OLED_DisPlay_On(void) {
	OLED_WR_Byte(0x8D, OLED_CMD);//电荷泵使能
	OLED_WR_Byte(0x14, OLED_CMD);//开启电荷泵
	OLED_WR_Byte(0xAF, OLED_CMD);//点亮屏幕
}

//关闭OLED显示
void OLED_DisPlay_Off(void) {
	OLED_WR_Byte(0x8D, OLED_CMD);//电荷泵使能
	OLED_WR_Byte(0x10, OLED_CMD);//关闭电荷泵
	OLED_WR_Byte(0xAE, OLED_CMD);//关闭屏幕
}

//更新显存到OLED
void OLED_Refresh(void) {
	unsigned char i;
	for (i = 0;i < 4;i++) {
		OLED_WR_Byte(0xb0 + i, OLED_CMD); //设置行起始地址
		OLED_WR_Byte(0x00, OLED_CMD);   //设置低列起始地址
		OLED_WR_Byte(0x10, OLED_CMD);   //设置高列起始地址
		// OLED_WR_Byte(0x40, OLED_CMD);   //设置高列起始地址
		OLED_GRAM[i][0] = 0x40;
		HAL_I2C_Master_Transmit(&hi2c1, 0x78, &(OLED_GRAM[i][0]), 128, 100);
	}
	OLED_content_refresh = 0;
}
//清屏函数
void OLED_Clear(void) {
	unsigned char i, n;
	for (i = 0;i < 4;i++) {
		for (n = 0;n < 128;n++) {
			OLED_GRAM[i][n] = 0;//清除所有数据
		}
	}
	OLED_Refresh();//更新显示
}

//画点
//x:0~127
//y:0~63
//t:1 填充 0,清空
void OLED_DrawPoint(unsigned char x, unsigned char y, unsigned char t) {
	unsigned char i, m, n;
	i = y / 8;
	m = y % 8;
	n = 1 << m;
	if (t) { OLED_GRAM[i][x + 1] |= n; }
	else {
		OLED_GRAM[i][x + 1] = ~OLED_GRAM[i][x + 1];
		OLED_GRAM[i][x + 1] |= n;
		OLED_GRAM[i][x + 1] = ~OLED_GRAM[i][x + 1];
	}
}

//画线
//x1,y1:起点坐标
//x2,y2:结束坐标
void OLED_DrawLine(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char mode) {
	unsigned short t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;
	delta_x = x2 - x1; //计算坐标增量
	delta_y = y2 - y1;
	uRow = x1;//画线起点坐标
	uCol = y1;
	if (delta_x > 0)incx = 1; //设置单步方向
	else if (delta_x == 0)incx = 0;//垂直线
	else { incx = -1;delta_x = -delta_x; }
	if (delta_y > 0)incy = 1;
	else if (delta_y == 0)incy = 0;//水平线
	else { incy = -1;delta_y = -delta_x; }
	if (delta_x > delta_y)distance = delta_x; //选取基本增量坐标轴
	else distance = delta_y;
	for (t = 0;t < distance + 1;t++) {
		OLED_DrawPoint(uRow, uCol, mode);//画点
		xerr += delta_x;
		yerr += delta_y;
		if (xerr > distance) {
			xerr -= distance;
			uRow += incx;
		}
		if (yerr > distance) {
			yerr -= distance;
			uCol += incy;
		}
	}
}
//x,y:圆心坐标
//r:圆的半径
void OLED_DrawCircle(unsigned char x, unsigned char y, unsigned char r) {
	int a, b, num;
	a = 0;
	b = r;
	while (2 * b * b >= r * r) {
		OLED_DrawPoint(x + a, y - b, 1);
		OLED_DrawPoint(x - a, y - b, 1);
		OLED_DrawPoint(x - a, y + b, 1);
		OLED_DrawPoint(x + a, y + b, 1);

		OLED_DrawPoint(x + b, y + a, 1);
		OLED_DrawPoint(x + b, y - a, 1);
		OLED_DrawPoint(x - b, y - a, 1);
		OLED_DrawPoint(x - b, y + a, 1);

		a++;
		num = (a * a + b * b) - r * r;//计算画的点离圆心的距离
		if (num > 0) {
			b--;
			a--;
		}
	}
}



//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//size1:选择字体 6x8/6x12/8x16/12x24
//mode:0,反色显示;1,正常显示
void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char chr, unsigned char size1, unsigned char mode) {
	OLED_content_refresh = 1;
	unsigned char i, m, temp, size2, chr1;
	unsigned char x0 = x, y0 = y;
	if (size1 == 8)size2 = 6;
	else size2 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) * (size1 / 2);  //得到字体一个字符对应点阵集所占的字节数
	chr1 = chr - ' ';  //计算偏移后的值
	for (i = 0;i < size2;i++) {
		if (size1 == 8) {
			temp = asc2_0806[chr1][i];
		} //调用0806字体
		else if (size1 == 12) {
			temp = asc2_1206[chr1][i];
		} //调用1206字体
		else if (size1 == 16) {
			temp = asc2_1608[chr1][i];
		} //调用1608字体
		else if (size1 == 24) {
			temp = asc2_2412[chr1][i];
		} //调用2412字体
		else return;
		for (m = 0;m < 8;m++) {
			if (temp & 0x01)OLED_DrawPoint(x, y, mode);
			else OLED_DrawPoint(x, y, !mode);
			temp >>= 1;
			y++;
		}
		x++;
		if ((size1 != 8) && ((x - x0) == size1 / 2)) {
			x = x0;y0 = y0 + 8;
		}
		y = y0;
	}
}


//显示字符串
//x,y:起点坐标
//size1:字体大小
//*chr:字符串起始地址
//mode:0,反色显示;1,正常显示
void OLED_ShowString(unsigned char x, unsigned char y, unsigned char* chr, unsigned char size1, unsigned char mode) {
	while ((*chr >= ' ') && (*chr <= '~'))//判断是不是非法字符!
	{
		OLED_ShowChar(x, y, *chr, size1, mode);
		if (size1 == 8)x += 6;
		else x += size1 / 2;
		chr++;
	}
}

//m^n
unsigned int OLED_Pow(unsigned char m, unsigned char n) {
	unsigned int result = 1;
	while (n--) {
		result *= m;
	}
	return result;
}

//显示数字
//x,y :起点坐标
//len :数字的位数
//size:字体大小
//mode:0,反色显示;1,正常显示
void OLED_ShowNum(unsigned char x, unsigned char y, unsigned int num, unsigned char len, unsigned char size1, unsigned char mode) {
	unsigned char t, temp, m = 0;
	if (size1 == 8)m = 2;
	for (t = 0;t < len;t++) {
		temp = (num / OLED_Pow(10, len - t - 1)) % 10;
		if (temp == 0) {
			OLED_ShowChar(x + (size1 / 2 + m) * t, y, '0', size1, mode);
		}
		else {
			OLED_ShowChar(x + (size1 / 2 + m) * t, y, temp + '0', size1, mode);
		}
	}
}

//显示汉字
//x,y:起点坐标
//num:汉字对应的序号
//mode:0,反色显示;1,正常显示
void OLED_ShowChinese(unsigned char x, unsigned char y, unsigned char num, unsigned char size1, unsigned char mode) {
	OLED_content_refresh = 1;
	unsigned char m, temp;
	unsigned char x0 = x, y0 = y;
	unsigned short i, size3 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) * size1;  //得到字体一个字符对应点阵集所占的字节数
	for (i = 0;i < size3;i++) {
		if (size1 == 16) {
			temp = HZk5[num][i];
		}//调用16*16字体
		else if (size1 == 24) {
			temp = Hzk2[num][i];
		}//调用24*24字体
		else if (size1 == 32) {
			temp = Hzk3[num][i];
		}//调用32*32字体
		else if (size1 == 64) {
			temp = Hzk4[num][i];
		}//调用64*64字体
		else return;
		for (m = 0;m < 8;m++) {
			if (temp & 0x01)OLED_DrawPoint(x, y, mode);
			else OLED_DrawPoint(x, y, !mode);
			temp >>= 1;
			y++;
		}
		x++;
		if ((x - x0) == size1) {
			x = x0;y0 = y0 + 8;
		}
		y = y0;
	}
}

void OLED_Splash_Screens(int time) {

	for (char i = 0; i < 8; i++)
		OLED_ShowChinese(16 * i, 0, 11 + i, 16, 1);
	for (char i = 0; i < 3; i++)
		OLED_ShowChinese(16 * i, 16, 8 + i, 16, 1);
	OLED_Refresh();

	HAL_Delay(time);

	OLED_ShowString(0, 16, "45152104041", 16, 1);

	OLED_Refresh();

	HAL_Delay(time);

}
void OLED_Config_interface1_Pre_layout() {
	OLED_Clear();
	OLED_ShowChinese(0, 0, 20, 16, 0);
	OLED_ShowChinese(16, 0, 21, 16, 0);
	OLED_ShowChinese(32, 0, 0, 16, 1);
	OLED_ShowChinese(48, 0, 1, 16, 1);
	OLED_ShowChinese(64, 0, 2, 16, 1);
	OLED_ShowChinese(80, 0, 3, 16, 1);
	OLED_ShowChinese(0, 16, 4, 16, 1);
	OLED_ShowString(16, 16, "00000", 16, 1);
	OLED_ShowChinese(72, 16, 6, 16, 1);
	OLED_ShowString(16 + 72, 16, "00000", 16, 1);
	OLED_Refresh();
}

void OLED_Config_interface2_Pre_layout() {
	OLED_Clear();
	OLED_ShowChinese(0, 0, 4, 16, 1);
	OLED_ShowChinese(72, 0, 6, 16, 1);
	OLED_ShowChar(16, 0, '0', 16, 0);
	OLED_ShowChar(24, 0, '0', 16, 1);
	OLED_ShowChar(32, 0, '0', 16, 1);
	OLED_ShowChar(40, 0, '0', 16, 1);
	OLED_ShowChar(48, 0, '0', 16, 1);
	OLED_ShowChar(88, 0, '0', 16, 1);
	OLED_ShowChar(96, 0, '0', 16, 1);
	OLED_ShowChar(104, 0, '0', 16, 1);
	OLED_ShowChar(112, 0, '0', 16, 1);
	OLED_ShowChar(120, 0, '0', 16, 1);
	OLED_ShowChinese(80, 16, 22, 16, 1);
	OLED_ShowChinese(96, 16, 23, 16, 1);
	OLED_ShowChinese(16, 16, 20, 16, 1);
	OLED_ShowChinese(32, 16, 21, 16, 1);

	OLED_Refresh();
}

void OLED_Main_interface_Pre_layout() {
	OLED_Clear();
	OLED_ShowChinese(0, 0, 0, 16, 1);
	OLED_ShowChinese(16, 0, 1, 16, 1);
	OLED_ShowChinese(32, 0, 6, 16, 1);
	OLED_ShowChinese(48, 0, 7, 16, 1);
	OLED_ShowChinese(0, 16, 2, 16, 1);
	OLED_ShowChinese(16, 16, 3, 16, 1);
	OLED_ShowChinese(32, 16, 24, 16, 1);
	OLED_ShowChinese(48, 16, 25, 16, 1);
	OLED_ShowString(80, 0, LUX_string, 16, 1);
	OLED_Refresh();
}

//x,y：起点坐标
//sizex,sizey,图片长宽
//BMP[]：要写入的图片数组
//mode:0,反色显示;1,正常显示
void OLED_ShowPicture(unsigned char x, unsigned char y, unsigned char sizex, unsigned char sizey, unsigned char BMP [], unsigned char mode) {
	unsigned short j = 0;
	unsigned char i, n, temp, m;
	unsigned char x0 = x, y0 = y;
	sizey = sizey / 8 + ((sizey % 8) ? 1 : 0);
	for (n = 0;n < sizey;n++) {
		for (i = 0;i < sizex;i++) {
			temp = BMP[j];
			j++;
			for (m = 0;m < 8;m++) {
				if (temp & 0x01)OLED_DrawPoint(x, y, mode);
				else OLED_DrawPoint(x, y, !mode);
				temp >>= 1;
				y++;
			}
			x++;
			if ((x - x0) == sizex) {
				x = x0;
				y0 = y0 + 8;
			}
			y = y0;
		}
	}
}
//OLED的初始化
void OLED_Init(void) {
	// for (char i = 0; i < 4; i++)
	// 	for (int j = 0; j < 144; j++)
	// 		OLED_GRAM[i][j] = 0x00;

	OLED_WR_Byte(0xAE, OLED_CMD); /*display off*/
	OLED_WR_Byte(0x00, OLED_CMD); /*set lower column address*/
	OLED_WR_Byte(0x10, OLED_CMD); /*set higher column address*/
	OLED_WR_Byte(0x00, OLED_CMD); /*set display start line*/
	OLED_WR_Byte(0xB0, OLED_CMD); /*set page address*/
	OLED_WR_Byte(0x81, OLED_CMD); /*contract control*/
	OLED_WR_Byte(0xff, OLED_CMD); /*128*/
	OLED_WR_Byte(0xA1, OLED_CMD); /*set segment remap*/
	OLED_WR_Byte(0xA6, OLED_CMD); /*normal / reverse*/
	OLED_WR_Byte(0xA8, OLED_CMD); /*multiplex ratio*/
	OLED_WR_Byte(0x1F, OLED_CMD); /*duty = 1/32*/
	OLED_WR_Byte(0xC8, OLED_CMD); /*Com scan direction*/
	OLED_WR_Byte(0xD3, OLED_CMD); /*set display offset*/
	OLED_WR_Byte(0x00, OLED_CMD);
	OLED_WR_Byte(0xD5, OLED_CMD); /*set osc division*/
	OLED_WR_Byte(0x80, OLED_CMD);
	OLED_WR_Byte(0xD9, OLED_CMD); /*set pre-charge period*/
	OLED_WR_Byte(0x1f, OLED_CMD);
	OLED_WR_Byte(0xDA, OLED_CMD); /*set COM pins*/
	OLED_WR_Byte(0x00, OLED_CMD);
	OLED_WR_Byte(0xdb, OLED_CMD); /*set vcomh*/
	OLED_WR_Byte(0x40, OLED_CMD);
	OLED_WR_Byte(0x8d, OLED_CMD); /*set charge pump enable*/
	OLED_WR_Byte(0x14, OLED_CMD);
	OLED_Clear();
	OLED_WR_Byte(0xAF, OLED_CMD); /*display ON*/
}

