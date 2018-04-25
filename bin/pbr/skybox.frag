#version 330 core
out vec4 FragColor;

in vec3 localPos;

uniform samplerCube skybox;

void main() {
  vec3 envColor = texture(skybox, localPos).rgb;

  envColor = vec3(1.0) - exp(-envColor * 1.5f);
  envColor = pow(envColor, vec3(1.0/1.6)); // gamma was specified in ibl file

  FragColor = vec4(envColor, 1.0);
}
