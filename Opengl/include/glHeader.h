#ifndef GLHEADER_H
#define GLHEADER_H

// glew static lib
#define GLEW_STATIC
#include "GL/glew.h"
#pragma comment (lib, "glew32s.lib")

// glfw static lib
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#pragma comment (lib, "glfw3.lib")

// opengl core api
#pragma comment (lib, "opengl32.lib")

// Simple OpenGL Image Library
#include "SOIL.h"
#pragma comment (lib, "SOIL.lib")

#define BUFFER_OFFSET(a) ((void*)(a))

// win32
#if defined(_WIN32) && defined(_WINDOWS)
#include "ft2build.h"
#include FT_FREETYPE_H
#pragma comment (lib,"freetype.lib")
#endif

#endif //GLHEADER_H