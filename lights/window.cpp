#include "window.h"

#include <iostream>
#include <cstdlib>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glerror.h"
#include "material.h"
#include "light.h"

const char* kPointLightVertexShader="PointLight.vertex.glsl";
const char* kPointLightFragmentShader="PointLight.fragment.glsl";
const char* kSunLightVertexShader="SunLight.vertex.glsl";
const char* kSunLightFragmentShader="SunLight.fragment.glsl";
const char* kSpotLightVertexShader="SpotLight.vertex.glsl";
const char* kSpotLightFragmentShader="SpotLight.fragment.glsl";
const char* kVertexShader="TextureShader.vertex.glsl";
const char* kFragmentShader="TextureShader.fragment.glsl";

const char* kSunTexureFile="sunmap.tga"; //???
const char* kColorTexureFile="uranusmap.tga"; //???
const char* kMoonTexureFile="moonmap.tga"; //???
const char* kSpaceTexureFile="starsmap.tga"; //???

const int kPlaneM = 30;
const int kPlaneN = 40;

const int kTorusM = 40;
const int kTorusN = 30;
const float kTorusR = 2;
const float kTorus_r = 0.75;

const Material  kYellowMaterial={
    {0.2f, 0.2f, 0.2f, 1.0f}, //Ambient
    {1.0f, 1.0f, 0.0f, 1.0f}, //Diffuse
    {0.6f, 0.6f, 0.6f, 1.0f}, //Specular
    {0.0f, 0.0f, 0.0f, 1.0f}, //Emission
    60.0f
  };

const Material kBlueMaterial={
    {0.2f, 0.2f, 0.2f, 1.0f}, //Ambient
    {0.1f, 1.0f, 0.1f, 1.0f}, //Diffuse
    {0.6f, 0.6f, 0.6f, 1.0f}, //Specular
    {0.0f, 0.0f, 0.0f, 1.0f}, //Emission
    90.0f
  };

const Material  kIceMaterial={
    {0.4f, 0.4f, 0.4f, 1.0f}, //Ambient
    {0.95f, 0.95f, 0.99f, 1.0f}, //Diffuse
    {0.99f, 0.99f, 0.99f, 1.0f}, //Specular
    {0.0f, 0.0f, 0.0f, 1.0f}, //Emission
    10.0f
  };

const PointLight kPointLight={
    {0.0f, 7.5f, 3.0f, 1.0f}, //position
    {0.1f, 0.1f, 0.1f, 1.0f}, //ambient
    {1.0f, 1.0f, 1.0f, 1.0f}, //diffuse
    {1.0f, 1.0f, 1.0f, 1.0f}, //specular
    {0.5f, 0.005f, 0.0125f}	//attenuation
};

const SunLight kSunLight={
    {0.0f, 7.5f, 3.0f, 0.0f}, //position
    {0.1f, 0.1f, 0.1f, 1.0f}, //ambient
    {1.0f, 1.0f, 1.0f, 1.0f}, //diffuse
    {1.0f, 1.0f, 1.0f, 1.0f} //specular
};

const SpotLight kSpotLight={
    {0.0f, 7.5f, 3.0f, 1.0f}, //position
    {0.3f, 0.3f, 0.3f, 1.0f}, //ambient
    {1.0f, 1.0f, 1.0f, 1.0f}, //diffuse
    {1.0f, 1.0f, 1.0f, 1.0f}, //specular
    {0.5f, 0.005f, 0.0125f},	//attenuation
    {0.0f, -6.0f, -3.0f},	//direction
    20,			//cutoff, degrees
    20			//exponent
};



Window::Window(const char * title, int width, int height){
    title_ = title;
    width_ = width;
    height_ = height;
    last_time_ = 0;
    tori_.SetInitAngle(15);
    tori_.SetVelocity(-45);
    x_origin_ = y_origin_ = -1;
    current_program_ = POINT_PROGRAM;
}

