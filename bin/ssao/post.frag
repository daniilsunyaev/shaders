#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D map;
// const float gamma = 2.2f;

void main() {
  vec3 color = texture(map, TexCoords).rgb;
  // color = vec3(1.0) - exp(-color * 1.2f);
  // color =  pow(color, vec3(1.0 / gamma));
  FragColor = vec4(color, 1.0f);
}
