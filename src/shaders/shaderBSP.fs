#version 330 core
in vec2 TexCoord;
in vec3 NormVec;
in vec3 FragPos;

out vec4 color;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform sampler2D ourTexture;

void main()
{
    float ambientStrength = 0.9, specStrength = 0.5;



    vec3 ambient = ambientStrength * lightColor;
    vec3 norm = normalize(NormVec);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm,lightDir),0.0f);
    vec3 diffuse = diff * lightColor;


    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specStrength * spec * lightColor;



    vec3 result = (ambient+diffuse+specular)*objectColor;
    color = texture(ourTexture, TexCoord)*vec4(result,1.0);
}
