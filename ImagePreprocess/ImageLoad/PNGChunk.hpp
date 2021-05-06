//#pragma once
//#include<vector>
//namespace Ros
//{
//	constexpr uint32_t convert4BypesTo32Int(const char* const str) {
//		return (str[0] << 24) + (str[1] << 16) + (str[2] << 8) + str[3];
//	}
//	struct PNGChunk
//	{
//	public:
//		enum class chunkType : uint32_t
//		{
//			IHDR = convert4BypesTo32Int("IHDR"),
//			PLTE = convert4BypesTo32Int("PLTE"),
//			IDAT = convert4BypesTo32Int("IDAT"),
//			IEND = convert4BypesTo32Int("IEND"),
//			tRNS = convert4BypesTo32Int("tRNS"),
//			UNDEFINED
//		};
//		const static uint8_t lengthSize = 4;
//		const static uint8_t chunkTypeCodeSize = 4;
//		//the size of data is alterable.
//		const static uint8_t CRCSize = 4;
//	public:
//		uint32_t length;
//		chunkType chunkTypeCode;
//		uint8_t* pData;
//		uint32_t CRC;
//	public:
//		virtual void decodeDataBypes() {
//			//It won't do anything until it has been determined types.
//		}
//	};
//	struct HeaderChunk : public PNGChunk
//	{
//	public:
//		struct HeaderChunkData
//		{
//			uint32_t width;
//			uint32_t height;
//			uint8_t bitDepth;
//			uint8_t colorType;
//			uint8_t compressionMethod;
//			uint8_t filterMethod;
//			uint8_t interlaceMethod;
//		}data;
//	public:
//		virtual void decodeDataBypes()
//		{
//			data.width = convert4BypesTo32Int((char*)pData);
//			data.height = convert4BypesTo32Int((char*)(pData + 4));
//			data.bitDepth = *(pData + 8);
//			data.colorType = *(pData + 9);
//			data.compressionMethod = *(pData + 10);
//			data.filterMethod = *(pData + 11);
//			data.interlaceMethod = *(pData + 12);
//			//It may can be done througn memcpy()
//			//But I think it would be uncontrollable.
//		}
//	};
//}
