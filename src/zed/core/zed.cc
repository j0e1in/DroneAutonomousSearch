/**
* Cmd line Arg(optional): SVO path
*/

// #include <opencv2/highgui/highgui.hpp>

//Standard includes
#include <iostream>
#include <chrono> //high_resolution_clock
#include <thread>
#include <ctime>
#include <vector>
#include <future>

//ZED include
#include <zed/Camera.hpp>

/// Glut and OpenGL extension (GLEW) for shaders
#include "GL/glew.h"
#include "GL/glut.h"

//Cuda includes
#include "cuda.h"
#include "cuda_runtime.h"
#include "cuda_gl_interop.h"

#include "FPS.h"
#include "zed.h"
#include "env_eval.h"
#include "object_detect.h"

using namespace std;
using namespace sl::zed;
using namespace std::chrono;

//declare some ressources (GL texture ID, GL shader ID...)
GLuint imageTex;
GLuint depthTex;
cudaGraphicsResource* pcuImageRes;
cudaGraphicsResource* pcuDepthRes;
Camera* zed;

string DisplayModeStr;
SENSING_MODE dm_type;

GLuint shaderF;
GLuint program;

bool quit;
bool refreshed; // for object detection
bool initZed_ready = false;
int reliabilityIdx;
Fps fps;

cv::Mat cvimg;
// cv::Mat frame, img;
// std::vector<cv::Rect> rects;


//Here is the very simple fragment shader for flipping Red and Blue
std::string strFragmentShad = ("uniform sampler2D texImage;\n"
      " void main() {\n"
      " vec4 color = texture2D(texImage, gl_TexCoord[0].st);\n"
      " gl_FragColor = vec4(color.b, color.g, color.r, color.a);\n}");

bool is_initZed_ready(){
  return initZed_ready;
}

void keyboard(unsigned char key, int x, int y)
{
  switch(key)
  {
    //re-compute alignment
    case 'r' :
      cout<<"reset Alignment"<<endl;
      zed->reset();
      break;

    case '+':
      num_blks_w += 1;
      update_grids(num_blks_w);
      break;

    case '-':
      if (num_blks_w > 0)
        num_blks_w -= 1;
      update_grids(num_blks_w);
      break;

    case '1':
      log_opt.drawDepthMode = 1;
      break;

    case '2':
      log_opt.drawDepthMode = 2;
      break;

    case '3':
      log_opt.drawDepthMode = 3;
      break;

    case '4':
      log_opt.drawDepthMode = 4;
      break;

    case 'm':
      isMovingForward = !isMovingForward;
      break;

    case 'q':
      quit = true;
      break;
  }
}

// Print text on glut window
void printtext(int x, int y, string String)
{
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  int w_wnd = glutGet(GLUT_WINDOW_WIDTH);
  int h_wnd = glutGet(GLUT_WINDOW_HEIGHT);
  glOrtho(0, w_wnd, 0, h_wnd, -1.0f, 1.0f);
  glColor3f(0.008f, 0.925f, 0.925f);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  int w_wnd_2 = (w_wnd / 2);

  if (x < w_wnd_2) x += w_wnd_2;

  y = h_wnd - y;

  glRasterPos2i((GLint)(x - (w_wnd * 0.01)), (GLint)(y + (h_wnd * 0.02)));
  for (unsigned int i = 0; i < String.size(); i++)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, String[i]);

  glPopAttrib();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glDisable(GL_TEXTURE_2D);
  float r_ = (float)0.01;
  float d_ = (r_ * w_wnd) / w_wnd;

  float n_x_ = (float)x / w_wnd;
  float n_y_ = (float)y / h_wnd;

  float x_p1 = (n_x_ - d_) * 2 - 1;
  float x_p2 = (n_x_ + d_) * 2 - 1;
  float y_p1 = (n_y_ - d_) * 2 - 1;
  float y_p2 = (n_y_ + d_) * 2 - 1;

  glColor3f(0.008f, 0.925f, 0.925f);
  glBegin(GL_LINES);
  glVertex2f(x_p1, y_p1);
  glVertex2f(x_p2, y_p2);
  glVertex2f(x_p2, y_p1);
  glVertex2f(x_p1, y_p2);
  glEnd();
  glEnable(GL_TEXTURE_2D);
}

