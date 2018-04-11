#version 330 core

out vec4 FragColor;

in VS_OUT {
  vec3 WorldPos;
  vec2 texCoord;
  vec3 Normal;
} fs_in;

uniform sampler2D diffuse;

// uniform vec3 lightDir;

void main() {
  // vec3 L = normalize(lightDir);
  vec4 color = texture(diffuse, fs_in.texCoord);

  vec4 ambient = vec4(0.1f, 0.1f, 0.1f, 1.0f);

  //float litDegree = max(dot(L, fs_in.Normal), 0.0f) * lightPower;
  vec4 diffuse = 0.9f * color; // * litDegree;

  FragColor = ambient + diffuse;
}
