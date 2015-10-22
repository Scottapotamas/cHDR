createHDRGPU: ogl.cpp createHDRGPU.cpp utils.cpp
	g++ createHDRGPU.cpp ogl.cpp utils.cpp rgbe.c -o createHDRGPU -lGL -lglut -lGLEW -ljpeg -lm -Wno-write-strings

clean:
	rm -f *.o createHDRGPU
