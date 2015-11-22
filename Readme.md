#Drone Search

##Requirememts
###Windows
- Visual Studio 2013 (2015 is not supported by cuda 7.0)
- Visual Studio 2012 redistributable x64
- node v0.12.x
- python 2.7 (for node modules)
- cmake-js
- Nvidia Driver
- CUDA 7.0
- ZED v0.9.1

<!-- NO NEED FOR NOW
- Boost library
	- BOOST_INCLUDE_DIRS: ${BOOST_ROOT}
	- BOOST_LIBRARY_DIR_64: ${BOOST_ROOT}\lib64-msvc-12.0
-->


##Build & Run
```
 $ npm install
 $ npm run build		(to build)
 $ npm run				(to run)
 $ npm test				(to test)
```