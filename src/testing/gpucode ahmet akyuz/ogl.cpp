#ifdef _WIN32
#include <windows.h>
#endif
#include <cassert>
#include <cstdlib>
#include <string>
#include <cstring>
#include <cstdio>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#ifdef _WIN32
#include "glpfn.h"
#endif
#include "utils.h"

using std::string;
using std::vector;

GLuint* ldrTex;          ///< Textures to store the LDR images
GLuint  ldrSampler;      ///< Sampler to fetch from the LDR images
GLuint  hdrTex;          ///< Texture where HDR image will be written to
GLuint  tmTex;           ///< Texture where tonemapped LDR image will be written to
GLuint  hdrSampler;      ///< Sampler to fetch from the HDR image
GLint*  ldrSamplerUnits; ///< Stores which ldr sampler will fetch from which texture unit
GLint   hdrSamplerUnits; ///< Stores which hdr sampler will fetch from which texture unit

//extern Color** ldr;
extern float* hdr;
extern const char* LDRname;
extern int hdrWidth, hdrHeight;
extern int windowWidth, windowHeight;

int width, height, numImages;

GLuint prgHDRCreate;
GLuint prgTonemap;
GLuint prgTonemapGlobal;
GLuint prgShowHDR;

unsigned char** imagesLinearized;

enum Mode { TM_GLOBAL, TM_LOCAL, HDR };
Mode mode = HDR;
float factor = 1.0f;
float sceneKey = 0.18f;
float white = 1e12f;
float saturation = 1.0f;

void UpdateShaderConstant(
    const char* name,
    float       value)
{
    GLint prg;

    glGetIntegerv(GL_CURRENT_PROGRAM, &prg);
    assert(prg != 0); // a program has to be current.

    GLint location = glGetUniformLocation(prg, name);

    glUniform1f(location, value);
}

void UpdateShaderConstant(
    const char* name,
    int         value)
{
    GLint prg;

    glGetIntegerv(GL_CURRENT_PROGRAM, &prg);
    assert(prg != 0); // a program has to be current.

    GLint location = glGetUniformLocation(prg, name);

    glUniform1i(location, value);
}

void UpdateShaderConstant(
    const char* name,
    bool        value)
{
    GLint prg;

    glGetIntegerv(GL_CURRENT_PROGRAM, &prg);
    assert(prg != 0); // a program has to be current.

    GLint location = glGetUniformLocation(prg, name);

    glUniform1i(location, value);
}

void PrintShaderInfoLog(
    GLuint obj)
{
    int infologLength = 0;
    int charsWritten = 0;
    char *infoLog;

    glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *) malloc(infologLength);

        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);

        LogMessage(infoLog);

        free(infoLog);
    }
}

void PrintProgramInfoLog(
    GLuint obj)
{
    int infologLength = 0;
    int charsWritten = 0;
    char *infoLog;

    glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *) malloc(infologLength);

        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);

        LogMessage(infoLog);

        free(infoLog);
    }
}

GLuint CompileShader(
    const char* filename,
    GLenum      shaderType)
{
    string shaderSource;

    if (!ReadDataFromFile(filename, shaderSource))
    {
        string message = string("Cannot find file name: ") + filename;
        LogMessage(message.c_str());

        return false;
    }

    GLuint shader = glCreateShader(shaderType);
    GLint length = shaderSource.length();
    const GLchar* data = (const GLchar*) shaderSource.c_str();

    glShaderSource(shader, 1, &data, &length); 
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if (status != GL_TRUE)
    { 
        PrintShaderInfoLog(shader);
        glDeleteShader(shader);

        return 0;
    }

    return shader;
}

GLuint LinkProgram(
    GLuint vs,
    GLuint gs,
    GLuint fs)
{
    GLuint prg = glCreateProgram();

    if (vs)
    {
        glAttachShader(prg, vs);
    }

    if (gs)
    {
        glAttachShader(prg, gs);
    }

    if (fs)
    {
        glAttachShader(prg, fs);
    }

    glLinkProgram(prg);

    GLint status;
    glGetProgramiv(prg, GL_LINK_STATUS, &status);

    if (status != GL_TRUE)
    { 
        PrintProgramInfoLog(prg);
        glDeleteProgram(prg);

        return 0;
    }

    return prg;
}

