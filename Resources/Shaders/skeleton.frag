#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 inUV;
layout(location = 0) out vec4 outColor;
layout(set = 1, binding = 0) uniform sampler2D texSampler;

layout(set = 12, binding = 0) uniform sampler2D normalTexSampler;



void main() {

    vec3 lightDir = vec3(10.0, 0.0, 0.0);

    vec3 normal = texture(normalTexSampler, inUV).rgb;
    vec3 normalNormalized = normalize(normal);

    float diff = max(dot(normalize(lightDir), normalNormalized), 0.0);

    vec3 diffuseColor = texture(texSampler, inUV).rgb;
    vec3 diffuseColorWithNormal = diffuseColor * diff;

    vec3 ambient = 0.1 * diffuseColor;

    outColor = vec4(normal, 1.0);//vec4(ambient + diffuseColorWithNormal, 1.0);
}
