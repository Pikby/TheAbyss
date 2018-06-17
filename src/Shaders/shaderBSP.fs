#version 330 core
struct DirLight
{
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  sampler2D shadow;
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
in vec4 FragPosLightSpace;

out vec4 finalcolor;

uniform sampler2D curTexture;
uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform float far_plane;

uniform DirLight dirLight;
uniform PointLight pointLights[20];
uniform int numbOfLights;
vec3 viewDir = normalize(viewPos - FragPos);


float calcDirShadows()
{
  vec3 proj = FragPosLightSpace.xyz/FragPosLightSpace.w;
  proj = proj *0.5+0.5;


  float closestDepth = texture(dirLight.shadow, proj.xy).r;


  float currentDepth = proj.z;

  float bias  = 0.005;

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

  //float shadow = currentDepth -bias > closestDepth ? 1.0 : 0.0;

  //if(shadow > 1.0) shadow = 1.0;
  if(proj.z > 1.0)
  {
    shadow = 0.0;
  }
  return shadow/9.0;
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

  vec3 reflectDir = reflect(-lightDir, FinNormal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

  vec3 ambient = dirLight.ambient*0.2;
  vec3 diffuse = dirLight.diffuse*diff;
  vec3 specular = dirLight.specular*spec;
  return (ambient + (shadow)*(diffuse + specular));
}

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

void main()
{
    float shadow = 0;
    shadow += 1-calcDirShadows();

    /*
    for(int i=0;i<numbOfLights;i++)
    {
      shadow += 1-calcPointShadows(pointLights[i]);
    }

    */
    if(shadow>1) shadow = 1;

    vec3 result = calcDirectionalLight(shadow) * objectColor;

    for(int i=0;i<numbOfLights;i++)
    {
      result += calcPointLights(pointLights[i],shadow);
    }
    finalcolor = texture(curTexture,TexCoord);//*vec4(result,1.0f);
}
