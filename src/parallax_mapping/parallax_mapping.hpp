#pragma once

#include <iostream>
#include <vector>
#include "camera_app.hpp"
#include "shader.hpp"
#include "geometry_object.hpp"
#include "plane.hpp"
#include "sphere.hpp"

class ParallaxApp : public CameraApp {
  public:
    ParallaxApp(const char* tWindowName = "Parallax Occlusion Mapping",
        const int tWindowWidth = 800,
        const int tWindowHeight = 600);
    ~ParallaxApp();
  private:
    void setUpScene();
    void mainLoopBody();

    void buildGeometry();
    void buildPolygon();
    void buildLight(glm::vec3 tPosition);

    glm::mat4 mProjection, mView;

    Plane mParallaxMappedPolygon;
    Sphere mLight;

    char const* DIFFUSE_TEXTURE_PATH = "./rock.jpg";
    char const* RELIEF_TEXTURE_PATH = "./rock_relief.png";
    unsigned int mDiffuseTexture, mReliefTexture;

    Shader mParallaxMappingShader = { "./parallax.vert", "./parallax.frag" };
    Shader mLightShader = { "./light.vert", "./light.frag" };
};
