#include "ShaderUtils.h"
#include <fstream>
#include <sstream>

// utility functions for loading and compiling opengl shaders

// load a text file into out, return true on success
bool LoadTextFile(const std::string &path, std::string &out)
{
  // open file for reading
  std::ifstream in(path, std::ios::in);
  if (!in)
    return false;
  // read entire file into a stringstream then copy to out
  std::ostringstream ss;
  ss << in.rdbuf();
  out = ss.str();
  return true;
}

// compile a shader from a file path and return the shader object (or 0 on error)
// if err is provided, fill it with a human-readable error message
GLuint CompileShaderFromFile(GLenum type, const std::string &path, std::string *err)
{
  std::string src;
  if (!LoadTextFile(path, src))
  {
    if (err)
      *err = "Failed to read: " + path;
    return 0;
  }

  // create shader object and set source
  GLuint sh = glCreateShader(type);
  const char *csrc = src.c_str();
  glShaderSource(sh, 1, &csrc, nullptr);

  // compile and check status
  glCompileShader(sh);
  GLint ok = GL_FALSE;
  glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
  if (!ok)
  {
    // get compile log length and retrieve the log
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

// link vertex and fragment shaders into a program
// returns true on success and sets outProgram, false on error
bool LinkProgram(GLuint vs, GLuint fs, GLuint &outProgram, std::string *err)
{
  outProgram = glCreateProgram();
  // attach compiled shader objects
  glAttachShader(outProgram, vs);
  glAttachShader(outProgram, fs);

  // bind attribute locations before linking so vbo layout is stable
  glBindAttribLocation(outProgram, 0, "aPos");
  glBindAttribLocation(outProgram, 1, "aNormal");

  // link program and check for link errors
  glLinkProgram(outProgram);
  GLint ok = GL_FALSE;
  glGetProgramiv(outProgram, GL_LINK_STATUS, &ok);
  if (!ok)
  {
    // get link log and report it
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

// helper to create a shader program used for ground rendering
// compiles vertex + fragment shaders, links them, and fetches attribute/uniform locations
ShaderProgram MakeGroundProgram(const std::string &vsPath, const std::string &fsPath, std::string *err)
{
  ShaderProgram sp;

  // compile vertex shader
  GLuint vs = CompileShaderFromFile(GL_VERTEX_SHADER, vsPath, err);
  if (!vs)
    return sp;

  // compile fragment shader
  GLuint fs = CompileShaderFromFile(GL_FRAGMENT_SHADER, fsPath, err);
  if (!fs)
  {
    glDeleteShader(vs);
    return sp;
  }

  // link into program
  GLuint prog = 0;
  if (!LinkProgram(vs, fs, prog, err))
  {
    glDeleteShader(vs);
    glDeleteShader(fs);
    return sp;
  }

  // shaders no longer needed once linked
  glDeleteShader(vs);
  glDeleteShader(fs);

  sp.program = prog;

  // attribute indices we bound before linking
  sp.attribPos = 0;
  sp.attribNormal = 1;

  // cache uniform locations for faster use later
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
