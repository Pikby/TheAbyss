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
in vec2 TexCoords;
out vec4 finalcolor;


uniform sampler2D gColorSpec;
uniform sampler2D gNormal;
uniform sampler2D gPosition;
uniform sampler2D transTexture;

uniform vec3 viewPos;
uniform float far_plane;
uniform float fog_start;
uniform float fog_dist;

uniform DirLight dirLight;
uniform PointLight pointLights[20];
uniform int numbOfLights;
uniform int numbOfCascadedShadows;

vec4 objColorAO = texture(gColorSpec, TexCoords).rgba;
vec3 objColor = objColorAO.rgb;

vec4 transColorAO = texture(transTexture, TexCoords).rgba;
vec3 transColor = transColorAO.rgb;
float transColorCount = transColorAO.a;

float AO = objColorAO.a;
vec3 normal = texture(gNormal, TexCoords).rgb;
vec3 fragPosition = texture(gPosition, TexCoords).rgb;
float fragDepth = distance(fragPosition,viewPos);




int findCorrectShadowMap()
{
  int i;
  for(i=0;i<numbOfCascadedShadows;i++)
  {
    if(fragDepth < dirLight.arrayOfDistances[i])
    {
      return i;
    }
  }
  return numbOfCascadedShadows-1;
}

float calcDirShadows()
{
  int index = findCorrectShadowMap();
  vec4 fragPosLightSpace = dirLight.lightSpaceMatrix[index]*vec4(fragPosition,1.0f);
  vec3 proj = fragPosLightSpace.xyz/fragPosLightSpace.w;
  proj = proj*0.5+0.5;


//  float closestDepth = texture(dirLight.shadow[index], proj.xy).r;
  float currentDepth = proj.z;
  /*

  float shadow = currentDepth -bias > closestDepth ? 1.0 : 0.0;
  return shadow;
  */


  float bias = max(0.0005 * (1.0 - dot(normal, dirLight.direction)), 0.0000005);
  bias = 0;
  float shadow = 0;
  vec2 texelSize = 1/textureSize(dirLight.shadow[index],0);

  for(int x = -1;x <=1 ; ++x)
  {
    for(int y = -1; y <= 1; ++y)
    {
      float pcfDepth = texture(dirLight.shadow[index], proj.xy + vec2(x,y) * texelSize).r;
      shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
  }
  return shadow/9.0;

}
/*
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
*/
vec3 calcDirectionalLight(float shadow)
{
  vec3 lightDir = normalize(-dirLight.direction);
  float diff = max(dot(normal, lightDir), 0.0);

  vec3 viewDir = normalize(viewPos - fragPosition);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);


  vec3 ambient = dirLight.ambient*AO;
  vec3 diffuse = dirLight.diffuse*diff;
  vec3 specular = dirLight.specular*spec;

  return (ambient + shadow*(diffuse + specular));
}


void main()
{

    float shadow = 1;
    //shadow = 1-calcDirShadows();



/*
    if(findCorrectShadowMap() == 0) objColor =  vec3(0.000, 0.500, 1.000);
    else if(findCorrectShadowMap() == 1) objColor = vec3(1,0,0);
    else if(findCorrectShadowMap() == 2) objColor = vec3(0,1,0);
    */
    //float maxDistance = dirLight.arrayOfDistances[numbOfCascadedShadows];

    float fogMod = max((fragDepth-fog_start)/fog_dist,0);
    vec3 fog = fogMod*vec3(1.0f,1.0f,1.0f);
    vec3 finColor = fog+objColor*calcDirectionalLight(shadow);
    //vec3 finColor = fog+objColor;

    //Pixel has no translucent objects
    if(transColor == vec3(0.0f,0.0f,0.0f))
    {
      finalcolor = vec4(finColor,1);
    }
    else
    {
      //Pixel has translucent object, so calculate it
      vec3 finTransColor = transColor/((transColorCount-0.5));
      finalcolor = vec4((finTransColor + finColor )/2.0f,1.0f);

    }
}
