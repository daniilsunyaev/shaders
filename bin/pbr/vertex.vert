#version 330 core

layout (location=0) in vec3 aPos; 
layout (location=1) in vec3 aNormal; 
layout (location=2) in vec2 aTexCoord; 

out VS_OUT {
  vec3 WorldPos;
  vec2 texCoord;
  vec3 Normal;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  vs_out.WorldPos = vec3(model * vec4(aPos, 1.0f));
  vs_out.texCoord = aTexCoord;

  mat3 normalMatrix = transpose(inverse(mat3(model)));
  vs_out.Normal = normalize(normalMatrix * aNormal);

  gl_Position = projection * view * model * vec4(aPos, 1.0);
}