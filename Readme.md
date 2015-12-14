#Drone Search

##Requirememts
###Windows
- Visual Studio 2013 (2015 is not supported by cuda 7.0)
- Visual Studio 2012 redistributable x64
- node v0.12.x
- python 2.7 (for node modules)
- cmake-js v2.1.0
- Nvidia Driver
- CUDA 7.0
- ZED v0.9.2
- OpenCV 2.4.9

###Add Path
- Add 'C:\\..opencv_2.4.9\build'
	or '[ZED SDK PATH]\dependencies\opencv_2.4.9' as system variable 'OPENCV_DIR'
- Add 'C:\\..opencv_2.4.9\build\x64\vc12\bin' to 'Path'


##Build & Run
```
 $ npm install
 $ npm run build		(to build)
 $ npm start			(to start)
```