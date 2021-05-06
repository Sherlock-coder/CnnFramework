//#pragma once
//#include<vector>
//#include"../ImageLoad/PNGChunk.hpp"
//namespace Ros {
//	class PNGImage
//	{
//	public:
//		static const uint8_t SignatureSize = 8;
//		typedef uint8_t SignatureType[SignatureSize];
//		static const SignatureType PNGSignature;//the unique PNG file signature.
//
//		enum class ColorType
//		{
//			GREY = 0,
//			COLOR = 2,
//			OTHER
//		};
//
//	private:
//		bool isValid;//determine if the image instance is valid.
//		HeaderChunk* pHeaderChunk;
//
//	public:
//		PNGImage() :isValid(false), pHeaderChunk(nullptr)
//		{
//
//		}
//	public:
//		bool isValid() const { return isValid; }
//		void setValid() { isValid = true; }
//		void setInvaild() { isValid = false; }
//		void setHeaderChunk(HeaderChunk* p) { pHeaderChunk = p; }
//		void setHeaderChunk(PNGChunk* p) {
//			pHeaderChunk = dynamic_cast<HeaderChunk*>(p);
//			if (pHeaderChunk != nullptr)
//				pHeaderChunk->decodeDataBypes();
//		}
//		int getWidth() const { return (pHeaderChunk->data).width; }
//		int getHeigth() const { return (pHeaderChunk->data).height; }
//		int getBitDepth() const { return (pHeaderChunk->data).bitDepth; }
//		ColorType getColorType{
//			if ((pHeaderChunk->data).colorType == 0) { return ColorType::GREY; }
//			else if ((pHeaderChunk->data).colorType == 2) { return ColorType::COLOR; }
//			else { return ColorType::OTHER; }
//		}
//	};
//	const PNGImage::SignatureType PNGSignature[8]{ 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a };
//}
