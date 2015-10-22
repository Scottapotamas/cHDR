#ifndef __OGL_H__
#define __OGL_H__

#include "utils.h"

void UpdateShaderConstant(const char* name, float value);
void UpdateShaderConstant(const char* name, int value);
void UpdateShaderConstant(const char* name, bool value);
void CreateHDR(float** hdr, int& width, int& height);
void Tonemap();
void LoadHDR(float* hdr, int width, int height);
void InitGL();
void Resize(int width, int height);
void Render();
void normalKeyboardEvent(unsigned char key, int x, int y);
void specialKeyboardEvent(int key, int x, int y);

#endif // __OGL_H__
