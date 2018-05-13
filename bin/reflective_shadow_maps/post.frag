#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D map;
const float gamma = 2.2f;

void main()
{             
  vec3 color = texture(map, TexCoords).rgb;
  //color = vec3(1.0) - exp(-color * 1.8f);
  //color = color/(vec3(1.0f)+color);
  color = vec3(1.0) - exp(-color * 1.2f);
  float linLimit = 0.03;
  float a  = 0.05;
  if(length(color) < linLimit) {
    color = ((1 + a)*pow(vec3(linLimit), vec3(1.0 / gamma)) - a) * color / linLimit;
  } else {
    color =  (1 + a)*pow(color, vec3(1.0 / gamma)) - a;
  }
  FragColor = vec4(color, 1.0f);
}  
