#version 330 core

in VS_OUT {
  vec3 WorldPos;
  vec2 texCoord;
  vec3 Normal;
} fs_in;

layout (location = 0) out vec3 rPosition;
layout (location = 1) out vec3 rNormal;
layout (location = 2) out vec3 rFlux;

uniform sampler2D diffuse;
uniform vec3 L;
const float lightPower = 5;
const float scatteringCoefficient = 2e-6f;
// for sake of simplicity this is constant
// instead of vertex or texture data

void main()
{
  vec3 color = vec3(texture(diffuse, fs_in.texCoord));

  rPosition = fs_in.WorldPos;
  rNormal = fs_in.Normal;
  rFlux = dot(normalize(L), fs_in.Normal) * color * scatteringCoefficient * lightPower;
}
