#version 330 core

out vec4 FragColor;

in VS_OUT {
  vec3 WorldPos;
  vec2 texCoord;
  vec3 Normal;
  vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuse;
uniform sampler2D shadowMap;
uniform sampler2D illuminationMap;
uniform sampler2D lowResPosition;
uniform sampler2D lowResNormal;

uniform vec3 lightDir;
uniform int windowWidth;
uniform int windowHeight;

const int PCFsamples = 3;
const float lightPower = 5;

float shadowAmount() {
  vec3 projCoords = fs_in.FragPosLightSpace.xyz * 0.5f + 0.5f; // to [0,1] range
  float fragmentDepth = projCoords.z;
  float bias = max(0.005 * (1.0 - dot(fs_in.Normal, lightDir)), 0.005);
  vec2 texelSize = 1.0f / textureSize(shadowMap, 0);
  float shadow = 0;
  for(int i=-PCFsamples/2; i<=PCFsamples/2; i++) {
    for(int j=-PCFsamples/2; j<=PCFsamples/2; j++) {
      float neighbourDepth = texture(shadowMap, projCoords.xy + vec2(i,j)*texelSize).r;
      shadow += fragmentDepth > neighbourDepth + bias ? 1.0 : 0.0;
    }
  }

  return shadow/float(PCFsamples*PCFsamples);
}

void main() {
  vec3 L = normalize(lightDir);
  vec4 color = texture(diffuse, fs_in.texCoord);

  vec2 normScrrenCoord = vec2(gl_FragCoord.x/windowWidth, gl_FragCoord.y/windowHeight);
  vec4 illumination = vec4(0.0f);

  vec3 apos = texture(lowResPosition, normScrrenCoord).xyz;
  vec3 anorm = texture(lowResNormal, normScrrenCoord).xyz;
  float dist = max(0,length(apos - fs_in.WorldPos)/4.0f -0.1f );
  illumination = texture(illuminationMap, normScrrenCoord)
    *(dot(anorm, fs_in.Normal)) // smoothly dark out inputs from wrong points
    *(1-dist); // using normal and position interpolation

  vec4 ambient = color * illumination;

  float litDegree = max(dot(L, fs_in.Normal), 0.0f) * lightPower;
  vec4 diffuse = color * litDegree * (1-shadowAmount());

  FragColor = ambient + diffuse;
}
