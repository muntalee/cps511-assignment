#include "ShaderUtils.h"
#include <fstream>
#include <sstream>

bool LoadTextFile(const std::string &path, std::string &out)
{
  std::ifstream in(path, std::ios::in);
  if (!in)
    return false;
  std::ostringstream ss;
  ss << in.rdbuf();
  out = ss.str();
  return true;
}

GLuint CompileShaderFromFile(GLenum type, const std::string &path, std::string *err)
{
  std::string src;
  if (!LoadTextFile(path, src))
  {
    if (err)
      *err = "Failed to read: " + path;
    return 0;
  }
  GLuint sh = glCreateShader(type);
  const char *csrc = src.c_str();
  glShaderSource(sh, 1, &csrc, nullptr);
  glCompileShader(sh);
  GLint ok = GL_FALSE;
  glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
  if (!ok)
  {
    GLint len = 0;
    glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);
    std::string log(len, '\0');
    glGetShaderInfoLog(sh, len, nullptr, &log[0]);
    if (err)
      *err = "Compile error in " + path + ":\n" + log;
    glDeleteShader(sh);
    return 0;
  }
  return sh;
}

bool LinkProgram(GLuint vs, GLuint fs, GLuint &outProgram, std::string *err)
{
  outProgram = glCreateProgram();
  glAttachShader(outProgram, vs);
  glAttachShader(outProgram, fs);
  // Bind attribute locations before linking (so VBO layout is stable)
  glBindAttribLocation(outProgram, 0, "aPos");
  glBindAttribLocation(outProgram, 1, "aNormal");
  glLinkProgram(outProgram);
  GLint ok = GL_FALSE;
  glGetProgramiv(outProgram, GL_LINK_STATUS, &ok);
  if (!ok)
  {
    GLint len = 0;
    glGetProgramiv(outProgram, GL_INFO_LOG_LENGTH, &len);
    std::string log(len, '\0');
    glGetProgramInfoLog(outProgram, len, nullptr, &log[0]);
    if (err)
      *err = "Link error:\n" + log;
    glDeleteProgram(outProgram);
    outProgram = 0;
    return false;
  }
  return true;
}

ShaderProgram MakeGroundProgram(const std::string &vsPath, const std::string &fsPath, std::string *err)
{
  ShaderProgram sp;
  GLuint vs = CompileShaderFromFile(GL_VERTEX_SHADER, vsPath, err);
  if (!vs)
    return sp;
  GLuint fs = CompileShaderFromFile(GL_FRAGMENT_SHADER, fsPath, err);
  if (!fs)
  {
    glDeleteShader(vs);
    return sp;
  }
  GLuint prog = 0;
  if (!LinkProgram(vs, fs, prog, err))
  {
    glDeleteShader(vs);
    glDeleteShader(fs);
    return sp;
  }
  glDeleteShader(vs);
  glDeleteShader(fs);
  sp.program = prog;

  sp.attribPos = 0; // we bound these pre-link
  sp.attribNormal = 1;

  sp.locModel = glGetUniformLocation(prog, "uModel");
  sp.locView = glGetUniformLocation(prog, "uView");
  sp.locProj = glGetUniformLocation(prog, "uProj");
  sp.locNormalMat = glGetUniformLocation(prog, "uNormalMatrix");
  sp.locLightPos = glGetUniformLocation(prog, "uLightPos");
  sp.locViewPos = glGetUniformLocation(prog, "uViewPos");
  sp.locMatAmbient = glGetUniformLocation(prog, "uMat.ambient");
  sp.locMatDiffuse = glGetUniformLocation(prog, "uMat.diffuse");
  sp.locMatSpecular = glGetUniformLocation(prog, "uMat.specular");
  sp.locMatShininess = glGetUniformLocation(prog, "uMat.shininess");
  return sp;
}
