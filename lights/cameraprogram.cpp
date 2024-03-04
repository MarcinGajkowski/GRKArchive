#include <iostream>
using namespace std;

#include "cameraprogram.h"

#include <GLFW/glfw3.h>


void CameraProgram::Initialize(const char *vertex_shader_file, const char *fragment_shader_file){

    BaseProgram::Initialize(vertex_shader_file, fragment_shader_file);
    projection_matrix_location_ = GetUniformLocationOrDie("projection_matrix");
    view_matrix_location_ = GetUniformLocationOrDie("view_matrix");
}

void CameraProgram::SetProjectionMatrix(const Mat4 & matrix) const{
    glUniformMatrix4fv(projection_matrix_location_, 1, GL_FALSE, matrix);
}

void CameraProgram::SetViewMatrix(const Mat4 & matrix) const{
    glUniformMatrix4fv(view_matrix_location_, 1, GL_FALSE, matrix);
}
