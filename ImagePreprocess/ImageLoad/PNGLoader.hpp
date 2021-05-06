//#pragma once
//#include<string>
//#include<fstream>
//#include<iostream>
//#include<cstring>
//#include"../Image/PNGImage.hpp"
//#include"PNGChunk.hpp"
//namespace Ros {
//	enum class LoadMode
//	{
//		DEFAULT,
//	};
//	class PNGLoader
//	{
//	private:
//		Ros::PNGChunk* readChunk(std::ifstream& fin)
//		{
//			Ros::PNGChunk* pChunk = new Ros::PNGChunk();
//			uint8_t* bypes = new uint8_t[4];//Sorry I just know it would always be 4 bypes wide.
//			fin.read((char*)bypes, Ros::PNGChunk::lengthSize);
//			pChunk->length = convert4BypesTo32Int((char*)bypes);
//			fin.read((char*)bypes, Ros::PNGChunk::chunkTypeCodeSize);
//			pChunk->chunkTypeCode = Ros::PNGChunk::chunkType(convert4BypesTo32Int((char*)bypes));
//			pChunk->pData = new uint8_t[pChunk->length];
//			fin.read((char*)pChunk->pData, pChunk->length);
//			fin.read((char*)bypes, Ros::PNGChunk::CRCSize);
//			return pChunk;
//		}
//	public:
//		Ros::PNGImage loadFile(std::string filePath, LoadMode loadMode = LoadMode::DEFAULT)
//		{
//			std::ifstream fin;
//			Ros::PNGImage image;
//			fin.open(filePath, std::ios::binary);
//			if (!fin.is_open())
//			{
//				std::cerr << "Can not open the file.";
//				return image;
//			}
//			//Read the file signature and judge if it's a PNG file.
//			Ros::PNGImage::SignatureType signature;
//			fin.read((char*)signature, Ros::PNGImage::SignatureSize);
//			if (strcmp((char*)Ros::PNGImage::PNGSignature, (char*)signature) != 0)
//			{
//				std::cerr << "This is not a PNG file.";
//				return image;
//			}
//			//Read the IHDR and get the necessary image info.
//			image.setHeaderChunk(readChunk(fin));
//			//Read the chunks until the IEND chunk.
//			while (true)
//			{
//				Ros::PNGChunk* pChunk = readChunk(fin);
//				if (pChunk->chunkTypeCode == PNGChunk::chunkType::IEND)
//					break;
//				if (pChunk->chunkTypeCode == PNGChunk::chunkType::IDAT)
//				{
//				}
//			};
//			fin.close();
//			return image;
//		}
//	};
//}