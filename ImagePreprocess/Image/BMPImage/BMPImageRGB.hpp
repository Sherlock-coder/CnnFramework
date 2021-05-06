#pragma once
#include<cstdint>
#include<iostream>
#include<vector>
#include"../BMPImage.hpp"
namespace Ros{
	class BMPImageRGB : public BMPImage
	{
	public:
		virtual void setImageData(std::unique_ptr<uint8_t[]> pDataBypes)
		{
			//Because of windows' byte alignment mechanism
			//The bytes less than a multiple of 4 per line will be made up to a multiple of 4 by zero.

			int h = getHeight(), w = getWidth();
			int bypesAligned = w;
			if(w % 4 != 0)
				bypesAligned = (w * 3 / 4 + 1) * 4;
			else
				bypesAligned *= 3;
			data.resize(h * w);//Possibility of overflow.
			for(int i = 0; i < h; i++)
			{
				for(int j = 0; j < w; j++)
				{
					Pixel t;
					t.r = pDataBypes[bypesAligned * i + j * 3 + 2];
					t.g = pDataBypes[bypesAligned * i + j * 3 + 1];
					t.b = pDataBypes[bypesAligned * i + j * 3 + 0];
					//It's a little-endien in window.
					data[w * (h - i - 1) + j] = t;
				}
			}
		}
		virtual void printPixels() const
		{
			uint32_t h = getHeight(), w = getWidth();
			for(int i = 0; i < h; i++)
			{
				for(int j = 0; j < w; j++)
					std::cout << '<' << (int)data[w * i + j].r << ' ' << (int)data[w * i + j].g << ' ' << (int)data[w * i + j].b << ">\t";
				std::cout << std::endl;
			}
		}
		virtual std::string getImageType()const { return "RGB"; }
		virtual Pixel getPixel(int x, int y)const { return data[getWidth() * y + x]; }
	private:
		std::vector<Pixel> data;
	};
}

