#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D map;
const float gamma = 2.2f;

void main()
{             
  vec3 color = texture(map, TexCoords).rgb;

  FragColor = vec4(color, 1.0f);
}  