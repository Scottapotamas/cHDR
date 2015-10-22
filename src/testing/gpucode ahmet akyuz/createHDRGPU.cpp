/*
 * createHDRGPU.cpp: Source file to create and display HDR images on the GPU
 *
 * Developed by: Ahmet Oguz Akyuz (Middle East Technical University)
 * Year: 2012
 * E-mail: akyuz@ceng.metu.edu.tr
 * Homepage: http://www.ceng.metu.edu.tr/~akyuz
 * Reference: If you use this code in your research project, please
 * cite the following paper:
 *
 *     @article{Akyuz2012,
 *         author={Ahmet O\u{g}uz Aky\"{u}z},
 *         year={2012},
 *         issn={1861-8200},
 *         journal={Journal of Real-Time Image Processing},
 *         doi={10.1007/s11554-012-0270-9},
 *         title={High dynamic range imaging pipeline on the GPU},
 *         url={http://dx.doi.org/10.1007/s11554-012-0270-9},
 *         publisher={Springer-Verlag},
 *         keywords={High dynamic range imaging; GPU programming; Color imaging; Tone mapping; Real-time imaging},
 *         pages={1-15},
 *     }
 *
 * Copyright: Ahmet Oguz Akyuz - 2012
 *
 * Usage: createHDRGPU <HDRname> <LDRname> <LDR1> <LDR2> ... <LDRN>
 *
 * where <HDRname> is the name of the output HDR image
 *       <LDRname> is the name of the snapshot image to be saved when the user presses the "s" key (can be used to retrieve the tone mapped image)
 *       <LDR1> to <LDRN> are the names of the LDR images to be combined   
 *
 * Please see the normalKeyboardEvent and specialKeyboardEvent functions for keyboard options.
 */


#include <GL/glew.h>
#include <GL/glut.h>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include <string>
#include "ogl.h"
#include "rgbe.h"

float* hdr;              ///< Output HDR image
Color** ldr;             ///< Output tone mapped LDR image
const char* LDRname;     ///< Name of the image to be saved when the user presses the "s" key
int hdrWidth, hdrHeight; ///< Dimensions of the HDR image
int windowHeight, windowWidth = 1024;  ///< Dimensions of the display window

extern int width, height, numImages;
extern unsigned char** imagesLinearized;

using std::string;
using std::vector;

void ReadLDRImages(int argc, char* argv[])
{
    vector<string> imageNames;

    for (int i = 0; i < argc - 3; ++i)
    {
        imageNames.push_back(argv[i + 3]);
    }

    numImages = imageNames.size();

    imagesLinearized = new unsigned char* [numImages];

    for (unsigned int i = 0; i < numImages; ++i)
    {
        Color** image;
        ReadJPEG(imageNames[i].c_str(), &image, width, height);

        // Copy data from the 2D array into a 1D array.

        int scanlineBytes = width * 3;

        imagesLinearized[i] = new unsigned char [height * scanlineBytes];

        for (unsigned int y = 0; y < height; ++y)
        {
            memcpy(&(imagesLinearized[i][y * scanlineBytes]), image[(height - 1) - y], scanlineBytes);
            //memcpy(&(imagesLinearized[i][y * scanlineBytes]), image[y], scanlineBytes);
            
            // Delete the 2D images as we no longer need them.

            delete[] image[(height - 1) - y]; 
            //delete[] image[y]; 
        }
        delete[] image;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        std::cerr << "Error: Insufficient arguments" << std::endl;
        std::cerr << "Usage: createHDRGPU <HDRname> <LDRname> <LDR1> <LDR2> ... <LDRN>" << std::endl;
    }

    const char* outputHDRName = argv[1];
    LDRname = argv[2];

    ReadLDRImages(argc, argv); // this updates width and height
    windowHeight = ((float) windowWidth) * height / width;

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("createHDRGPU");

    glewInit();

	std::cout << glGetString(GL_RENDERER) << std::endl;
	std::cout << glGetString(GL_VENDOR) << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;
	std::cout << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    InitGL();

    glutDisplayFunc(Render);
    glutReshapeFunc(Resize);
    glutIdleFunc(Render);
    glutKeyboardFunc(normalKeyboardEvent);
    glutSpecialFunc(specialKeyboardEvent);

    CreateHDR(&hdr, hdrWidth, hdrHeight);
    //Tonemap(&ldr, hdrWidth, hdrHeight);

    FILE* fp = fopen(outputHDRName, "wb");
    RGBE_WriteHeader(fp, hdrWidth, hdrHeight, NULL);
    RGBE_WritePixels_RLE(fp, hdr, hdrWidth, hdrHeight);
    fclose(fp);

    // Show the HDR image on the window
    glutMainLoop();

    return 0;
}

