#version 330 core

layout (location=0) in vec3 aPos; 
layout (location=1) in vec3 aNormal; 
layout (location=2) in vec2 aTexCoord; 
layout (location=3) in vec3 aTangent; 
layout (location=4) in vec3 aBitangent; 

out VS_OUT {
  vec3 WorldPos;
  vec2 texCoord;
  vec3 Normal;
  vec3 TangentCameraPos;
  vec3 TangentLightPos;
  vec3 TangentFragPos;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPosition;
uniform vec3 lightPosition;

void main() {
  vs_out.WorldPos = vec3(model * vec4(aPos, 1.0f));
  vs_out.texCoord = aTexCoord;

  mat3 model3 = mat3(model);
  mat3 normalMatrix = transpose(inverse(model3));
  vs_out.Normal = normalize(normalMatrix * aNormal);

  vec3 Tangent = vec3(model3 * aTangent);
  vec3 Bitangent = vec3(model3 * aBitangent);
  mat3 WorldToTangent = transpose(mat3(Tangent, Bitangent, vs_out.Normal));

  vs_out.TangentCameraPos = WorldToTangent * cameraPosition;
  vs_out.TangentLightPos = WorldToTangent * lightPosition;
  vs_out.TangentFragPos = WorldToTangent * vs_out.WorldPos;

  gl_Position = projection * view * model * vec4(aPos, 1.0);
}
