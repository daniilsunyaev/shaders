#version 330 core

out vec4 FragColor;

in VS_OUT {
  vec3 WorldPos;
  vec2 texCoord;
  vec3 Normal;
  vec3 TangentCameraPos;
  vec3 TangentLightPos;
  vec3 TangentFragPos;
} fs_in;

uniform sampler2D diffuse;
uniform sampler2D relief;

const float PARALLAX_SCALE = 0.03f;
const float MIN_LAYERS = 4;
const float MAX_LAYERS = 20;
// uniform vec3 lightDir;

int numLayers(vec3 V) {
  return int(
      mix(
        MAX_LAYERS,
        MIN_LAYERS,
        abs(V.z)
        )
      );
}

vec2 texOffset(vec3 V) {
  vec3 rayStep = -V * PARALLAX_SCALE / (abs(V.z) * numLayers(V));
  vec3 ray;
  float rayHeight = 0;
  float rayHeightPrev = 0;
  float surfaceHeight = -texture(relief, fs_in.texCoord).a*PARALLAX_SCALE;
  float surfaceHeightPrev = surfaceHeight;

  while(rayHeight > surfaceHeight) {
    ray += rayStep;
    rayHeightPrev = rayHeight;
    rayHeight = ray.z;
    surfaceHeightPrev = surfaceHeight;
    surfaceHeight = -texture(relief, fs_in.texCoord + ray.xy).a * PARALLAX_SCALE;
  }

  float nextSurfaceDistance = rayHeight - surfaceHeight;
  float prevSurfaceDistance = surfaceHeightPrev - rayHeightPrev;
  float rel = abs(nextSurfaceDistance / prevSurfaceDistance);
  vec3 lerpBack = rayStep * nextSurfaceDistance/(prevSurfaceDistance + nextSurfaceDistance);
  return (ray - lerpBack).xy;
}

float calcLighting(vec3 L, vec2 texCoord) {
  int layers = numLayers(L);
  float shadow = 0;

  float rayHeight = -texture(relief, texCoord).a;
  float surfaceHeight = rayHeight;
  vec3 rayStep = L * PARALLAX_SCALE / (abs(L.z) * numLayers(L));
  vec3 ray = vec3(0.0f, 0.0f, rayHeight);

  for(int i=0; i<layers; i++) {
    ray += rayStep;
    surfaceHeight = -texture(relief, texCoord + ray.xy).a;
    float PSF = (surfaceHeight - ray.z)*(1.0f - i/layers);
    shadow = max(shadow, PSF);
  }

  return (1.0f - shadow);
}

void main() {
  vec3 V = normalize(fs_in.TangentCameraPos - fs_in.TangentFragPos);
  vec2 parallaxOffset = texOffset(V);

  vec3 color = texture(diffuse, fs_in.texCoord + parallaxOffset).rgb;
  vec3 L = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);

  float lighting = calcLighting(L, fs_in.texCoord + parallaxOffset);
  FragColor = vec4(color * lighting, 1.0f);
}