void print_depth_info()
{
  int i, j;

  switch(log_opt.drawDepthMode){

    case 1: // draw distance

      for (i = 0; i < blks_h; ++i){
        for (j = 0; j < blks_w; ++j){

          ostringstream ostr;
          if (grid[i][j].dist > 0) {
            ostr << (float)grid[i][j].dist / 100. << "m";
          } else {
            ostr << "NaN";
          }

          printtext(r_grid[i][j].x, r_grid[i][j].y, ostr.str().c_str());
          glColor3f(1.0f, 1.0f, 1.0f);
        }
      }

      break;

    case 2: // draw area classification

      for (i = 0; i < blks_h; ++i){
        for (j = 0; j < blks_w; ++j){
          ostringstream ostr;
          // ostr << "[" << grid[i][j].valid << "@"
          //      << valid_classfied.area_grid[i][j].area_no << "]";
          ostr << "[" << grid[i][j].valid << "-"
               << (float)grid[i][j].dist / 100. << "]";

          printtext(r_grid[i][j].x, r_grid[i][j].y, ostr.str().c_str());
          glColor3f(1.0f, 1.0f, 1.0f);
        }
      }
      break;

    case 3: // draw valid / invalid / danger areas

      if (danger_areas.num_area > 0){ // draw danger areas
        for (int i = sens_area.topl.h-1; i < sens_area.btmr.h; ++i){
          for (int j = sens_area.topl.w-1; j < sens_area.btmr.w; ++j){
            if (grid[i][j].valid < 0){
              ostringstream ostr;
              // ostr << "dg-\n" << (float)grid[i][j].dist / 100.;
              ostr << "dg";
              printtext(r_grid[i][j].x, r_grid[i][j].y, ostr.str().c_str());
              glColor3f(1.0f, 1.0f, 1.0f);
            }
          }
        }
      }else if (invalid_areas.num_area > 0){
        for (int i = sens_area.topl.h-1; i < sens_area.btmr.h; ++i){
          for (int j = sens_area.topl.w-1; j < sens_area.btmr.w; ++j){
            if (grid[i][j].valid == 0){
              ostringstream ostr;
              // ostr << "nv-\n" << (float)grid[i][j].dist / 100.;
              ostr << "nv";
              printtext(r_grid[i][j].x, r_grid[i][j].y, ostr.str().c_str());
              glColor3f(1.0f, 1.0f, 1.0f);
            }
          }
        }
      }else{ // draw valid blks
        for (int i = sens_area.topl.h-1; i < sens_area.btmr.h; ++i){
          for (int j = sens_area.topl.w-1; j < sens_area.btmr.w; ++j){
            if (grid[i][j].valid > 0){
              ostringstream ostr;
              // ostr << "vd-\n" << (float)grid[i][j].dist / 100.;
              ostr << "vd";
              printtext(r_grid[i][j].x, r_grid[i][j].y, ostr.str().c_str());
              glColor3f(1.0f, 1.0f, 1.0f);
            }else{
              cerr << "WARN: has blk is not valid in valid drawing loop" << endl;
            }
          }
        }
      }
      break;

    case 4: // draw sensing area (not ignored for obstacle detection)

      for (i = 0; i < blks_h; ++i){
        for (j = 0; j < blks_w; ++j){
          ostringstream ostr;
          // If the block is inside the area then draw it
          if (i >= sens_area.topl.h-1
           && i < sens_area.btmr.h
           && j >= sens_area.topl.w-1
           && j < sens_area.btmr.w){
            if (grid[i][j].dist > 0) {
              ostr << (float)grid[i][j].dist / 100. << "m";
            } else {
              ostr << "NaN";
            }
            printtext(r_grid[i][j].x, r_grid[i][j].y, ostr.str().c_str());
            glColor3f(1.0f, 1.0f, 1.0f);
          }
        }
      }
      break;

    default: break;
  }
  return;
}

// Retreive distance of each block, store it to grid
void get_depth()
{
  int i, j;
  int x, y;
  int depth_clamp;
  float depth_click;

  for (i = 0; i < blks_h; ++i){
    for (j = 0; j < blks_w; ++j){
      x = grid[i][j].x;
      y = grid[i][j].y;

      // print measured depth on a point
      if (x >= 0 && y >= 0){
        depth_click = 0.f;

        // ***** Get distance of a block
        cudaMemcpy(&depth_click, (float*)(zed->retrieveMeasure_gpu(DEPTH).data + y * zed->retrieveMeasure_gpu(DEPTH).step + (x * 4)), 4, cudaMemcpyDeviceToHost);
        // *****

        // Convert distance from 'mm' to 'cm'
        depth_clamp = (int)(depth_click / (float)10.);

        if (depth_clamp > 0){
          grid[i][j].dist = depth_clamp; // Save distance, in 'cm'
        }
        else{
          grid[i][j].dist = -1; // Save distance as negative on failure
        }
      }
    }
  }
  return;
}

// A infinite loop for an async thread for object detection
void detectObject(bool show){
  while(true){
    if (refreshed){
      refreshed = false;
      if (detectAndDisplay(cvimg, show))
        objDetected = true;
    }
    else
      std::this_thread::sleep_for (std::chrono::milliseconds(50));
  }
}

