// vtfTrans.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
//#include <ctype.h>

enum TextureFlags
{
	// flags from the *.txt config file
	TEXTUREFLAGS_POINTSAMPLE = 0x00000001,
	TEXTUREFLAGS_TRILINEAR = 0x00000002,
	TEXTUREFLAGS_CLAMPS = 0x00000004,
	TEXTUREFLAGS_CLAMPT = 0x00000008,
	TEXTUREFLAGS_ANISOTROPIC = 0x00000010,
	TEXTUREFLAGS_HINT_DXT5 = 0x00000020,
	TEXTUREFLAGS_NOCOMPRESS = 0x00000040,
	TEXTUREFLAGS_NORMAL = 0x00000080,
	TEXTUREFLAGS_NOMIP = 0x00000100,
	TEXTUREFLAGS_NOLOD = 0x00000200,
	TEXTUREFLAGS_MINMIP = 0x00000400,
	TEXTUREFLAGS_PROCEDURAL = 0x00000800,

	// These are automatically generated by vtex from the texture data.
	TEXTUREFLAGS_ONEBITALPHA = 0x00001000,
	TEXTUREFLAGS_EIGHTBITALPHA = 0x00002000,

	// newer flags from the *.txt config file
	TEXTUREFLAGS_ENVMAP = 0x00004000,
	TEXTUREFLAGS_RENDERTARGET = 0x00008000,
	TEXTUREFLAGS_DEPTHRENDERTARGET = 0x00010000,
	TEXTUREFLAGS_NODEBUGOVERRIDE = 0x00020000,
	TEXTUREFLAGS_SINGLECOPY = 0x00040000,
	TEXTUREFLAGS_ONEOVERMIPLEVELINALPHA = 0x00080000,
	TEXTUREFLAGS_PREMULTCOLORBYONEOVERMIPLEVEL = 0x00100000,
	TEXTUREFLAGS_NORMALTODUDV = 0x00200000,
	TEXTUREFLAGS_ALPHATESTMIPGENERATION = 0x00400000,

	TEXTUREFLAGS_NODEPTHBUFFER = 0x00800000,

	TEXTUREFLAGS_NICEFILTERED = 0x01000000,

	TEXTUREFLAGS_CLAMPU = 0x02000000,

	// xbox extensions
	TEXTUREFLAGS_PRESWIZZLED = 0x04000000,
	TEXTUREFLAGS_CACHEABLE = 0x08000000,
	TEXTUREFLAGS_UNFILTERABLE_OK = 0x10000000,

	TEXTUREFLAGS_LASTFLAG = 0x10000000,
};

enum ImageFormat
{
	IMAGE_FORMAT_UNKNOWN = -1,
	IMAGE_FORMAT_RGBA8888 = 0,
	IMAGE_FORMAT_ABGR8888,
	IMAGE_FORMAT_RGB888,
	IMAGE_FORMAT_BGR888,
	IMAGE_FORMAT_RGB565,
	IMAGE_FORMAT_I8,
	IMAGE_FORMAT_IA88,
	IMAGE_FORMAT_P8,
	IMAGE_FORMAT_A8,
	IMAGE_FORMAT_RGB888_BLUESCREEN,
	IMAGE_FORMAT_BGR888_BLUESCREEN,
	IMAGE_FORMAT_ARGB8888,
	IMAGE_FORMAT_BGRA8888,
	IMAGE_FORMAT_DXT1,
	IMAGE_FORMAT_DXT3,
	IMAGE_FORMAT_DXT5,
	IMAGE_FORMAT_BGRX8888,
	IMAGE_FORMAT_BGR565,
	IMAGE_FORMAT_BGRX5551,
	IMAGE_FORMAT_BGRA4444,
	IMAGE_FORMAT_DXT1_ONEBITALPHA,
	IMAGE_FORMAT_BGRA5551,
	IMAGE_FORMAT_UV88,
	IMAGE_FORMAT_UVWQ8888,
	IMAGE_FORMAT_RGBA16161616F,
	// GR - HDR
	IMAGE_FORMAT_RGBA16161616,
	IMAGE_FORMAT_UVLX8888,
	IMAGE_FORMAT_R32F,	// Single-channel 32-bit floating point
	IMAGE_FORMAT_RGB323232F,
	IMAGE_FORMAT_RGBA32323232F,
	IMAGE_FORMAT_LINEAR_BGRX8888,
	IMAGE_FORMAT_LINEAR_RGBA8888,
	IMAGE_FORMAT_LINEAR_ABGR8888,
	IMAGE_FORMAT_LINEAR_ARGB8888,
	IMAGE_FORMAT_LINEAR_BGRA8888,
	IMAGE_FORMAT_LINEAR_RGB888,
	IMAGE_FORMAT_LINEAR_BGR888,
	IMAGE_FORMAT_LINEAR_BGRX5551,
	IMAGE_FORMAT_LINEAR_I8,
	NUM_IMAGE_FORMATS
};
int ImageFormatBlock[39] = {4,4,3,3,2,1,2,1,1,3,3,4,4,8,16,16,4,2,2,2,8,2,2,4,8,8,4,4,12,16,4,4,4,4,4,3,3,2,1};



