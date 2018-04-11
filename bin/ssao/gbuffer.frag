#version 330 core

in GBUF_VS_OUT {
  vec3 ViewPos;
  vec2 texCoord;
  vec3 ViewNormal;
} fs_in;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;

uniform sampler2D diffuse;

void main() {
  vec4 color = texture(diffuse, fs_in.texCoord);

  gPosition = fs_in.ViewPos;
  gNormal = fs_in.ViewNormal;
  gAlbedo = vec3(color);
}
