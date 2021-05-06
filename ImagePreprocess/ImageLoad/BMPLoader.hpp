#pragma once
#include<fstream>
#include<iostream>
#include<cstdint>
#include<memory>
#include<vector>
#include<io.h>
#include"../Image/BMPImage.hpp"
#include"../Image/BMPImage/BMPImageGrey.hpp"
#include"../Image/BMPImage/BMPImageRGB.hpp"
namespace Ros {
	class BMPLoader
	{
	public:
		enum class LoadMode//How to load a Image
		{
			RGB,	//Load full RGB image
			GREY,	//Load RNG image as grey image.
			//Because the original data is gray, we can only took the first bypes of each pixel to get the grey image.
			DEFAULT,//GREY
		};
	private:
		uint32_t offsetBits, size;
		uint8_t	width, height;
		Ros::BMPImage::BMPFileHeader getFileHeader(std::ifstream& fin)
		{
			Ros::BMPImage::BMPFileHeader t;
			fin.read((char*)(&t), sizeof(Ros::BMPImage::BMPFileHeader));
			offsetBits = t.offsetBits;
			size = t.size;
			return t;
		}
		Ros::BMPImage::BMPInfoHeader getInfoHeader(std::ifstream& fin)
		{
			Ros::BMPImage::BMPInfoHeader t;
			fin.read((char*)(&t), sizeof(Ros::BMPImage::BMPInfoHeader));
			width = t.width, height = t.height;
			return t;
		}
		Ros::BMPImage::BMPPixelInfo getPixelInfo(std::ifstream& fin)
		{
			Ros::BMPImage::BMPPixelInfo t;
			fin.read((char*)(&t), sizeof(Ros::BMPImage::BMPPixelInfo));
			return t;
		}
		std::unique_ptr<uint8_t[]> getImageData(std::ifstream& fin)
		{
			fin.seekg(offsetBits, std::ios::beg);
			std::unique_ptr<uint8_t[]> pData(new uint8_t[size - offsetBits]);
			fin.read((char*)(pData.get()), size - offsetBits);
			return pData;
		}
		Ros::BMPImage& loadFile(std::ifstream& fin, LoadMode loadMode)
		{
			BMPImage* pImage = nullptr;
			switch (loadMode)
			{
			case LoadMode::RGB:
				pImage = new Ros::BMPImageRGB();
				break;
			case LoadMode::GREY:
			case LoadMode::DEFAULT:
				pImage = new Ros::BMPImageGrey();
				break;
			}
			if (!fin.is_open())
			{
				std::cerr << "Can't open the file.";
				return (*pImage);
			}
			//possible machine dependency, because of bype allign.
			uint16_t tag;
			fin.read((char*)(&tag), sizeof(uint16_t));
			if (tag != Ros::BMPImage::TAG)
			{
				std::cerr << "Not a BMP file.";
				return (*pImage);
			}
			//pImage->setFileHeader(getFileHeader(fin));
			//pImage->setInfoHeader(getInfoHeader(fin));
			//image.setPixelInfo(getPixelInfo(fin));
			offsetBits = getFileHeader(fin).offsetBits;
			auto t = getInfoHeader(fin);
			pImage->setWidth(t.width), pImage->setHeight(t.height);
			pImage->setImageData(getImageData(fin));
			pImage->setValidity(true);
			return (*pImage);
		}
		std::vector<std::string> findAllBMPFilesPath(std::string filesPath)
		{
			std::vector<std::string> paths;
			intptr_t handle;												//用于查找的句柄
			struct _finddata_t fileInfo;									//文件信息的结构体
			handle = _findfirst((filesPath + "/*.bmp").c_str(), &fileInfo);	//第一次查找
			if (-1 == handle)
				return paths;
			do
			{
				paths.push_back(filesPath + "/" + fileInfo.name);
			} while (_findnext(handle, &fileInfo) == 0);					//循环查找
			_findclose(handle);												//关闭句柄
			return paths;
		}
	public:
		std::vector<Ros::BMPImage*> loadBatchFile(std::string filesPath//May have to be full path
			, LoadMode loadMode = LoadMode::DEFAULT)
		{
			std::vector<Ros::BMPImage*> r;
			std::vector<std::string> paths(findAllBMPFilesPath(filesPath));
			int cnt = 0;
			for (auto path : paths)
			{
				std::ifstream fin;
				fin.open(path, std::ios::binary);
				r.push_back(&loadFile(fin, loadMode));
				fin.close();
				cnt++;
				if (cnt % 300 == 0)
					std::cout << "已经读取文件数：" << cnt << std::endl;
			}
			std::cout << "总共读取文件数：" << cnt << std::endl;
			return r;
		}
		Ros::BMPImage& loadSingleFile(std::string filePath, //May have to be full path
			LoadMode loadMode = LoadMode::DEFAULT)
		{
			std::ifstream fin(filePath, std::ios::binary);
			Ros::BMPImage& r = loadFile(fin, loadMode);
			fin.close();
			return r;
		}
	};
}