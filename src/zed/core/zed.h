#ifndef ZED_H
#define ZED_H

//ZED include
#include <zed/Camera.hpp>
#include <thread>

/// Glut and OpenGL extension (GLEW) for shaders
#include "GL/glew.h"
#include "GL/glut.h"

//Cuda includes
#include "cuda.h"
#include "cuda_runtime.h"
#include "cuda_gl_interop.h"

int initZed(int argc, char *argv[]);
bool is_initZed_ready();

#endif /* ZED_H */