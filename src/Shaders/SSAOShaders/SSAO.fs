#version 330 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];

// parameters (you'd probably want to use them as uniforms to more easily tweak the effect)
int kernelSize = 16;
float radius = 2.0;
float bias = 1.0;

// tile noise texture over screen based on screen dimensions divided by noise size
const vec2 noiseScale = vec2(1920.0/4.0, 1080.0/4.0);

uniform mat4 projection;
uniform mat4 view;

void main()
{
    // get input for SSAO algorithm

    vec3 fragPos = texture(gPosition, TexCoords).rgb;
    fragPos = (view*vec4(fragPos,1)).rgb;
    vec3 normal = texture(gNormal, TexCoords).rgb;
    normal = normalize(transpose(inverse(mat3(view)))*normal);
    vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).rgb);

    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 samp = TBN * samples[i]; // from tangent to view-space
        samp = fragPos + samp * radius;

        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(samp, 1.0);
        offset = projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

        // get sample depth

        vec3 temp = texture(gPosition,offset.xy).xyz;
        float sampleDepth = (view*vec4(temp,1.0)).z;

        //float sampleDepth = texture(gPosition, offset.xy).z; // get depth value of kernel sample

        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samp.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / kernelSize);

    FragColor = occlusion;
}
