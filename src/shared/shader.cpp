#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "shader.hpp"

Shader::Shader(const GLchar* vertexPath, const GLchar* geometryPath,
    const GLchar* fragmentPath) {

  auto vertex = generateShader(vertexPath, GL_VERTEX_SHADER);
  auto geometry = generateShader(geometryPath, GL_GEOMETRY_SHADER);
  auto fragment = generateShader(fragmentPath, GL_FRAGMENT_SHADER);

  ID = glCreateProgram();
  glAttachShader(ID, vertex);
  glAttachShader(ID, geometry);
  glAttachShader(ID, fragment);
  glLinkProgram(ID);

  int success;
  char infoLog[512];
  glGetProgramiv(ID, GL_LINK_STATUS, &success);
  if(!success) {
    glGetProgramInfoLog(ID, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }

  glDeleteShader(vertex);
  glDeleteShader(geometry);
  glDeleteShader(fragment);
}

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath) {
  auto vertex = generateShader(vertexPath, GL_VERTEX_SHADER);
  auto fragment = generateShader(fragmentPath, GL_FRAGMENT_SHADER);

  ID = glCreateProgram();
  glAttachShader(ID, vertex);
  glAttachShader(ID, fragment);
  glLinkProgram(ID);
  char infoLog[512];
  int success;

  glGetProgramiv(ID, GL_LINK_STATUS, &success);
  if(!success) {
    glGetProgramInfoLog(ID, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }

  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

unsigned int Shader::generateShader(const GLchar* path, GLenum shaderType) {
  std::string shaderCode;
  std::ifstream shaderFile;
  shaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
  try {
    shaderFile.open(path);
    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();
    shaderCode = shaderStream.str();
  } catch(std::ifstream::failure e) {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
  }

  const char* shaderCodeCstr = shaderCode.c_str();

  unsigned int shader;
  int success;
  char infoLog[512];

  shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, &shaderCodeCstr, NULL);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if(!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cout << "from " << path << ":" << std::endl;
    std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
  };

  return shader;
}

void Shader::use() {
  glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &value) const {
  glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const { 
  glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
}

void Shader::setVec3(const std::string &name, float x, float y, float z) const { 
  glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); 
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value) const { 
  glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
}
