#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

const int SAMPLES = 64;
const int ROTATION_SAMPLES_X = 4;
const int ROTATION_SAMPLES_Y = 4;
const float RADIUS = 0.3;
const float BIAS = 0.005;

const int WIDTH = 800;
const int HEIGHT = 600;

const vec2 ROTATION_SCALE = vec2(float(WIDTH)/ROTATION_SAMPLES_X,
    float(HEIGHT)/ROTATION_SAMPLES_Y);

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D rotationVectors;

uniform vec3 ssaoKernel[SAMPLES];
uniform mat4 projection;


void main() {
  vec3 randomVec = texture(rotationVectors, TexCoords*ROTATION_SCALE).rgb;
  vec3 normal = texture(gNormal, TexCoords).rgb;
  vec3 position = texture(gPosition, TexCoords).rgb;

  vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
  vec3 bitangent = cross(normal, tangent);
  mat3 TBN = mat3(tangent, bitangent, normal);

  float occlusion = 0.0f;
  for(int i=0; i<SAMPLES; i++) {
    vec4 sample = vec4(position + RADIUS * TBN * ssaoKernel[i], 1.0f);
    vec4 projectedSample = projection * sample;
    projectedSample.xyz /= projectedSample.w;
    projectedSample.xyz = projectedSample.xyz * 0.5f + 0.5f; // to screen coordinates

    vec3 obstaclePosition = texture(gPosition, projectedSample.xy).xyz;

    // discard samples that hit distant obstacles
    float sampleWeight = smoothstep(0.0f, 1.0f,
        1 - 0.5f * length(position.z - obstaclePosition.z)/RADIUS);

    bool occluded = bool(sample.z + BIAS < obstaclePosition.z);

    if(occluded) occlusion += sampleWeight;
  }
  occlusion /= float(SAMPLES);

  FragColor = vec4(vec3(occlusion), 1.0f);
}
