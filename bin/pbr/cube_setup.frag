#version 330 core
out vec4 FragColor;

in vec3 cubeLocalPos;

uniform sampler2D equirect_map;

const float PI = 3.141596;

vec2 SampleSphericalMap(vec3 v) {
  float latitude = atan(v.z, v.x);
  float longitude = asin(v.y);
  latitude /= (2*PI); // to [-0.5, 0.5]
  longitude /= (PI); // to [-0.5, 0.5]
  vec2 sphereUV = vec2(latitude + 0.5f, longitude + 0.5f); // to [0.0, 1.0]
  return sphereUV;
}

void main() {
  vec2 uv = SampleSphericalMap(normalize(cubeLocalPos));
  vec3 color = texture(equirect_map, uv).rgb;

  FragColor = vec4(color, 1.0);
}
