#version 330 core
#define MAXDIRECTIONALLIGHTPARTS 4

struct DirLight
{
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  mat4 lightSpaceMatrix[MAXDIRECTIONALLIGHTPARTS];
  float arrayOfDistances[MAXDIRECTIONALLIGHTPARTS];
  sampler2D shadow[MAXDIRECTIONALLIGHTPARTS];
};

struct PointLight
{
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;
  samplerCube shadow;
};
in vec2 TexCoord;
in vec3 FinNormal;
in vec3 FragPos;
in float ClipSpaceDepth;

out vec4 finalcolor;

uniform sampler2D curTexture;
uniform vec3 objectColor;
uniform vec3 viewPos;
uniform float far_plane;

uniform DirLight dirLight;
uniform PointLight pointLights[20];
uniform int numbOfLights;
uniform int numbOfCascadedShadows;

int findCorrectShadowMap()
{
  int i =0;
  for(;i<numbOfCascadedShadows;i++)
  {
    if(ClipSpaceDepth < dirLight.arrayOfDistances[i])
    {
      return i;
    }
  }
  return numbOfCascadedShadows-1;
}

float calcDirShadows()
{
  int index = findCorrectShadowMap();

  vec4 fragPosLightSpace = dirLight.lightSpaceMatrix[index]*vec4(FragPos,1.0f);
  vec3 proj = fragPosLightSpace.xyz/fragPosLightSpace.w;
  proj = proj *0.5+0.5;


  float closestDepth = texture(dirLight.shadow[index], proj.xy).r;


  float currentDepth = proj.z;

  float bias  = 0.0005;
  float shadow = currentDepth -bias > closestDepth ? 1.0 : 0.0;
  return shadow;


  /*
  float shadow = 0;
  vec2 texelSize = 1/textureSize(dirLight.shadow,0);

  for(int x = -1;x <=1 ; ++x)
  {
    for(int y = -1; y <= 1; ++y)
    {
      float pcfDepth = texture(dirLight.shadow, proj.xy + vec2(x,y) * texelSize).r;
      shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
  }
*/


  /*
  //if(shadow > 1.0) shadow = 1.0;
  if(proj.z > 1.0)
  {
    shadow = 0.0;
  }
  return shadow/9.0;
  */
}

float calcPointShadows(PointLight light)
{
  vec3 fragToLight = FragPos - light.position;
  float closestDepth = texture(light.shadow, fragToLight).r;
  closestDepth *= far_plane;
  float currentDepth = length(fragToLight);

  float bias = 0.5;
  float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
  return shadow;
}

vec3 calcDirectionalLight(float shadow)
{
  vec3 lightDir = normalize(-dirLight.direction);
  float diff = max(dot(FinNormal, lightDir), 0.0);

  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, FinNormal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

  vec3 ambient = dirLight.ambient;
  vec3 diffuse = dirLight.diffuse*diff;
  vec3 specular = dirLight.specular*spec;

  //return (ambient + shadow*(diffuse + specular))*texture(curTexture, TexCoord).rgb;
  return (ambient + shadow*(diffuse + specular));
  //return ((ambient+diffuse+specular)*texture(curTexture, TexCoord.rgb));
}

/*
vec3 calcPointLights(PointLight light, float shadow)
{
  vec3 lightDir = normalize(light.position - FragPos);

  float diff = max(dot(FinNormal, lightDir), 0.0);

  vec3 reflectDir = reflect(-lightDir,FinNormal);
  float spec = pow(max(dot(viewDir,reflectDir),0.0),32);
  float dist = length(light.position - FragPos);
  float atten = 1.0/ (light.constant + light.linear*dist + light.quadratic*(dist*dist));

  vec3 ambient = light.ambient;
  vec3 diffuse = light.diffuse;
  vec3 specular = light.specular;

  return (ambient + diffuse + specular) *atten;
}
*/

void main()
{
    float shadow = 1;
    vec3 color;
    //shadow += 1-calcDirShadows();

    /*
    for(int i=0;i<numbOfLights;i++)
    {
      shadow += 1-calcPointShadows(pointLights[i]);
    }

    */
    if(shadow>1) shadow = 1;

    //vec3 result = calcDirectionalLight(1.0f) * objectColor;
    //shadow = 1-calcDirShadows();
    //shadow = 1.0f;
    color = calcDirectionalLight(shadow);
    // /color *= objectColor;
    /*
    for(int i=0;i<numbOfLights;i++)
    {
      result += calcPointLights(pointLights[i],shadow);
    }
    */
    vec3 objColor = objectColor;

    /*
    if(findCorrectShadowMap() == 0) objColor =  vec3(0.000, 0.500, 1.000);
    else if(findCorrectShadowMap() == 1) objColor = vec3(1,0,0);
    else if(findCorrectShadowMap() == 2) objColor = vec3(0,1,0);
    */
    float maxDistance = dirLight.arrayOfDistances[numbOfCascadedShadows];
    vec3 finColor = color*objColor*texture(curTexture,TexCoord).rgb;
    finalcolor =vec4(finColor,1);
}
