#version 330 core

out vec4 FragColor;

in VS_OUT {
  vec3 WorldPos;
  vec2 texCoord;
  vec3 Normal;
  mat3 TangentToWorld;
} fs_in;


uniform vec3 camPos;
uniform sampler2D albedoTex;
uniform sampler2D metallicTex;
uniform sampler2D roughnessTex;
uniform sampler2D aoTex;
uniform sampler2D normalTex;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

const float PI = 3.14159265359;
const float MAX_REFLECTION_LOD = 3.0;

vec3 frenelSchlick(float cosTheta, vec3 F0) {
  return F0 + (1-F0)*pow(1-cosTheta, 5.0);
}

vec3 frenelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
  return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
  float a = roughness*roughness;
  float a2 = a*a;
  float NdotH = max(dot(N,H), 0.0);
  float NdotH2 = NdotH*NdotH;

  float nom = a2;
  float denom = (NdotH2 * (a2 - 1.0)+1.0);
  denom = PI * denom * denom;
  return nom/denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
  float r = roughness + 1.0;
  float k = (r*r) / 8.0;

  float nom = NdotV;
  float denom = NdotV * (1.0 - k) + k;

  return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);

  float ggx1 = GeometrySchlickGGX(NdotV, roughness);
  float ggx2 = GeometrySchlickGGX(NdotL, roughness);

  return ggx1*ggx2;
}

void main() {
  vec3 tangentN = texture(normalTex, fs_in.texCoord).rgb;
  tangentN = normalize(tangentN * 2.0 - 1.0); // to [-1,1] range
  vec3 N = normalize(fs_in.TangentToWorld * tangentN);
  vec3 V = normalize(camPos - fs_in.WorldPos);

  vec3 albedo = texture(albedoTex, fs_in.texCoord).rgb;
  float metallic = texture(metallicTex, fs_in.texCoord).r;
  float roughness = texture(roughnessTex, fs_in.texCoord).r;
  float ao = texture(aoTex, fs_in.texCoord).r;

  vec3 F0 = vec3(0.04); // normal glass reflection coefficient
  F0 = mix(F0, albedo, metallic); // metals are coloring reflected light

  vec3 R = reflect(-V,N);
  vec3 prefilteredColor = textureLod(prefilterMap, R,
      roughness*MAX_REFLECTION_LOD).rgb;

  vec3 Fr = frenelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
  vec2 envBRDF = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
  vec3 specular = prefilteredColor * (Fr * envBRDF.x + envBRDF.y);

  vec3 kS = Fr;
  vec3 kD = vec3(1.0)-kS;
  kD *= 1.0-metallic;
  vec3 irradiance = texture(irradianceMap, N).rgb;
  vec3 diffuse = irradiance * albedo;

  vec3 ambient = (kD * diffuse + specular) * ao;

  vec3 color = ambient;
  color = color/(vec3(1.0)+color);
  color = pow(color, vec3(1.0/2.2));

  FragColor = vec4(color, 1.0);
}
