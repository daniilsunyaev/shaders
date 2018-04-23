#pragma once

#include <iostream>
#include <vector>
#include "camera_app.hpp"
#include "shader.hpp"
#include "geometry_object.hpp"
#include "plane.hpp"
#include "cube.hpp"
#include "sphere.hpp"

class PBRApp : public CameraApp {
  public:
    PBRApp(const char* tWindowName = "PBR",
        const int tWindowWidth = 800,
        const int tWindowHeight = 600);
    ~PBRApp();
  private:
    void setUpScene();
    void mainLoopBody();

    void buildCubemapFromEquirectmap(unsigned int tCubemapTextureId, int tSize, char const* tEquirectmapPath);
    void buildEnvCubemap();
    void buildIrradianceCubemap();
    void buildPrefilteredEnvMap();
    void buildBRDFLUT();

    void buildGeometry();

    void renderEnvCubemap();
    void renderIrradianceCubemap();

    unsigned int mCaptureFBO, mCaptureRBO;
    unsigned int mEnvCubemapTexture;
    unsigned int mIrradianceCubemapTexture;
    unsigned int mPrefilteredEnvMapTexture;
    unsigned int mBRDFLUTTexture;

    unsigned int mAlbedo, mMetallic, mRoughness, mAo, mNormal;
    const glm::mat4 mCaptureViews[6] = {
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };
    glm::mat4 mCaptureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

    glm::mat4 mProjection, mView;

    std::vector<GeometryObject*> mScene;

    Cube mSkyboxCube;
    Plane mPostProcessingPlane;
    Sphere mBall;

    const char* ENV_MAP_PATH = "./Ditch_River/Ditch-River_2k.hdr";
    const char* IRRADIANCE_MAP_PATH = "./Ditch_River/Ditch-River_Env.hdr";
    //const char* ENV_MAP_PATH = "./Factory_Catwalk/Factory_Catwalk_2k.hdr";
    //const char* IRRADIANCE_MAP_PATH = "./Factory_Catwalk/Factory_Catwalk_Env.hdr";

    Shader mCubemapSetupShader = { "./cube_setup.vert", "./cube_setup.frag" };
    Shader mSkyboxShader = { "./skybox.vert", "./skybox.frag" };
    Shader mPrefilterShader = { "./skybox.vert", "./prefilter_convolution.frag" };
    Shader mBRDFShader = { "./post.vert", "./brdf_lut.frag" };
    Shader mPBRShader = { "./pbr.vert", "./pbr.frag" };
    Shader mPostShader = { "./post.vert", "./post.frag" };
};
