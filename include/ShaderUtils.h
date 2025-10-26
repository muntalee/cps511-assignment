#pragma once
#include <string>
#include <GL/glew.h>

struct ShaderProgram
{
  GLuint program = 0;
  GLint locModel = -1, locView = -1, locProj = -1, locNormalMat = -1;
  GLint locLightPos = -1, locViewPos = -1;
  GLint locMatAmbient = -1, locMatDiffuse = -1, locMatSpecular = -1, locMatShininess = -1;
  GLint attribPos = -1, attribNormal = -1;
};

bool LoadTextFile(const std::string &path, std::string &out);
GLuint CompileShaderFromFile(GLenum type, const std::string &path, std::string *err = nullptr);
bool LinkProgram(GLuint vs, GLuint fs, GLuint &outProgram, std::string *err = nullptr);
ShaderProgram MakeGroundProgram(const std::string &vsPath, const std::string &fsPath, std::string *err = nullptr);
