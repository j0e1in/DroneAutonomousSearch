/**
* Cmd line Arg(optional): SVO path
*/

//Standard includes
#include <stdio.h>
#include <string.h>
#include <ctime>

#include <chrono> //high_resolution_clock
#include "env_eval.h"
#include "zed.h"
#include "FPS.h"

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
bool initZed_ready = false;
int reliabilityIdx;
Fps fps;

//Here is the very simple fragment shader for flipping Red and Blue
std::string strFragmentShad = ("uniform sampler2D texImage;\n"
      " void main() {\n"
      " vec4 color = texture2D(texImage, gl_TexCoord[0].st);\n"
      " gl_FragColor = vec4(color.b, color.g, color.r, color.a);\n}");



bool is_initZed_ready(){
  return (initZed_ready == true);
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

  glRasterPos2i(x - (w_wnd * 0.035), y + (h_wnd * 0.02));
  for (unsigned int i = 0; i < String.size(); i++)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, String[i]);

  glPopAttrib();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glDisable(GL_TEXTURE_2D);
  float r_ = 0.01;
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
  switch(DepthInfoID){

    case 1: // print distance

      for (i = 0; i < blks_h; ++i){
        for (j = 0; j < blks_w; ++j){
          ostringstream ostr;
          if (dist_grid[i][j].dist > 0) {
            ostr << (float)dist_grid[i][j].dist / 100. << "m";
          } else {
            ostr << "NaN";
          }

          printtext(r_grid[i][j].x, r_grid[i][j].y, ostr.str().c_str());
          glColor3f(1.0f, 1.0f, 1.0f);
        }
      }

      break;

    case 2: // print area classification

      for (i = 0; i < blks_h; ++i){
        for (j = 0; j < blks_w; ++j){
          ostringstream ostr;
          // ostr << "[" << validity_grid[i][j].valid << "@"
          //      << valid_classfied.area_grid[i][j].area_no << "]";
          ostr << "[" << validity_grid[i][j].valid << "@"
               << (float)dist_grid[i][j].dist / 100. << "]";

          printtext(r_grid[i][j].x, r_grid[i][j].y, ostr.str().c_str());
          glColor3f(1.0f, 1.0f, 1.0f);
        }
      }
      break;

    default: break;
  }
  return;
}

// Retreive distance of each block, store it to dist_grid
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
        depth_clamp = depth_click / 10.;

        if (depth_clamp > 0){
          dist_grid[i][j].dist = depth_clamp; // Save distance, in 'cm'
        }
        else{
          dist_grid[i][j].dist = -1; // Save distance as negative on failure
        }
      }
    }
  }
  return;
}

//glut main loop : grab --> extract GPU Mat --> send to OpenGL and quad.
void draw() {
  high_resolution_clock::time_point t1 = high_resolution_clock::now();

  int res = zed->grab(dm_type, true, true);

  if (res == 0)
  {
    fps.update();

    // Map GPU Ressource for Image
    // With Gl texture, we have to use the cudaGraphicsSubResourceGetMappedArray cuda functions. It will link the gl texture with a cuArray.
    // Then, we just have to copy our GPU Buffer to the CudaArray (D2D copy).
    // Between , you can do some other GPU stuffs.
    Mat gpuImage = zed->getView_gpu(VIEW_MODE::STEREO_LEFT);
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
    analyze_depth_map();
    print_depth_info();
    /*===================================*/
    high_resolution_clock::time_point t4 = high_resolution_clock::now();
    auto duration_cust = duration_cast<milliseconds>( t4 - t3 ).count();
    cout << "Custom code: " << duration_cust << "ms\n";



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
  cout << "loop: " << duration << "ms\n";
}

int initZed(int argc, char **argv)
{
  //init glut
  glutInit(&argc, argv);

  /*Setting up  The Display  */
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  //Configure Window Postion
  glutInitWindowPosition(50, 25);

  //Configure Window Size (w=h*vedio_aspect_ratio, h)
  glutInitWindowSize(1778, 500);

  //Create Window
  glutCreateWindow("ZED DISPARITY Viewer");

  //init GLEW Library
  glewInit();

  //Setup our ZED Camera (construct and Init)
  if (argc == 1) { // Use in Live Mode
    switch(CameraResolution){
      case 480: // VGA
        zed = new Camera(VGA, DefaultFPS);
        break;
      case 720:
        zed = new Camera(HD720, DefaultFPS);
        break;
      case 1080:
        zed = new Camera(HD1080, DefaultFPS);
        break;
    }
  } else {// Use in SVO playback mode
    zed = new Camera(argv[1]);
  }

  ERRCODE err = zed->init(MODE::PERFORMANCE, 0, true, false);
  // ERRCODE display
  std::cout << "ZED Init : " << errcode2str(err) << std::endl;
  if (err != SUCCESS) {
    delete zed;
    printf("ERR: ZED init failed.\n");
    return -1;
  }else{
    initZed_ready = true;
  }

  quit = false; // quit draw loop

  //get Image Size
  w = zed->getImageSize().width;
  h = zed->getImageSize().height;
  aspect_ratio = ((float)h/(float)w);

  num_blks_w = DefaultBlocks;
  update_grids(num_blks_w);

  if (SensingMode == 1)
    dm_type = SENSING_MODE::FULL;
  else
    dm_type = SENSING_MODE::RAW;

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
    printf("ERR: error in OpenGL in initZed\n");
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

  //Set Draw Loop
  glutKeyboardFunc(keyboard);
  glutDisplayFunc(draw);
  glutMainLoop();

  return 0;
}



