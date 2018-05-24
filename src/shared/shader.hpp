#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
  public:
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
    Shader(const GLchar* vertexPath, const GLchar* geometryPath,
        const GLchar* fragmentPath);
    void use();
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMat4(const std::string &name, const glm::mat4 &value) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setVec2(const std::string &name, const glm::vec2 &value) const;
  private:
    unsigned int ID;
    unsigned int generateShader(const GLchar* path, GLenum shaderType);
};
