#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject {
    //mat4 model;
    mat4 view;
    mat4 proj;
    vec3 lightPos;
    float lightIntensity;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 2) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {

    fragColor = inColor;

    gl_Position = ubo.proj * ubo.view  * vec4(inPosition, 1.0);
}
