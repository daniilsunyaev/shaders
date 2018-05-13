#version 330 core

layout (location = 0) out vec3 illumination;
layout (location = 1) out vec3 position;
layout (location = 2) out vec3 normal;

in VS_OUT {
  vec3 WorldPos;
  vec2 texCoord;
  vec3 Normal;
  vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D rPosition;
uniform sampler2D rNormal;
uniform sampler2D rFlux;

const int SAMPLES = 300;
uniform vec2 offsetSamples[SAMPLES];
uniform float illuminationArea;

vec3 calculate_first_bounces() {
  vec3 projCoords = fs_in.FragPosLightSpace.xyz * 0.5f + 0.5f; // to [0,1] range
  float fragmentDepth = projCoords.z;

  vec3 color = vec3(0.0f, 0.0f, 0.0f);
  for(int i=0; i<SAMPLES; i++) {
    vec2 emitterCoords = projCoords.xy + offsetSamples[i];

    vec3 flux = texture(rFlux, emitterCoords).rgb;
    vec3 emitterPos = texture(rPosition, emitterCoords).xyz;
    vec3 emitterNormal = normalize(texture(rNormal, emitterCoords).xyz);
    vec3 emitterDir = emitterPos - fs_in.WorldPos;

    float quadDistance = max(0.001f, dot(emitterDir, emitterDir));
    // soften up bright pixels in the corners a bit

    color += flux
      * max(0, dot(emitterNormal, -emitterDir))
      * max(0, dot(fs_in.Normal, emitterDir))
      / pow(quadDistance, 2.0f);
  }
  color *= illuminationArea/float(SAMPLES);
  return color;
}

void main() {
  vec3 color = calculate_first_bounces();

  illumination = color;
  position = fs_in.WorldPos;
  normal = fs_in.Normal;
}
