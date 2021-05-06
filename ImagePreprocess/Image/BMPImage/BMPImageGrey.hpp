#pragma once
#include<cstdint>
#include<vector>
#include<memory>
#include"../BMPImage.hpp"
namespace Ros{
	class BMPImageGrey : public BMPImage
	{
	public:
		virtual void setImageData(std::unique_ptr<uint8_t[]> pDataBypes)
		{
			//Because of windows' byte alignment mechanism
				//The bytes less than a multiple of 4 per line will be made up to a multiple of 4 by zero.

			int h = getHeight(), w = getWidth();
			int bypesAligned = w;
			if((w * 3) % 4 != 0)
				bypesAligned = (w * 3 / 4 + 1) * 4;
			else
				bypesAligned *= 3;
			data.resize(h * w);//Possibility of overflow.
			for(int i = 0; i < h; i++)
				for(int j = 0; j < w; j++)
					data[w * (h - i - 1) + j] = pDataBypes[bypesAligned * i + j * 3];//Possibility of overflow.
		}
		virtual void printPixels() const
		{
			uint32_t h = getHeight(), w = getWidth();
			for(int i = 0; i < h; i++)
			{
				for(int j = 0; j < w; j++)
					std::cout << '<' << (int)data[w * i + j] << ">" << '\t';
				std::cout << std::endl;
			}
		}
		virtual std::string getImageType()const { return "GREY"; }
		virtual Pixel getPixel(int x, int y)const
		{
			uint8_t g = data[getWidth() * y + x];
			return Pixel(g, g, g);
		}
	private:
		std::vector<uint8_t> data;
	};
}
