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
  mat3 TangentToWorld;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 camPos;

void main() {
  vs_out.WorldPos = vec3(model * vec4(aPos, 1.0f));
  vs_out.texCoord = aTexCoord;

  mat3 model3 = mat3(model);

  mat3 normalMatrix = transpose(inverse(model3));
  vs_out.Normal = normalize(normalMatrix * aNormal);

  vs_out.TangentToWorld = mat3(model3 * aTangent, model3 * aBitangent, vs_out.Normal);

  gl_Position = projection * view * model * vec4(aPos, 1.0);
}