bool SetupShaders()
{
    // Shaders for HDR creation
    GLuint vs = CompileShader("vertHDRCreate.glsl", GL_VERTEX_SHADER);
    GLuint fs = CompileShader("fragHDRCreate.glsl", GL_FRAGMENT_SHADER);

    prgHDRCreate = LinkProgram(vs, 0, fs);

    // Shaders for tone mapping (local)
    vs = CompileShader("vertTonemap.glsl", GL_VERTEX_SHADER);
    fs = CompileShader("fragTonemap.glsl", GL_FRAGMENT_SHADER);

    prgTonemap = LinkProgram(vs, 0, fs);

    // Shaders for tone mapping (global)
    fs = CompileShader("fragTonemapGlobal.glsl", GL_FRAGMENT_SHADER);
    prgTonemapGlobal = LinkProgram(vs, 0, fs);

    // Shaders for showing HDR
    vs = CompileShader("vertShowHDR.glsl", GL_VERTEX_SHADER);
    fs = CompileShader("fragShowHDR.glsl", GL_FRAGMENT_SHADER);

    prgShowHDR = LinkProgram(vs, 0, fs);

    return true;
}

bool SetupVertexAttribs()
{
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLfloat posData[][2] = {
        {-1.0f, -1.0f},
        { 1.0f, -1.0f},
        { 1.0f,  1.0f},
        {-1.0f,  1.0f}
    };

    GLfloat colData[][3] = {
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f},
        {1.0f, 1.0f, 0.0f}
    };

    GLfloat texCoordData[][2] = {
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f}
    };

    GLuint attr[3]; // pos, col, texture
    glGenBuffers(3, attr);

    glBindBuffer(GL_ARRAY_BUFFER, attr[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(posData), posData, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, attr[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colData), colData, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, attr[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoordData), texCoordData, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    return true;
}

bool SetupTextures()
{
    // Make sure that the number of images does not exceed the number of texture units.
    
    GLint numTexUnits;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &numTexUnits);
    assert(numImages + 2 <= numTexUnits); // reserve one for HDR image, one for tone mapped image

    ldrTex          = new GLuint [numImages];
    ldrSamplerUnits = new GLint [numImages];

    // Generate the LDR and HDR textures.
    
    glGenTextures(numImages, ldrTex);
    glGenTextures(1, &hdrTex);
    glGenTextures(1, &tmTex);

    // Generate the samplers to fetch from LDR and HDR textures.

    glGenSamplers(1, &ldrSampler);
    glGenSamplers(1, &hdrSampler);

    // Set the LDR sampler properties.
    
    glSamplerParameteri(ldrSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(ldrSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(ldrSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(ldrSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Set the HDR sampler properties. The reason we enabled mipmapping for the
    // HDR texture is so that we can fetch from the highest miplevel to get the
    // average luminance value.
    
    //glSamplerParameteri(hdrSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    //glSamplerParameteri(hdrSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glSamplerParameteri(hdrSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(hdrSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(hdrSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(hdrSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create the destination HDR texture.
    
    glBindTexture(GL_TEXTURE_2D, hdrTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    // Create the destination tonemapped LDR texture

    glBindTexture(GL_TEXTURE_2D, tmTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    for (unsigned int i = 0; i < numImages; ++i)
    {
        glBindTexture(GL_TEXTURE_2D, ldrTex[i]);

        // Use an sRGB texture format to get linearization upon texture fetch.
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imagesLinearized[i]);

        // Images are uploaded to the texture, so we can free their host memory.
        
        delete[] imagesLinearized[i];
    }
    delete[] imagesLinearized;

    assert(GL_NO_ERROR == glGetError());

    return true;
}

void DrawQuad()
{
    GLuint indices[] = {0, 1, 2, 3};

    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, indices);
}

void LoadHDR(float* hdr, int width, int height)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT, hdr);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void CreateHDR(float** hdr, int& hdrWidth, int& hdrHeight)
{
    GLuint fbo;

    // Setup the destination state.

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, hdrTex, 0);

    /*
     * Need to enable this if fragment shader is writing out to more
     * than COLOR_ATTACHMENT0.
     *
    GLenum bufs[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, bufs);
    */

    // Setup the source state.

    for (unsigned int i = 0; i < numImages; ++i)
    {
        ldrSamplerUnits[i] = i;
        glBindSampler(i, ldrSampler);

        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, ldrTex[i]);
    }

    GLint mySamplerLoc = glGetUniformLocation(prgHDRCreate, "MySampler");
    GLint numImagesLoc = glGetUniformLocation(prgHDRCreate, "numImages");

    glUseProgram(prgHDRCreate);

    // Bind the samplers to texture units.

    glUniform1iv(mySamplerLoc, numImages, ldrSamplerUnits);
    glUniform1i(numImagesLoc, numImages);

    glViewport(0, 0, width, height);

    DrawQuad();

    // Detach the texture and switch back to default FBO.
    
    //glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0); NVIDIA BUG
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);

    // Create a mipmap to get an image pyramid.

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTex);
    glGenerateMipmap(GL_TEXTURE_2D);

    hdrWidth = width;
    hdrHeight = height;
    *hdr = new float [hdrWidth * hdrHeight * 3];

    assert(sizeof(GLfloat) == sizeof(float));

    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, *hdr);

    assert(GL_NO_ERROR == glGetError());
}

