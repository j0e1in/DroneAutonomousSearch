#Drone Autonomous Searching System

A program that makes drone search in a closed area autonomously. By observing surroundings with a front stereo camera (no budget for 3 or 4 cameras), it detects obstacles and avoids them in real-time. No foreknowledge of the environment is required. I use opencv cascade classifiers for demonstrating searching for humans in a closed area.

The obstacle avoidance and human detection parts are written in c++, and then are wrapped as a nodejs library for controlling drone using nodejs. (Since the [third-party  ARDrone2.0 nodejs client](https://github.com/felixge/node-ar-drone) is much easier to program.)

##Requirememts

### Hardware

- ARDrone 2.0
- Stereolab [ZED stereo camera](https://www.stereolabs.com/zed/specs/)

### Windows

- Visual Studio 2013 (2015 is not supported by cuda 7.0)
- Visual Studio 2012 redistributable x64
- node v0.12.x
- python 2.7 (for npm modules)
- cmake-js v2.1.0
- Nvidia Driver
- CUDA 7.0
- ZED SDK v0.9.2
- OpenCV 2.4.9

###Add Path
- Add `(opencv_2.4.9 root)\build` (absolute path) as system variable `OPENCV_DIR`
- Add `(opencv_2.4.9 root)\build\x64\vc12\bin` (ausolute path) to `Path`

##Build & Run
```bash
 $ mkdir build && cd build
 $ cmake ..
 $ npm install		(to install dependent npm modules)
 $ npm run build		(to build)
 $ npm start			(to start)
```