struct Vector
{
	float x, y, z;
};

#ifdef _WIN32
struct __declspec(align(16))VectorAligned : public Vector 
#elif _LINUX
struct __attribute__((aligned(16))) VectorAligned : public Vector
#endif
{
	VectorAligned(float x = 1.0f, float y = 1.0f, float z = 1.0f) { Vector(); }
};

#pragma pack(1)

// version number for the disk texture cache
#define VTF_MAJOR_VERSION 7
#define VTF_MINOR_VERSION 2

struct VTFFileBaseHeader_t
{
	char fileTypeString[4]; // "VTF" Valve texture file
	int version[2]; 		// version[0].version[1]
	int headerSize;
};

struct VTFFileHeaderV7_1_t : public VTFFileBaseHeader_t
{
	unsigned short	width;
	unsigned short	height;
	unsigned int	flags;
	unsigned short	numFrames;
	unsigned short	startFrame;
	VectorAligned	reflectivity; // This is a linear value, right?  Average of all frames?
	float			bumpScale;
	ImageFormat		imageFormat;
	unsigned char	numMipLevels;
	ImageFormat		lowResImageFormat;
	unsigned char	lowResImageWidth;
	unsigned char	lowResImageHeight;
};

struct VTFFileHeader_t : public VTFFileHeaderV7_1_t
{

	unsigned short depth;
};
VTFFileHeader_t VTFFileHeader_Default() {
	VTFFileHeader_t VTF = VTFFileHeader_t();
	strcpy_s(VTF.fileTypeString, (char*)"VTF");
	int version[2] = { VTF_MAJOR_VERSION ,VTF_MINOR_VERSION };
	memcpy(VTF.version, version, sizeof(version));
	VTF.headerSize = sizeof(VTFFileHeader_t);
	VectorAligned reflectivity = VectorAligned();
	reflectivity.x = 1.0f;
	reflectivity.y = 1.0f;
	reflectivity.z = 1.0f;
	memcpy(&VTF.reflectivity, &reflectivity, sizeof(reflectivity));
	VTF.bumpScale = 1.0f;
	VTF.lowResImageHeight = 8;
	VTF.lowResImageWidth = 8;
	VTF.numFrames = 1;
	VTF.numMipLevels = 1;
	VTF.depth = 1;

	return VTF;

};



#define XTF_MAJOR_VERSION	5
#define XTF_MINOR_VERSION	0