void Window::Initialize(int major_gl_version, int minor_gl_version){

    InitGlfwOrDie(major_gl_version, minor_gl_version);
    InitGlewOrDie();


    std::cout << "OpenGL initialized: OpenGL version: " << glGetString(GL_VERSION) << " GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    InitTextures();
    InitModels();
    InitPrograms();
    view_matrix_.Translate(0, 0, -10);
    projection_matrix_ = Mat4::CreatePerspectiveProjectionMatrix(60, (float)width_/(float)height_, 0.1f, 100.0f);
    SetViewMatrix();
    SetProjectionMatrix();


    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0.75f, 0.75f, 0.35f, 0.0f);

}

void Window::InitGlfwOrDie(int major_gl_version, int minor_gl_version){
    if ( !glfwInit() ) {
        std::cerr << "ERROR: Could not initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major_gl_version);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor_gl_version);
#ifdef DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
    window_ = glfwCreateWindow(width_, height_, title_, nullptr, nullptr);
    if (!window_){
        std::cerr << "ERROR: Could not create a new rendering window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window_);
}

void Window::InitGlewOrDie(){
    GLenum glew_init_result;
    glewExperimental = GL_TRUE;
    glew_init_result = glewInit();

    if (GLEW_OK != glew_init_result) {
        std::cerr << "Glew ERROR: " << glewGetErrorString(glew_init_result) << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
#ifdef DEBUG
    if(glDebugMessageCallback){
        std::cout << "Register OpenGL debug callback " << std::endl;
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback((GLDEBUGPROC) OpenglCallbackFunction, NULL);
        GLuint unused_ids = 0;
        glDebugMessageControl(GL_DONT_CARE,
            GL_DONT_CARE,
            GL_DONT_CARE,
            0,
            &unused_ids,
            GL_FALSE);
    }
    else
        std::cout << "glDebugMessageCallback not available" << std::endl;
#endif

}

void Window::InitTextures(){
    sun_texture_.Initialize(kSunTexureFile);
    color_texture_.Initialize(kColorTexureFile);
    moon_texture_.Initialize(kMoonTexureFile);
    space_texture_.Initialize(kSpaceTexureFile);
}

void Window::InitModels(){
    tori_.Initialize(kTorusN, kTorusM, kTorusR, kTorus_r);
    tori_.SetTexture(sun_texture_);
    tori_.SetTexturePrim(color_texture_);
    tori_.SetTextureDuo(moon_texture_);
    tori_.SetTextureUnit(GL_TEXTURE0);
    //tori_.SetMaterial(kBlueMaterial); // ?
    //tori_.SetMaterialPrim(kYellowMaterial); // ?
    //tori_.SetMaterialDuo(kIceMaterial); // ?

    plane_.Initialize(kPlaneM, kPlaneN);
    plane_.SetTexture(space_texture_);
    plane_.SetTextureUnit(GL_TEXTURE0);
}

void Window::InitPrograms(){

    //point
    point_program_.Initialize(kPointLightVertexShader, kPointLightFragmentShader);
    point_program_.SetLight(kPointLight);
    point_program_.SetTextureUnit(0);

    //sun
    sun_program_.Initialize(kSunLightVertexShader, kSunLightFragmentShader);
    sun_program_.SetLight(kSunLight);
    sun_program_.SetTextureUnit(0);

    //spot
    spot_program_.Initialize(kSpotLightVertexShader, kSpotLightFragmentShader);
    spot_program_.SetLight(kSpotLight);
    spot_program_.SetTextureUnit(0);

    //shader
    program_.Initialize(kVertexShader, kFragmentShader);
    program_.SetTextureUnit(0);

    glUseProgram(0);
}

void Window::SetViewMatrix()const{
    glUseProgram(point_program_);
    point_program_.SetViewMatrix(view_matrix_);
    glUseProgram(spot_program_);
    spot_program_.SetViewMatrix(view_matrix_);
    glUseProgram(spot_program_);
    spot_program_.SetViewMatrix(view_matrix_);
    glUseProgram(sun_program_);
    sun_program_.SetViewMatrix(view_matrix_);
    glUseProgram(program_);
    program_.SetViewMatrix(view_matrix_);

    glUseProgram(0);
}

void Window::SetProjectionMatrix()const{
    glUseProgram(point_program_);
    point_program_.SetProjectionMatrix(projection_matrix_);
    glUseProgram(spot_program_);
    spot_program_.SetProjectionMatrix(projection_matrix_);
    glUseProgram(sun_program_);
    sun_program_.SetProjectionMatrix(projection_matrix_);
    glUseProgram(program_);
    program_.SetProjectionMatrix(projection_matrix_);

    glUseProgram(0);
}



void Window::Resize(int new_width, int new_height){
    width_ = new_width;
    height_ = new_height;
    projection_matrix_ = Mat4::CreatePerspectiveProjectionMatrix(60, (float)width_/(float)height_, 0.1f, 100.0f);
    SetProjectionMatrix();
    glViewport(0, 0, width_, height_);
}

void Window::KeyEvent(int key, int /*scancode*/, int action, int /*mods*/){
    if(action == GLFW_PRESS){
        switch (key){
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window_, GLFW_TRUE);
            break;
            case GLFW_KEY_LEFT:
              tori_.SlowDown();
            break;
            case GLFW_KEY_RIGHT:
              tori_.SpeedUp();
            break;
            case GLFW_KEY_SPACE:
              tori_.ToggleAnimated();
            break;
            case GLFW_KEY_F1:
              current_program_ = POINT_PROGRAM;
            break;
            case GLFW_KEY_F2:
              current_program_ = SUN_PROGRAM;
            break;
            case GLFW_KEY_F3:
              current_program_ = SPOT_PROGRAM;
            break;
            default:
            break;
        }
    }
    else if(action == GLFW_REPEAT){
        switch (key){
            case GLFW_KEY_LEFT:
              tori_.SlowDown();
            break;
            case GLFW_KEY_RIGHT:
              tori_.SpeedUp();
            break;
            default:
            break;
        }
    }
}

void Window::MouseScroll(double /* xoffset */, double yoffset){
    view_matrix_.Translate(0, 0, -0.25f*yoffset);
    SetViewMatrix();
}

void Window::MouseButton(int button, int action, int /*mods*/) {
    // only start motion if the left button is pressed
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
          // when the button is released
          if (action == GLFW_RELEASE) {
              x_origin_ = -1;
              y_origin_ = -1;
          }
          else  {// state = GLFW_PRESS
              double x_pos, y_pos;
              glfwGetCursorPos(*this, &x_pos, &y_pos);
              x_origin_ = x_pos;
              y_origin_ = y_pos;
          }
    }
}


void Window::MouseMove(double x_pos, double y_pos){	// this will only be true when the left button is down
    float delta_x_angle=0;
    float delta_y_angle=0;
    if (x_origin_ >= 0 && y_origin_ >=0) {
        // update deltaAngle
        delta_x_angle = (x_pos - x_origin_) * 0.1f;
        delta_y_angle = (y_pos - y_origin_) * 0.1f;
        x_origin_=x_pos;
        y_origin_=y_pos;

        // update camera's direction
        view_matrix_.RotateAboutY(delta_x_angle);
        view_matrix_.RotateAboutX(delta_y_angle);
        SetViewMatrix();
   }
}


void Window::Run(void){
    while (!glfwWindowShouldClose(window_)){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        clock_t now = clock();
        if (last_time_ == 0) last_time_ = now;
        tori_.Move((float)(now - last_time_) / CLOCKS_PER_SEC);
        last_time_ = now;

        switch (current_program_){
        case POINT_PROGRAM:
            tori_.Draw(point_program_);
            plane_.Draw(program_);
        break;
        case SUN_PROGRAM:
            tori_.Draw(sun_program_);
            plane_.Draw(program_);
        break;
        case SPOT_PROGRAM:
            tori_.Draw(spot_program_);
            plane_.Draw(program_);
        break;
        }

        glfwSwapBuffers(window_);
        glfwPollEvents();
    }

}
