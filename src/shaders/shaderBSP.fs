#version 330 core
in vec2 TexCoord;
in vec3 NormVec;
in vec3 FragPos;
in vec4 FragPosLightSpace;

out vec4 color;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D curTexture;
uniform sampler2D shadowMap;

float shadowCalculation(vec4 fragPosLightSpace)
{
  vec3 projCoords = fragPosLightSpace.xyz/fragPosLightSpace.w;

  projCoords = projCoords*0.5 + 0.5;
  float nearestDepth = texture(shadowMap, projCoords.xy).r;
  float curDepth = projCoords.z;

  float shadow = curDepth -0.005> nearestDepth ? 1.0 : 0.0;
  return shadow;
}

void main()
{
  float ambientStrength = 0.3;
  vec3 ambient = ambientStrength * lightColor;

  // diffuse
  vec3 norm = normalize(NormVec);
  vec3 lightDir = normalize(lightPos - FragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * lightColor;

  // specular
  float specularStrength = 0.5;
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  vec3 specular = specularStrength * spec * lightColor;

  float shadow = shadowCalculation(FragPosLightSpace);
  vec3 result = (ambient+(1.0-shadow)*(diffuse+specular));
  color = texture(curTexture,TexCoord)* vec4(result, 1.0);
}
