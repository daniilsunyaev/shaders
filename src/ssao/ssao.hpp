#pragma once

#include <iostream>
#include <vector>
#include "camera_app.hpp"
#include "shader.hpp"
#include "geometry_object.hpp"
#include "plane.hpp"

class SSAOApp : public CameraApp {
  public:
    SSAOApp(const char* tWindowName = "SSAO",
        const int tWindowWidth = 800,
        const int tWindowHeight = 600);
    ~SSAOApp();
  private:
    void setUpScene();
    void mainLoopBody();
    void buildGeometry();
    void initGbuffer();
    void initRawSSAO();
    void initBluredSSAO();
    void initRandomizedLookupHemisphere();

    unsigned int mGbufferFBO, mRawSSAOFBO, mBluredSSAOFBO;
    unsigned int mGtextures[3], mRawSSAOtexture, mBluredSSAOtexture;
    unsigned int mSSAORotationVectorsTexture;
    const int AO_SAMPLES = 64;
    const int ROTATION_TEXTURE_SIZE = 4;
    glm::mat4 mProjection, mView;

    Plane mPostProcessingPlane;
    std::vector<GeometryObject*> mScene;

    glm::vec3 mLightDirection = { 4.0f, 1.2f, 3.0f };

    Shader mRenderShader = { "./vertex.vert", "./fragment.frag" };
    Shader mGbufferShader = { "./gbuffer.vert", "./gbuffer.frag" };
    Shader mSSAOShader = { "./post.vert", "./ssao.frag" };
    Shader mBlurShader = { "./post.vert", "./blur.frag" };
    Shader mPostProcessingShader = { "./post.vert", "./post.frag" };
    Shader mIlluminationShader = { "./post.vert", "./illumination.frag" };
};
