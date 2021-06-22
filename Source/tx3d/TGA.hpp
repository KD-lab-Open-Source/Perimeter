#ifndef TGA_H
#define TGA_H

#include "tweaks.h"
#include "Vector3D.hpp"

namespace tx3d {
	class TGA {
		public:
			TGA();
			~TGA();
			
			bool load(const char *fileName);
			bool loadTest(const char *fileName);

			Vector3D getPixel(int x, int y) const;

			int getBPP() const {
				return bpp;
			}

			int getWidth() const {
				return width;
			}

			int getHeight() const {
				return height;
			}

			void clean();

			static void setAssertLoad(bool enable);
		protected:
			#pragma pack(push, 1)
			struct TGAHeader {
				unsigned char idFieldLength;
				unsigned char colorMapType;
				unsigned char imageType;
				unsigned short indexFirstColorMapEntry;
				unsigned short countColorMapEntries;
				unsigned char numberOfBitsPerColorMapEntry;
				unsigned short startX;
				unsigned short startY;
				unsigned short width;
				unsigned short height;
				unsigned char bitsPerPixel;
				unsigned char flags;
			};
			#pragma pack(pop)

			unsigned char* pixels;

			int	bpp;
			int width;
			int height;
			static bool enable_load_assert;
	};
}

#endif //TGA_H