//glut main loop : grab --> extract GPU Mat --> send to OpenGL and quad.
void draw()
{
  high_resolution_clock::time_point t1 = high_resolution_clock::now();

  int res = zed->grab(dm_type, true, true);

  if (!res)
  {
    fps.update();

    // Map GPU Ressource for Image
    // With Gl texture, we have to use the cudaGraphicsSubResourceGetMappedArray cuda functions. It will link the gl texture with a cuArray.
    // Then, we just have to copy our GPU Buffer to the CudaArray (D2D copy).
    // Between , you can do some other GPU stuffs.

    Mat gpuImage = zed->retrieveImage_gpu(SIDE::LEFT); // for cuda
    Mat cpuImage = zed->retrieveImage(SIDE::LEFT);  // for opencv

    // convert ZED Mat to cv Mat
    slMat2cvMat(cpuImage).copyTo(cvimg);
    // detectAndDisplay(cvimg);

    refreshed = true; // refreshed img for opencv
    // std::async(std::launch::async, detectAndDisplay, cvimg);


    cudaArray_t ArrIm;
    cudaGraphicsMapResources(1, &pcuImageRes, 0);
    cudaGraphicsSubResourceGetMappedArray(&ArrIm, pcuImageRes, 0, 0);
    cudaMemcpy2DToArray(ArrIm, 0, 0, gpuImage.data, gpuImage.step, w * 4, h, cudaMemcpyDeviceToDevice);
    cudaGraphicsUnmapResources(1, &pcuImageRes, 0);

    //map GPU Ressource for Depth !! Image of depth == 8U 4channels
    Mat gpuDepth = zed->normalizeMeasure_gpu(MEASURE::DEPTH);
    cudaArray_t ArrDe;
    cudaGraphicsMapResources(1, &pcuDepthRes, 0);
    cudaGraphicsSubResourceGetMappedArray(&ArrDe, pcuDepthRes, 0, 0);
    cudaMemcpy2DToArray(ArrDe, 0, 0, gpuDepth.data, gpuDepth.step, w * 4, h, cudaMemcpyDeviceToDevice);
    cudaGraphicsUnmapResources(1, &pcuDepthRes, 0);

    DisplayModeStr = "DEPTH";
    char WndTitle[256];
    sprintf(WndTitle, "ZED %s Viewer - (dist max : %d mm) - %d fps - blks_w=%d - blks_h=%d", DisplayModeStr.c_str(),  zed->getDepthClampValue(), fps.get(), blks_w, blks_h);
    glutSetWindowTitle(WndTitle);

    //OpenGL Part
    glDrawBuffer(GL_BACK); //write to both BACK_LEFT & BACK_RIGHT
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //Draw Image Texture in Left Part of Side by Side
    glBindTexture(GL_TEXTURE_2D, imageTex);
    //flip R and B with GLSL Shader
    glUseProgram(program);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 1.0);
    glVertex2f(-1.0, -1.0);
    glTexCoord2f(1.0, 1.0);
    glVertex2f(0.0, -1.0);
    glTexCoord2f(1.0, 0.0);
    glVertex2f(0.0, 1.0);
    glTexCoord2f(0.0, 0.0);
    glVertex2f(-1.0, 1.0);
    glEnd();

    glUseProgram(0);

    //Draw Depth Texture in Right Part of Side by Side
    glBindTexture(GL_TEXTURE_2D, depthTex);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 1.0);
    glVertex2f(0.0, -1.0);
    glTexCoord2f(1.0, 1.0);
    glVertex2f(1.0, -1.0);
    glTexCoord2f(1.0, 0.0);
    glVertex2f(1.0, 1.0);
    glTexCoord2f(0.0, 0.0);
    glVertex2f(0.0, 1.0);
    glEnd();



    high_resolution_clock::time_point t3 = high_resolution_clock::now();
    /*==========  Custom code  ==========*/
    get_depth();

    // Set area of blocks for obstable detection
    if (isMovingForward){
      sens_area = cvt_pxl_to_area(sens_area_pxl_moving);
    }else{
      sens_area = cvt_pxl_to_area(sens_area_pxl);
    }
    analyze_depth_map();
    print_depth_info();
    /*===================================*/
    high_resolution_clock::time_point t4 = high_resolution_clock::now();
    auto duration_cust = duration_cast<milliseconds>( t4 - t3 ).count();
    log_opt.loop_time ? clog << "Custom code: " << duration_cust << "ms\n" : skip() ;


    //swap.
    glutSwapBuffers();
  }

  glutPostRedisplay();

  if (quit) {
    glutDestroyWindow(1);
    delete zed;
    glDeleteShader(shaderF);
    glDeleteProgram(program);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>( t2 - t1 ).count();
  // log loop time or not
  log_opt.loop_time ? clog << "loop: " << duration << "ms\n" : skip();
}

