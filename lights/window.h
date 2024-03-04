#ifndef WINDOW_H
#define WINDOW_H

#include <cstring>
#include <ctime>


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "tori.h"
#include "plane.h"
#include "pointlightprogram.h"
#include "sunlightprogram.h"
#include "spotlightprogram.h"
#include "matma.h"
#include "texture.h"

class Window{
 public:
    Window(const char*, int, int);
    void Initialize(int major_gl_version, int minor_gl_version);
    void Resize(int new_width, int new_height);
    void KeyEvent(int key, int scancode, int action, int mods);
    void MouseScroll(double xoffset, double yoffset);
    void MouseButton(int button, int action, int mods);
    void MouseMove(double x_pos, double y_pos);
    void Run(void);
    operator GLFWwindow*(){return window_;}
 private:
    int width_;
    int height_;
    const char* title_;
    GLFWwindow* window_;

    Tori tori_;
    Plane plane_;

    PointLightProgram point_program_;
    SpotLightProgram spot_program_;
    SunLightProgram sun_program_;
    TextureCameraProgram  program_;

    enum {POINT_PROGRAM, SUN_PROGRAM, SPOT_PROGRAM} current_program_;

    Texture sun_texture_;
    Texture moon_texture_;
    Texture color_texture_;
    Texture space_texture_;

    clock_t last_time_;
    int x_origin_;
    int y_origin_;

    Mat4 view_matrix_;
    Mat4 projection_matrix_;

    void InitModels();
    void InitPrograms();
    void InitTextures();

    void SetViewMatrix() const;
    void SetProjectionMatrix() const;

    void InitGlfwOrDie(int major_gl_version,
                       int minor_gl_version);
    void InitGlewOrDie();
};


#endif // WINDOW_H

