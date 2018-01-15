#include <algorithm>
#include <vector>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#if defined(ANDROID) || defined(__ANDROID__)
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#endif

extern const char* AppResourcePath(const char* fileName);
extern void* getAAssetManager();

#include "app_util.h"

void WinErrorMessageBox(const char* functionLastCalled, int errorCode)
{
	LOGE(" %s : 0x%X\n", functionLastCalled, errorCode);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

FileData::FileData(const char* fileName)
	: m_size(0)
, m_data(0)
{
#ifdef __ANDROID__
	AAsset* asst = AAssetManager_open((AAssetManager*)getAAssetManager(), fileName, AASSET_MODE_UNKNOWN);
	if(NULL == asst)
		return;


	m_size = (int)AAsset_getLength(asst);
	int size_4   = int((m_size+7)/4) * 4;
	m_data  = (char*)malloc(size_4);
	memset(m_data, 0, size_4);
	AAsset_read(asst, m_data, m_size);
	AAsset_close(asst);
#else
	const char* full_path = AppResourcePath(fileName);
	FILE* fp = fopen(full_path, "rb");
	if(fp)
	{
		fseek(fp, 0, SEEK_END);
		m_size = (int)ftell(fp);
		if(0<m_size)
		{
			m_data = (char*)malloc(m_size+1);
			fseek(fp, 0, SEEK_SET);
			fread(m_data, 1, m_size, fp);
			m_data[m_size]=0;
		}
		fclose(fp);
	}
#endif
}

FileData::~FileData()
{
	if(m_data)
	{
		m_size = 0;
		free(m_data);
		m_data = 0;
	}
}

int FileData::getFileDataWithMalloc(char** ret_buf, int* ret_len, const char* fileName)
{
	if(!ret_buf || !ret_len)
		return -1;
	int size   = 0;
	char* data = 0;
#ifdef __ANDROID__
	AAsset* asst = AAssetManager_open((AAssetManager*)getAAssetManager(), fileName, AASSET_MODE_UNKNOWN);
	if(NULL == asst)
		return -1;

	size = (int)AAsset_getLength(asst);
	int size_4   = int((size+7)/4) * 4;
	data  = (char*)malloc(size_4);
	memset(data, 0, size_4);
	AAsset_read(asst, data, size);
	AAsset_close(asst);
#else
	const char* full_path = AppResourcePath(fileName);
	FILE* fp = fopen(full_path, "rb");
	if(!fp)
		return -1;

	fseek(fp, 0, SEEK_END);
	size = (int)ftell(fp);
	if(0<size)
	{
		data = (char*)malloc(size+1);
		fseek(fp, 0, SEEK_SET);
		fread(data, 1, size, fp);
		data[size]=0;
	}
	fclose(fp);
#endif
	*ret_len = size;
	*ret_buf = data;
	return 0;
}

std::string FileData::fileExtension(const char * path)
{
	char* file_name=(char*)path;
	int file_name_len = strlen(file_name);
	file_name +=file_name_len ;

	char *file_ext ;
	for(int i =0 ; i <file_name_len ; i ++)
	{
		if(* file_name == '.')
		{
			file_ext = file_name +1 ;
			break;
		}
		file_name --;
	}

	std::string ret(file_ext);
	std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
	return ret ;
}

struct TGAHEADER
{
	unsigned char	ImgT;				// imageTypeCode
	short			ImgW;				// Width
	short			ImgH;				// Height
	unsigned char	ImgB;				// Bit Count
};

int LoadTGA(int* oW, int* oH, int* oD, unsigned char** oB, const char* _src_buf, size_t src_size)
{
	char*			src_buf = (char*)_src_buf;
	unsigned char	charBad;					// garbage data
	short			sintBad;					// garbage data
	long			imageSize;					// size of TGA image

	TGAHEADER		Tga;

	int				nImgB;
	unsigned char*	src_pxl = NULL;


	// read first two bytes of garbage
	memcpy(&charBad, src_buf, sizeof(unsigned char) * 1);	src_buf += sizeof(unsigned char) * 1;
	memcpy(&charBad, src_buf, sizeof(unsigned char) * 1);	src_buf += sizeof(unsigned char) * 1;

	// read in the image type
	memcpy(&Tga.ImgT, src_buf, sizeof(Tga.ImgT) * 1);	src_buf += sizeof(Tga.ImgT) * 1;

	// for our purposes, the image type should be either a 2 or a 3
	if((Tga.ImgT != 2) && (Tga.ImgT != 3))
		return -1;


	// read 13 bytes of garbage data
	memcpy(&sintBad, src_buf, sizeof(short)         * 1);	src_buf += sizeof(short) * 1;
	memcpy(&sintBad, src_buf, sizeof(short)         * 1);	src_buf += sizeof(short) * 1;
	memcpy(&charBad, src_buf, sizeof(unsigned char) * 1);	src_buf += sizeof(unsigned char) * 1;
	memcpy(&sintBad, src_buf, sizeof(short)         * 1);	src_buf += sizeof(short) * 1;
	memcpy(&sintBad, src_buf, sizeof(short)         * 1);	src_buf += sizeof(short) * 1;

	// read image dimensions
	memcpy(&Tga.ImgW, src_buf, sizeof(short)        * 1);	src_buf += sizeof(short) * 1;
	memcpy(&Tga.ImgH, src_buf, sizeof(short)        * 1);	src_buf += sizeof(short) * 1;

	// read bit depth
	memcpy(&Tga.ImgB, src_buf, sizeof(unsigned char) * 1);	src_buf += sizeof(unsigned char) * 1;

	// read garbage
	memcpy(&charBad, src_buf, sizeof(unsigned char) * 1);	src_buf += sizeof(unsigned char) * 1;


	// colormode -> 3 = BGR, 4 = BGRA
	nImgB		= Tga.ImgB>>3;
	imageSize	= Tga.ImgW * Tga.ImgH * nImgB;
	src_pxl		= new unsigned char[imageSize]{};

	memcpy(src_pxl, src_buf, sizeof(unsigned char) * imageSize);	src_buf += sizeof(unsigned char) * imageSize;

	*oW	= Tga.ImgW;
	*oH	= Tga.ImgH;
	*oD	= 4;

	unsigned char* dst_pxl	= new unsigned char[Tga.ImgW * Tga.ImgH* 4]{};
	*oB = dst_pxl;

	// BGR
	if(3 == nImgB)
	{
		for(int y=0; y<Tga.ImgH; ++y)
		{
			for(int x=0; x<Tga.ImgW; ++x)
			{
				int n1 = (y*Tga.ImgW + x)* 3;
				int n2 = (y*Tga.ImgW + x)* 4;

				unsigned char B = src_pxl[n1+0];
				unsigned char G = src_pxl[n1+1];
				unsigned char R = src_pxl[n1+2];

				dst_pxl[n2+0] = R;
				dst_pxl[n2+1] = G;
				dst_pxl[n2+2] = B;
				dst_pxl[n2+3] = 0xFF;
			}
		}
	}

	// BGRA
	else if(4 == nImgB)
	{
		for(int y=0; y<Tga.ImgH; ++y)
		{
			for(int x=0; x<Tga.ImgW; ++x)
			{
				int n1 = (y*Tga.ImgW + x)* 4;
				int n2 = (y*Tga.ImgW + x)* 4;

				unsigned char B = src_pxl[n1+0];
				unsigned char G = src_pxl[n1+1];
				unsigned char R = src_pxl[n1+2];
				unsigned char A = src_pxl[n1+3];

				dst_pxl[n2+0] = R;
				dst_pxl[n2+1] = G;
				dst_pxl[n2+2] = B;
				dst_pxl[n2+3] = A;
			}
		}
	}

	delete[] src_pxl;
	return 0;
}

int LoadRAW(int* oW, int* oH, int* oD, unsigned char** oB, const char *sFile)
{
	return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#include <png.h>
#define PNGSIG_SIZE  8

struct PNG_SRC
{
	const unsigned char* data;
	size_t size;
	int offset;
};

static void pngReadCallback(png_structp png_ptr, png_bytep data, png_size_t length)
{
	PNG_SRC* isource = (PNG_SRC*)png_get_io_ptr(png_ptr);

	if((int)(isource->offset + length) <= isource->size)
	{
		memcpy(data, isource->data+isource->offset, length);
		isource->offset += length;
	}
	else
	{
		png_error(png_ptr, "pngReaderCallback failed");
	}
}

int LoadPNG(int* oW, int* oH, int* oD, unsigned char** oB, const char* _src_buf, const size_t src_size)
{
	png_uint_32    img_width  ={};
	png_uint_32    img_height ={};
	png_byte       img_depth  ={};
	png_byte       img_bit    ={};
	png_uint_32    img_type   ={};
	unsigned char* img_data   ={};
	size_t         img_dataLen={};

	char* src_buf = (char*)_src_buf;
	int ret = -1;
	png_byte    pbSig[PNGSIG_SIZE]{};
	png_structp png_ptr     = 0;
	png_infop   info_ptr    = 0;

	if(PNGSIG_SIZE > src_size)
		return -1;

	// check the data is png or not
	memcpy(pbSig, src_buf, PNGSIG_SIZE);
	if(png_sig_cmp(pbSig, 0, PNGSIG_SIZE));

	// init png_struct
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, (png_error_ptr)NULL, (png_error_ptr)NULL);
	if(! png_ptr)
		return -1;

	// init png_info
	info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr)
		return -1;

	// set the read call back function
	PNG_SRC imageSource;
	imageSource.data    = (unsigned char*)src_buf;
	imageSource.size    = src_size;
	imageSource.offset  = 0;
	png_set_read_fn(png_ptr, &imageSource, pngReadCallback);

	// read png pbSig info

	// read png file info
	png_read_info(png_ptr, info_ptr);

	img_width  = png_get_image_width(png_ptr, info_ptr);
	img_height = png_get_image_height(png_ptr, info_ptr);
	img_bit    = png_get_bit_depth(png_ptr, info_ptr);
	img_type   = png_get_color_type(png_ptr, info_ptr);

	//CCLOG("color type %u", img_type);

	// force palette images to be expanded to 24-bit RGB
	// it may include alpha channel
	if(img_type == PNG_COLOR_TYPE_PALETTE)
	{
		png_set_palette_to_rgb(png_ptr);
	}
	// low-bit-depth grayscale images are to be expanded to 8 bits
	if(img_type == PNG_COLOR_TYPE_GRAY && img_bit < 8)
	{
		img_bit = 8;
		png_set_expand_gray_1_2_4_to_8(png_ptr);
	}
	// expand any tRNS chunk src_buf into a full alpha channel
	if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
	{
		png_set_tRNS_to_alpha(png_ptr);
	}
	// reduce images with 16-bit samples to 8 bits
	if(img_bit == 16)
	{
		png_set_strip_16(png_ptr);
	}

	// Expanded earlier for grayscale, now take care of palette and rgb
	if(img_bit < 8)
	{
		png_set_packing(png_ptr);
	}
	// update info
	png_read_update_info(png_ptr, info_ptr);
	img_type = png_get_color_type(png_ptr, info_ptr);

	img_depth = 1;
	switch(img_type)
	{
		case PNG_COLOR_TYPE_GRAY:
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			img_depth = 1;
			break;
		case PNG_COLOR_TYPE_RGB:
			img_depth = 3;
			break;
		case PNG_COLOR_TYPE_RGB_ALPHA:
			img_depth = 4;
			break;
		default:
			break;
	}

	// read png src_buf
	png_size_t rowbytes;
	png_bytep* row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * img_height);

	rowbytes = png_get_rowbytes(png_ptr, info_ptr);

	img_dataLen = rowbytes * img_height;
	img_data = static_cast<unsigned char*>(malloc(img_dataLen * sizeof(unsigned char)));
	if(!img_data)
	{
		if(row_pointers != nullptr)
		{
			free(row_pointers);
		}
		return -1;
	}

	for(unsigned short i = 0; i < img_height; ++i)
	{
		row_pointers[i] = img_data + i*rowbytes;
	}
	png_read_image(png_ptr, row_pointers);

	png_read_end(png_ptr, nullptr);

	if(row_pointers != nullptr)
	{
		free(row_pointers);
	}

#define CC_RGB_PREMULTIPLY_ALPHA(vr, vg, vb, va) \
    (unsigned)(((unsigned)((unsigned char)(vr) * ((unsigned char)(va) + 1)) >> 8) | \
    ((unsigned)((unsigned char)(vg) * ((unsigned char)(va) + 1) >> 8) << 8) | \
    ((unsigned)((unsigned char)(vb) * ((unsigned char)(va) + 1) >> 8) << 16) | \
    ((unsigned)(unsigned char)(va) << 24))


	if(4 == img_depth)
	{
		unsigned int* pixel = (unsigned int*)img_data;
		int image_size = img_width * img_height;
		for(int i = 0; i < image_size; ++i)
		{
			unsigned char* p = img_data + i * 4;
			pixel[i] = CC_RGB_PREMULTIPLY_ALPHA(p[0], p[1], p[2], p[3]);
		}
		
	}

	if(png_ptr)
	{
		png_destroy_read_struct(&png_ptr, (info_ptr) ? &info_ptr : 0, 0);
	}

	if(oW) *oW = img_width;
	if(oH) *oH = img_height;
	if(oD) *oD = img_depth;
	if(oB) *oB = img_data;
	return 0;
}