int initZed(const string arg_str)
{
  parseArgs(arg_str);

  int argc = 0;
  char **argv = NULL;
  //init glut
  glutInit(&argc, argv);
  // glutInit(&argc, argv);

  /*Setting up  The Display  */
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  //Configure Window Postion
  glutInitWindowPosition(50, 25);

  //Configure Window Size (w=1280*(500/720)*2)
  glutInitWindowSize(1778, 500);

  //Create Window
  glutCreateWindow("ZED DISPARITY Viewer");

  //init GLEW Library
  glewInit();

  //Setup ZED Camera (construct and Init)
  switch(CameraResolution){
    case 480:
      zed = new Camera(VGA, DefaultFPS);
      break;
    case 720:
      zed = new Camera(HD720, DefaultFPS);
      break;
    case 1080:
      zed = new Camera(HD1080, DefaultFPS);
      break;
  }

  ERRCODE err = zed->init(MODE::PERFORMANCE, 0, true);
  // ERRCODE display
  clog << "ZED Init : " << errcode2str(err) << endl;

  if (err != SUCCESS) {
    delete zed;
    return -1;
  }

  quit = false; // quit draw loop

  //get Image Size
  w = zed->getImageSize().width;
  h = zed->getImageSize().height;

  cvimg.create(h, w, CV_8UC4); // allocate cv image

  aspect_ratio = ((float)h/(float)w);

  // Add sensing mask
  sens_area_pxl.topl = {(int)(w*.15f), (int)(h*.25f)};
  sens_area_pxl.btmr = {(int)(w*.9f), (int)(h*.70f)};
  sens_area_pxl_moving.topl = {(int)(w*.15f), (int)(h*.25f)};
  sens_area_pxl_moving.btmr = {(int)(w*.9f), (int)(h*.55f)};

  // Allocate grids
  num_blks_w = DefaultBlocks;
  update_grids(num_blks_w);

  if (SensingMode == 1)
    dm_type = SENSING_MODE::FULL;
  else
    dm_type = SENSING_MODE::RAW;

  initZed_ready = true;
  objDetected = false;

  objdetectMain();
  // namedWindow("Obj");

  // Setting confidence threshold for depth map
  // reliabilityIdx = 100;
  // zed->setConfidenceThreshold(reliabilityIdx);


  /**********************************/
  /************* OPENGL *************/
  /**********************************/

  cudaError_t err1, err2;

  //Create and Register OpenGL Texture for Image (RGBA -- 4channels)
  glEnable(GL_TEXTURE_2D);
  glGenTextures(1, &imageTex);
  glBindTexture(GL_TEXTURE_2D, imageTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, NULL);
  glBindTexture(GL_TEXTURE_2D, 0);
  err1 = cudaGraphicsGLRegisterImage(&pcuImageRes, imageTex, GL_TEXTURE_2D, cudaGraphicsMapFlagsNone);

  //Create and Register a OpenGL texture for Depth (RGBA- 4 Channels)
  glGenTextures(1, &depthTex);
  glGenTextures(1, &depthTex);
  glBindTexture(GL_TEXTURE_2D, depthTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glBindTexture(GL_TEXTURE_2D, 0);
  err2 = cudaGraphicsGLRegisterImage(&pcuDepthRes, depthTex, GL_TEXTURE_2D, cudaGraphicsMapFlagsNone);

  if (err1!=0 || err2!=0) {
    cerr << "ERR: error init OpenGL\n";
    return -1;
  }

  //Create GLSL fragment Shader for future processing (and here flip R/B)
  shaderF = glCreateShader(GL_FRAGMENT_SHADER); //fragment shader
  const char* pszConstString = strFragmentShad.c_str();
  glShaderSource(shaderF, 1, (const char**) &pszConstString, NULL);

  // compile the shader source code and check
  glCompileShader(shaderF);
  GLint compile_status = GL_FALSE;
  glGetShaderiv(shaderF, GL_COMPILE_STATUS, &compile_status);
  if (compile_status != GL_TRUE) return -2;

  //create the progamm for both w and F Shader
  program = glCreateProgram();
  glAttachShader(program, shaderF);
  //and link + Check
  glLinkProgram(program);
  GLint link_status = GL_FALSE;
  glGetProgramiv(program, GL_LINK_STATUS, &link_status);
  if (link_status != GL_TRUE) return -2;

  glUniform1i(glGetUniformLocation(program, "texImage"), 0);

  /**********************************/
  /*********** OPENGL END ***********/
  /**********************************/

  std::async(detectObject, false);

  //Set Draw Loop
  glutKeyboardFunc(keyboard);
  glutDisplayFunc(draw);
  glutMainLoop();

  return 0;
}




