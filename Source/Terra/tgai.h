#ifndef __TGAI_H__
#define __TGAI_H__

#pragma pack (1)
struct TGAHEAD
{
	char IDLenght;		// 00h Размер поля ID-изображения // 0
	char ColorMapType;	// 01h 0-палитры нет, 1-палитра есть
	char ImageType;		// 02h Тип изображения (02h -true color , 08h-compessed)
	short CMapStart;	// 03h Номер цвета с какого начинается палитра
	short CMapLenght;	// 05h Количество элементов в палитре
	char CMapDepth;		// 07h Количество битов в элементе палитры
	short XOffset;		// 08h Начальная X-координата изображения
	short YOffset;		// 0Ah Начальная Y-координата изображения
	unsigned short Width;		// 0Ch Ширина изображения
	unsigned short Height;		// 0Eh Высота изображения
	char PixelDepth;	// 10h Количество битов в пикселе
	char ImageDescriptor;	// 11h (биты 0-3 количество битов атрибутов в пикселе)(4,5 позиция начала изображения)
							// 20h начало изображения в левом верхнем углу
	TGAHEAD(){
		init();
	};
	void init(void){
		IDLenght=0;
		ColorMapType=0;
		ImageType=2;
		CMapStart=0;CMapLenght=0;CMapDepth=0;
		XOffset=0;YOffset=0;
		Width=0;Height=0;
		PixelDepth=24;
		ImageDescriptor=0x20;
	}
	void save3layers(const char* fname,int sizeX,int sizeY,unsigned char* Ra,unsigned char* Ga,unsigned char* Ba);
	void load3layers(const char* fname,int sizeX,int sizeY,unsigned char* Ra,unsigned char* Ga,unsigned char* Ba);
	//void load216(char* fname,unsigned short *ClBuf);
	bool loadHeader(const char* fname);
	bool load2buf(unsigned char* buf);
	void load2RGBL(int sizeX,int sizeY, unsigned long* RGBLBuf);
};

#pragma pack()
#endif	// __TGAI_H__
