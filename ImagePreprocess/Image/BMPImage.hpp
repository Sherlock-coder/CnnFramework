#pragma once
#include<cstdint>
#include<iostream>
namespace Ros {
	class BMPImage
	{
	public:
		struct BMPFileHeader//14
		{
			uint32_t size;			// 文件大小 以字节为单位(2-5字节)
			uint16_t reservered1;	// 保留，必须设置为0 (6-7字节)
			uint16_t reservered2;	// 保留，必须设置为0 (8-9字节)
			uint32_t offsetBits;	// 从文件头到像素数据的偏移  (10-13字节)
		};
		struct BMPInfoHeader//40
		{
			uint32_t size;          // 此结构体的大小 (14-17字节)
			uint32_t width;         // 图像的宽  (18-21字节)
			uint32_t height;        // 图像的高  (22-25字节)
			uint16_t planes;        // 表示bmp图片的平面属，显然显示器只有一个平面，所以恒等于1 (26-27字节)
			uint16_t bitCount;      // 一像素所占的位数，一般为24   (28-29字节)
			uint32_t compression;   // 说明图象数据压缩的类型，0为不压缩。 (30-33字节)
			uint32_t sizeImage;     // 像素数据所占大小, 这个值应该等于上面文件头结构中bfSize-bfOffBits (34-37字节)
			uint32_t xPelsPerMeter; // 说明水平分辨率，用象素/米表示。一般为0 (38-41字节)
			uint32_t yPelsPerMeter; // 说明垂直分辨率，用象素/米表示。一般为0 (42-45字节)
			uint32_t clrUsed;       // 说明位图实际使用的彩色表中的颜色索引数（设为0的话，则说明使用所有调色板项）。 (46-49字节)
			uint32_t clrImportant;  // 说明对图象显示有重要影响的颜色索引的数目，如果是0，表示都重要。(50-53字节)
		};
		struct BMPPixelInfo//4
		{
			uint8_t rgbBlue;	//该颜色的蓝色分量  (值范围为0-255)
			uint8_t rgbGreen;	//该颜色的绿色分量  (值范围为0-255)
			uint8_t rgbRed;		//该颜色的红色分量  (值范围为0-255)
			uint8_t rgbReserved;// 保留，必须为0
		};
		struct Pixel
		{
			uint8_t r;
			uint8_t g;
			uint8_t b;
		public:
			Pixel() = default;
			Pixel(uint8_t red, uint8_t green, uint8_t blue) : r(red), g(green), b(blue) {}
		};

		static const uint16_t TAG = 19778;// 必须是BM字符串，对应的十六进制为0x4d42,十进制为19778，否则不是bmp格式文件
	private:
		bool isValid;			//determine if the image instance is valid.
		uint32_t width, height;

		//BMPFileHeader fileHeader;
		//BMPInfoHeader infoHeader;
		//BMPPixelInfo pixelInfo;
	public:
		BMPImage() :isValid(false), width(0), height(0) {}
	public:
		bool getValidity() const { return isValid; }
		void setValidity(bool v) { isValid = v; }
		//void setFileHeader(const BMPFileHeader& t) { fileHeader = t; }
		//void setInfoHeader(const BMPInfoHeader& t) { infoHeader = t; }
		//void setPixelInfo(const BMPPixelInfo& t) { pixelInfo = t; }
		void setWidth(uint32_t w) { width = w; }
		int getWidth() const {
			//return infoHeader.width;
			return width;
		}
		void setHeight(uint32_t h) { height = h; }
		int getHeight() const {
			//return infoHeader.height;
			return height;
		}
		int getSize() const { return getWidth() * getHeight(); }
	public:
		virtual void setImageData(std::unique_ptr<uint8_t[]> pDataBypes) = 0;
		//Directly input the bypes is a efficient and for-implement-convenience consideration.
		virtual std::string getImageType()const = 0;
		//return "GREY" or "RGB"
		virtual void printPixels() const = 0;
		virtual Pixel getPixel(int x, int y) const = 0;
		//O X➡
		//Y
		//⬇
		//virtual void toMatrix() = 0;
		//Transformed to Math Matrix
		//To be done.
	};
}