#ifdef _WIN32
#include <windows.h>
#endif
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cassert>

extern "C" {
#define INT32 INT32JPEG
#undef FAR
#include <jpeglib.h>
#undef INT32JPEG
}

#include "utils.h"

using std::fstream;
using std::string;

/**
****************************************************************************
*   ReadDataFromFile
*
*   @brief
*       Read the contents of the data from the given file
*
*   @return
*       true if the read succeeded, false otherwise
****************************************************************************
*/
bool ReadDataFromFile(
    const string& fileName, ///< [in]  Name of the shader file
    string&       data)     ///< [out] The contents of the file
{
    fstream myfile;

    // Open the input 
    myfile.open(fileName.c_str(), std::ios::in);

    if (myfile.is_open())
    {
        string curLine;

        while (getline(myfile, curLine))
        {
            data += curLine;
            if (!myfile.eof())
            {
                data += "\n";
            }
        }

        myfile.close();
    }
    else
    {
        return false;
    }

    return true;
}

/**
****************************************************************************
*   WritePPM
*
*   @brief
*       Write out a three channel RGB PPM image file.
*
*   @return
*       true if success, false otherwise.
****************************************************************************
*/
bool WritePPM(
    const char*          filename, ///< [in] Image file name
    const unsigned char* data,     ///< [in] Image data (in RGB order)
    int                  width,    ///< [in] Image width
    int                  height)   ///< [in] Image height
{
    FILE* fp = fopen(filename, "w");

    if (!fp)
    {
        return false;
    }

    fprintf(fp, "P3\n");
    fprintf(fp, "%d %d\n", width, height);
    fprintf(fp, "256\n");

    const unsigned char* pix;
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            pix = &data[((height -  1) - y) * width * 3 + x * 3];
            fprintf(fp, "%d %d %d ", pix[0], pix[1], pix[2]);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);

    return true;
}

/**
****************************************************************************
*   ReadJPEG
*
*   @brief
*       Read in a three channel RGB JPEG image file. This function allocates
*       memory for the image data.
*
*   @return
*       true if success, false otherwise.
****************************************************************************
*/
bool ReadJPEG(
	const char* filename, ///< [in]  JPEG file name
	Color***    image,    ///< [out] Image data array 
	int&        width,    ///< [out] Image width
	int&        height)   ///< [out] Image height
{
	FILE* fp = fopen(filename, "rb");

	if (!fp)
	{
		return false;
	}

	jpeg_error_mgr err;
	jpeg_decompress_struct cinfo;

	cinfo.err = jpeg_std_error(&err);

	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, fp);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	width = cinfo.output_width;
	height = cinfo.output_height;

	assert(sizeof(Color) == (sizeof(JSAMPLE) * cinfo.output_components));

	*image = new Color* [height];
	for (int i = 0; i < height; ++i)
	{
		(*image)[i] = new Color [width];
	}

	JSAMPROW scanline = new JSAMPLE [width * cinfo.output_components];

	int y = 0;
	while (cinfo.output_scanline < cinfo.output_height)
	{
		jpeg_read_scanlines(&cinfo, &scanline, 1);
		memcpy((*image)[y++], scanline, sizeof(JSAMPLE) * width * cinfo.output_components);
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	delete[] scanline;

	fclose(fp);

    return true;
}

/**
****************************************************************************
*   WritePEG
*
*   @brief
*       Write out a three channel RGB JPEG image file. 
*
*   @return
*       true if success, false otherwise.
****************************************************************************
*/
bool writeJPEG(
	const char* filename, ///< [in] JPEG file name
	Color**     image,    ///< [in] Image data array 
	int         width,    ///< [in] Image width
	int         height)   ///< [in] Image height
{
	FILE* fp = fopen(filename, "wb");

	if (!fp)
	{
		return false;
	}

	jpeg_error_mgr err;
	jpeg_compress_struct cinfo;

	cinfo.err = jpeg_std_error(&err);

	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, fp);

	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;

	jpeg_set_defaults(&cinfo);

	jpeg_start_compress(&cinfo, TRUE);

	assert(sizeof(Color) == (sizeof(JSAMPLE) * cinfo.input_components));

	JSAMPROW scanline;

	int y = 0;
	while (cinfo.next_scanline < cinfo.image_height)
	{
		scanline = (JSAMPROW) &(image[y++][0]);
		jpeg_write_scanlines(&cinfo, &scanline, 1);
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	fclose(fp);

    return true;
}

/**
****************************************************************************
*   ConvertImage
*
*   @brief
*       Convert a 1D array of pixels to a 2D image.
*
*   @return
*       N/A
****************************************************************************
*/
void ConvertImage(
    unsigned char* arrayImg, ///< [in]  1D image with channels ordered as R,G,B for each pixel
    int            width,    ///< [in]  Image width
    int            height,   ///< [in]  Image height
    Color***       colorImg) ///< [out] 2D image
{
    *colorImg = new Color* [height];

    for (unsigned int y = 0, i = 0; y < height; ++y)
    {
        //(*colorImg)[y] = new Color [width];
        (*colorImg)[height-1-y] = new Color [width];

        for (unsigned int x = 0; x < width; ++x, i += 3)
        {
            //memcpy(&(*colorImg)[y][x], &arrayImg[i], 3);
            memcpy(&(*colorImg)[height-1-y][x], &arrayImg[i], 3);
        }
    }
}

/**
****************************************************************************
*   LogMessage
*
*   @brief
*       Log out the given message in various formats.
*
*   @return
*       N/A
****************************************************************************
*/
void LogMessage(
    const char* message) ///< [in] Message to log
{
    printf("%s\n", message);

#ifdef _WIN32
    OutputDebugStringA(message);
    OutputDebugStringA("\n");

    MessageBox(0, message, 0, 0);
#endif
}
