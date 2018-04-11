#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D map;
const int TEXEL_X = 4;
const int TEXEL_Y = 4;
const int WIDTH = 800;
const int HEIGHT = 600;

void main() {
  vec3 color = vec3(0.0f);
  for(int i=-TEXEL_X/2; i<TEXEL_X/2; i++) {
    for(int j=-TEXEL_Y/2; j<TEXEL_Y/2; j++) {
      vec2 offset = vec2(i,j) * vec2(1/float(WIDTH), 1/float(HEIGHT));

      color += texture(map, TexCoords + offset).rgb;
    }
  }
  color /= TEXEL_X*TEXEL_Y;
  FragColor = vec4(color, 1.0f);
}
