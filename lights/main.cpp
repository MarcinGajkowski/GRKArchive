#include "window.h"

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>



const int kMajorGLVersion = 4;
const int kMinorGLVersion = 3;

static Window window("Postawy OpenGL", 800, 600);

/*** Callbacks   ***/
void Resize (GLFWwindow* /*window*/, int new_width, int new_height){
    window.Resize(new_width, new_height);
}

void KeyPressed(GLFWwindow* /*window*/, int key, int scancode, int action, int mods){
    window.KeyEvent(key, scancode, action, mods);
}

void MouseScroll(GLFWwindow* /*window*/, double xoffset, double yoffset){
    window.MouseScroll(xoffset, yoffset);
}

void MouseButton(GLFWwindow* /*window*/, int button, int action, int mods){
    window.MouseButton(button, action, mods);
}

void MouseMove(GLFWwindow* /*window*/,  double x_pos, double y_pos){
    window.MouseMove(x_pos, y_pos);
}

/*******************/



int main(void){
    window.Initialize(kMajorGLVersion, kMinorGLVersion);
    glfwSetWindowSizeCallback(window, Resize);
    glfwSetKeyCallback(window, KeyPressed);
    glfwSetScrollCallback(window, MouseScroll);
    glfwSetMouseButtonCallback(window, MouseButton);
    glfwSetCursorPosCallback(window, MouseMove);

    window.Run();
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