void SaveImage()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tmTex);

    unsigned char* ldrImg;
    ldrImg = new unsigned char [width * height * 3];

    Color** myLdr = new Color* [height];
    for (int y = 0; y < height; ++y)
    {
        myLdr[y] = new Color [width];
    }

    assert(sizeof(GLubyte) == sizeof(unsigned char));

    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, ldrImg);

    ConvertImage(ldrImg, width, height, &myLdr);
    writeJPEG(LDRname, myLdr, width, height);

    delete[] ldrImg;
}

void ShowHDR()
{
    GLuint fbo;

    // Setup the destination state.

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tmTex, 0);
    glEnable(GL_FRAMEBUFFER_SRGB);

    // Setup the source state.

    glBindSampler(0, hdrSampler);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTex);

    GLint mySamplerLoc = glGetUniformLocation(prgShowHDR, "hdrSampler");

    glUseProgram(prgShowHDR);

    UpdateShaderConstant("factor", factor);
    
    // Bind the samplers to texture units. This basically says that
    // "MySampler" will fetch from unit 0.

    glUniform1i(mySamplerLoc, 0);

    glViewport(0, 0, width, height);

    DrawQuad();
    glDeleteFramebuffers(1, &fbo);

    assert(GL_NO_ERROR == glGetError());
}

void Tonemap()
{
    GLuint fbo;

    // Setup the destination state.

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tmTex, 0);
    glEnable(GL_FRAMEBUFFER_SRGB);

    // Setup the source state.

    glBindSampler(0, hdrSampler);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTex);

    GLint mySamplerLoc;

    if (mode == TM_GLOBAL)
    {
        mySamplerLoc = glGetUniformLocation(prgTonemapGlobal, "hdrSampler");
        glUseProgram(prgTonemapGlobal);
    }
    else
    {
        mySamplerLoc = glGetUniformLocation(prgTonemap, "hdrSampler");
        glUseProgram(prgTonemap);
    }

    UpdateShaderConstant("key", sceneKey);
    UpdateShaderConstant("Yw", white);
    UpdateShaderConstant("sat", saturation);
    
    // Bind the samplers to texture units. This basically says that
    // "MySampler" will fetch from unit 0.

    glUniform1i(mySamplerLoc, 0);

    glViewport(0, 0, width, height);

    DrawQuad();
    glDeleteFramebuffers(1, &fbo);

    assert(GL_NO_ERROR == glGetError());
}

void InitGL()
{
    SetupShaders();
    SetupTextures();
    SetupVertexAttribs();
}

void Resize(
    int width,
    int height)
{
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int a, b;

void Render()
{
    static time_t startTime = time(0);
    static int swaps = 0;

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (mode == TM_GLOBAL || mode == TM_LOCAL)
    {
        Tonemap();
    }
    else
    {
        ShowHDR();
    }

    glViewport(0, 0, windowWidth, windowHeight);
    DrawQuad();

    glutSwapBuffers();

    ++swaps;
    time_t currentTime = time(0);
    if (currentTime == startTime + 1)
    {
        std::cout << "FPS = " << swaps << std::endl;
        swaps = 0;
        startTime = currentTime;
    }
}

void normalKeyboardEvent(unsigned char key, int x, int y)
{
    if (key == 'l')
    {
        mode = TM_LOCAL;
    }
    else if (key == 'g')
    {
        mode = TM_GLOBAL;
    }
    else if (key == 'h')
    {
        mode = HDR;
    }
    else if (key == 'z')
    {
        sceneKey += 0.01f;
    }
    else if (key == 'x')
    {
        sceneKey -= 0.01f;
        sceneKey = sceneKey < 0 ? 0 : sceneKey;
    }
    else if (key == 'c')
    {
        white *= 1.1f;
    }
    else if (key == 'v')
    {
        white /= 1.1f;
    }
    else if (key == 'b')
    {
        saturation += 0.1f;
    }
    else if (key == 'n')
    {
        saturation -= 0.1f;
        saturation = saturation < 0 ? 0 : saturation;
    }
    else if (key == 's')
    {
        SaveImage();
    }
    else if (key == 'q')
    {
        exit(0);
    }
}

void specialKeyboardEvent(int key, int x, int y)
{
    if (key == GLUT_KEY_UP)
    {
        factor *= 1.1;
    }
    else if (key == GLUT_KEY_DOWN)
    {
        factor /= 1.1;
    }
}
