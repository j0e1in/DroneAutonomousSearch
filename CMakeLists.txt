PROJECT (zed)

# SET(execName ZED_openGL_gpu_interop)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 OLD)
	cmake_policy(SET CMP0015 OLD)
endif(COMMAND cmake_policy)

SET(SRC_FOLDER src/zed)
SET(CMAKE_C++_CREATE_SHARED_LIBRARY 1)
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")# "${CMAKE_CXX_FLAGS} -g -O3" ) # Release Perf mode

FILE(GLOB_RECURSE SRC_FILES "${SRC_FOLDER}/*.cc")

IF(WIN32) # Windows

SET(ZED_INCLUDE_DIRS $ENV{ZED_INCLUDE_DIRS})
SET(ZED_LIBRARIES $ENV{ZED_LIBRARIES_64})
SET(ZED_LIBRARY_DIR $ENV{ZED_LIBRARY_DIR})

SET(GLEW_INCLUDE_DIRS $ENV{GLEW_INCLUDE_DIRS})
SET(GLEW_LIBRARY_DIRS $ENV{GLEW_LIBRARY_DIRS})
SET(GLEW_LIBRARIES $ENV{GLEW_LIBRARY})

SET(GLUT_INCLUDE_DIRS $ENV{GLUT_INCLUDE_DIRS})
SET(GLUT_LIBRARY_DIRS $ENV{GLUT_LIBRARY_DIRS})

SET(OpenCV_INCLUDE_DIRS $ENV{OPENCV_DIR}/include)
SET(OpenCV_LIBRARY_DIRS $ENV{OPENCV_DIR}/x64/vc12/lib)

find_package(CUDA 7.0 REQUIRED)

ELSE() # Linux

find_package(ZED REQUIRED)
find_package(GLUT)
find_package(GLEW)
find_package(CUDA 6.5 REQUIRED)

ENDIF(WIN32)

# Mandatory
find_package(OpenGL)
find_package(OpenCV 2.4)

include_directories(${SRC_FOLDER})
include_directories(${ZED_INCLUDE_DIRS})
include_directories(${GLUT_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${CUDA_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_JS_INC}) # for node cmake-js

link_directories(${ZED_LIBRARY_DIR})
link_directories(${CUDA_LIBRARY_DIRS})
link_directories(${GLUT_LIBRARY_DIRS})
link_directories(${GLEW_LIBRARY_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRC_FILES})
set_target_properties (${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
set_target_properties (${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

#add_definitions(-std=c++0x)# -m64) #-Wall)

# Add the required libraries for linking
TARGET_LINK_LIBRARIES(
                      	${PROJECT_NAME}
												${CMAKE_JS_LIB}
                        ${ZED_LIBRARIES}
												${GLEW_LIBRARIES}
												${OpenCV_LIBRARIES}
                        ${OPENGL_LIBRARIES}
                        ${GLUT_LIBRARY}
												${CUDA_LIBRARIES}
												${CUDA_nppi_LIBRARY}
												${CUDA_npps_LIBRARY}
                    )
