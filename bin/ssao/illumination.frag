#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D SSAO;
uniform vec3 viewLightDirection;

void main() {
  vec3 normal = texture(gNormal, TexCoords).rgb;
  vec3 albedo = texture(gAlbedo, TexCoords).rgb;
  float occlusion = texture(SSAO, TexCoords).r;

  float litDegree = max(0, dot(normal, normalize(viewLightDirection)));
  vec3 diffuse = 0.3 * litDegree * albedo;
  vec3 ambient = 0.7 * (1 - occlusion) * albedo;
  vec3 color = diffuse + ambient;

  FragColor = vec4(color, 1.0f);
}
