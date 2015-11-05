#ifndef ZED_H
#define ZED_H

#include <string>
#include <vector>

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

int initZed(const std::string arg_str);
bool is_initZed_ready();

#endif /* ZED_H */