struct XTFFileHeader_t : public VTFFileBaseHeader_t
{
	unsigned int	flags;
	unsigned short	width;					// actual width
	unsigned short	height;					// actual height
	unsigned short	depth;					// always 1
	unsigned short	numFrames;
	unsigned short	preloadDataSize;		// exact size of preload data
	unsigned short	imageDataOffset;		// aligned to sector size
	Vector			reflectivity;			// Resides on 16 byte boundary!
	float			bumpScale;
	ImageFormat		imageFormat;
	unsigned char	lowResImageWidth;
	unsigned char	lowResImageHeight;
	unsigned char	fallbackImageWidth;
	unsigned char	fallbackImageHeight;
	unsigned char	mipSkipCount;			// used to resconstruct mapping dimensions
	unsigned char	pad;					// for alignment
};
XTFFileHeader_t XTFFileHeader_Default() {
	XTFFileHeader_t XTF = XTFFileHeader_t();
	strcpy_s(XTF.fileTypeString, (char*)"XTF");
	int version[2] = { XTF_MAJOR_VERSION ,XTF_MINOR_VERSION };
	memcpy(XTF.version, version, sizeof(version));
	XTF.headerSize = sizeof(XTFFileHeader_t);
	Vector reflectivity = { 1.0f ,1.0f ,1.0f };
	memcpy(&XTF.reflectivity, &reflectivity, sizeof(reflectivity));
	XTF.bumpScale = 1.0f;
	XTF.imageDataOffset = 0x200;
	XTF.lowResImageHeight = 1;
	XTF.lowResImageWidth = 1;
	XTF.fallbackImageHeight = 8;
	XTF.fallbackImageWidth = 8;
	XTF.mipSkipCount = 1;
	XTF.depth = 1;
	return XTF;
}


#pragma pack()

int blockSize(ImageFormat f) {
	if (f == ImageFormat::IMAGE_FORMAT_DXT1 or f == ImageFormat::IMAGE_FORMAT_DXT1_ONEBITALPHA) {
		return 8;
	}
	else if (f == ImageFormat::IMAGE_FORMAT_DXT3 or f == ImageFormat::IMAGE_FORMAT_DXT5) {
		return 16;
	}
	else {
		return 1;
	}
};
int* getRes(int w, int h) {
	int res[13] = { 0 };
	int x = 0;
	int i = 1;
	while ((i / w) >= 1 and (h / i) >= 1) {
		res[x++] = ((i / w) * (h / i));
		i = i << 1;
	}
	int* newres = new int[x];
	memcpy(&newres, &res, sizeof(newres));
	free(res);
	return newres;
};
int* mipsize(int w, int h, ImageFormat f) {
	int blkSize = blockSize(f);
	int* resSize = getRes(w, h);
	if (blkSize > 2) {
		if (blkSize == 8) {
			for (int x = 0; x < (sizeof(*resSize) / sizeof(int)); x++) {
				resSize[x] /= 2;
			}
			return resSize;
		}
		else {
			return resSize;
		}
	}
	else {
		for (int x = 0; x < (sizeof(*resSize) / sizeof(int)); x++) {
			resSize[x] *= ImageFormatBlock[f];
		}
	}
};


const char VTFout[5] = "-vtf";
const char XTFout[5] = "-xtf";

const char* DefaultTest = "Command line: vtfTrans.exe\nusage\t: vtfTrans.exe -xtf texture.vtf <optional.xtf>\nCommon options:\n\t-xtf\t: Transition vtf to xtf\n\t-vtf\t: Transition xtf to vtf\n";

char typeIn[5] = "    ";



int main(int argc, char* argv[])
{
	/*VTFFileHeader_t XTFhdr = VTFFileHeader_Default();
	std::ofstream outfile;
	outfile.open("test.bin", std::ios::binary);
	outfile.write((char*)&XTFhdr, sizeof(XTFhdr));*/

	

	if (argc > 1) {
		if (strlen(argv[1]) == 4) {
			for (int x = 0; x < 4; x++) {
				typeIn[x] = tolower(argv[1][x]);
			}
			
		}
		if (!strcmp(typeIn, VTFout)) {
			std::cout << "We are converting xtf2vtf\n";
		}
		else if (!strcmp(typeIn, XTFout)) {
			std::cout << "We are converting vtf2xtf\n";
		}
		else {
			std::cout << "Error\nType output was not input\n" << DefaultTest;
		}
	}
	else {
		std::cout << DefaultTest;
	}


}

void convertData(char* in, char* out, bool isXBOX) {
	std::ifstream infile;
	std::ofstream outfile;
	infile.open(in);
	XTFFileHeader_t XTFhdr;
	VTFFileHeader_t* VTFhdr = (VTFFileHeader_t*)malloc(sizeof(struct VTFFileHeader_t));

	free(VTFhdr);
}




// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
