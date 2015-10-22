#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>

using std::string;

typedef unsigned char Color[3];

/**
****************************************************************************
*   ReadDataFromFile
*
*   @brief
*       Read the contents of the data from the given file
*
*   @return
*       true if the read succeeded, false otherwise
*
****************************************************************************
*/
bool ReadDataFromFile(
    const string& fileName, ///< [in]  Name of the shader file
    string&       data);    ///< [out] The contents of the file

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
    int                  height);  ///< [in] Image height

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
	int&        height);  ///< [out] Image height

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
	int         height);  ///< [in] Image height

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
    unsigned char* arrayImg,  ///< [in]  1D image with channels ordered as R,G,B for each pixel
    int            width,     ///< [in]  Image width
    int            height,    ///< [in]  Image height
    Color***       colorImg); ///< [out] 2D image

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
    const char* message); ///< [in] Message to log

#endif // __UTILS_H__
