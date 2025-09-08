#ifndef __OLED_H
#define __OLED_H

//-----------------OLED端口定义----------------

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

extern unsigned char LUX_string [];
extern unsigned char Brightness_string [];
extern uint8_t OLED_content_refresh ;

void OLED_ClearPoint(unsigned char x, unsigned char y);
void OLED_ColorTurn(unsigned char i);
void OLED_DisplayTurn(unsigned char i);
void OLED_WR_Byte(unsigned char dat, unsigned char mode);
void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
void OLED_Refresh(void);
void OLED_Clear(void);
void OLED_DrawPoint(unsigned char x, unsigned char y, unsigned char t);
void OLED_DrawLine(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char mode);
void OLED_DrawCircle(unsigned char x, unsigned char y, unsigned char r);
void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char chr, unsigned char size1, unsigned char mode);
void OLED_ShowChar6x8(unsigned char x, unsigned char y, unsigned char chr, unsigned char mode);
void OLED_ShowString(unsigned char x, unsigned char y, unsigned char* chr, unsigned char size1, unsigned char mode);
void OLED_ShowNum(unsigned char x, unsigned char y, unsigned int num, unsigned char len, unsigned char size1, unsigned char mode);
void OLED_ShowChinese(unsigned char x, unsigned char y, unsigned char num, unsigned char size1, unsigned char mode);
void OLED_Splash_Screens(int time);
void OLED_Main_interface_Pre_layout();
void OLED_Config_interface1_Pre_layout();
void OLED_Config_interface2_Pre_layout();
void OLED_ScrollDisplay(unsigned char num, unsigned char space, unsigned char mode);
void OLED_ShowPicture(unsigned char x, unsigned char y, unsigned char sizex, unsigned char sizey, unsigned char BMP [], unsigned char mode);
void OLED_Init(void);

#endif

