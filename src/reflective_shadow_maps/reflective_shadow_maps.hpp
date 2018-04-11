#pragma once

#include <iostream>
#include <vector>
#include "camera_app.hpp"
#include "shader.hpp"
#include "geometry_object.hpp"
#include "plane.hpp"

class ReflectiveShadowMapsApp : public CameraApp {
  public:
    ReflectiveShadowMapsApp(const char* tWindowName = "Reflective Shadow maps",
        const int tWindowWidth = 800,
        const int tWindowHeight = 600);
    ~ReflectiveShadowMapsApp();
  private:
    void setUpScene();
    void mainLoopBody();
    void buildGeometry();
    void initRSM(int tWidth = 1024, int tHeight = 1024);
    void initRSMOffsets(int tSamples = 400);
    void initGIMap(int tWidth = 32, int tHeight = 32);
    void initHDR();

    void renderShadowMaps();
    void renderGIMaps();
    void renderScreenScene();
    void performPostProcessing();

    glm::mat4 mLightSpaceMatrix, mProjection, mView;

    Plane mPostProcessingPlane;
    int mShadowmapWidth, mShadowmapHeight;
    int mFirstBounceTextureWidth = 32;
    int mFirstBounceTextureHeight = 32;
    std::vector<GeometryObject*> mScene;
    unsigned int mRSMFBO, mIlluminationFBO, mHdrFBO;
    unsigned int mLightDepthMapTexture, mIlluminationMapTexture, mScreenTexture;
    unsigned int mLowResNormals, mLowResPositions;
    unsigned int mRsmTextures[3];
    std::vector<glm::vec2> mRSMOffsets;
    glm::vec3 mLightPosition = { 4.0f, 1.2f, 3.0f };
    const float MAX_ILLUMINATION_RADIUS = 0.3f;

    Shader mRenderShader = { "./vertex.vert", "./fragment.frag" };
    Shader mRSMShader = { "./rsm_map.vert", "./rsm_map.frag" };
    Shader mFirstBounceIlluminationShader = { "./vertex.vert", "./first_bounce.frag" };
    Shader mPostProcessingShader = { "./post.vert", "./post.frag" };
};
