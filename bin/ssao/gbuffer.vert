#version 330 core

layout (location=0) in vec3 aPos;
layout (location=1) in vec3 aNormal;
layout (location=2) in vec2 aTexCoord;

out GBUF_VS_OUT {
  vec3 ViewPos;
  vec2 texCoord;
  vec3 ViewNormal;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  vs_out.ViewPos = vec3(view * model * vec4(aPos, 1.0f));
  vs_out.texCoord = aTexCoord;

  mat3 normalViewMatrix = transpose(inverse(mat3(view * model)));
  vs_out.ViewNormal = normalize(normalViewMatrix * aNormal);

  gl_Position = projection * view * model * vec4(aPos, 1.0);
}
