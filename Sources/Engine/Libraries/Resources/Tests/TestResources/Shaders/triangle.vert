#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 lightPos;
    float lightIntensity;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexel;

layout(location = 0) out vec3 fragColor;

const vec3 lightDirection = normalize(vec3(0.0, 0.0, 0.1));

void main() {
    const vec4 vertexPosition = vec4(inPosition, 1.0);

    vec3 normalWorldSpace = normalize(mat3(ubo.model) * inNormal);
    float lightIntensity = max(dot(normalWorldSpace, lightDirection), 0.1);

    fragColor = vec3(1.0, 0.0, 0.0) * lightIntensity;//vec3(abs(inNormal[0]), abs(inNormal[1]), abs(inNormal[2]));//inColor * lightIntensity;
    gl_Position = ubo.proj * ubo.view * ubo.model * vertexPosition;
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