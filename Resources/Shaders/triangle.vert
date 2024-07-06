#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject {
    //mat4 model;
    mat4 view;
    mat4 proj;
    vec3 lightPos;
    float lightIntensity;
} ubo;

layout(set = 1, binding = 0) uniform Transform {
    mat4 model;
} modelTransform;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    vec3 lightDirection = vec3(ubo.lightPos) - inPosition;
    vec4 transformedNormal = vec4(inNormal, 1.0) *  modelTransform.model;
    float intensity = dot(lightDirection, vec3(transformedNormal.x, transformedNormal.y, transformedNormal.z));
    fragColor = inColor * intensity;

    gl_Position = ubo.proj * ubo.view * modelTransform.model * vec4(inPosition, 1.0);
}


// #version 450

// layout(binding = 0) uniform UniformBufferObject {
//     mat4 model;
//     mat4 view;
//     mat4 proj;
// } ubo;

// layout(location = 0) in vec3 inPosition;
// layout(location = 1) in vec3 inColor;
// layout(location = 2) in vec2 inTexCoord;

// layout(location = 0) out vec3 fragColor;
// layout(location = 1) out vec2 fragTexCoord;

// void main() {
//     gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
//     fragColor = inColor;
//     fragTexCoord = inTexCoord;
// }