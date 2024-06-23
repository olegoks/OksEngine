#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 lightPos;
    float lightIntensity;
} ubo;

// layout(set = 1, binding = 0) uniform Transform {
//     mat4 model;
// } modelTransform;

//layout(set = 2, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 outUV;
void main() {
    //vec3 lightDirection = vec3(ubo.lightPos) - inPosition;
    //ragColor = vec4(0, 0, 0, 1);//texture(texSampler, inUV); //vec4(inUV, 0.0, 1.0);//texture(texSampler, inUV);//vec3(1, 1, 1);//* dot(lightDirection, inNormal);//texture(texSampler, inUV);// * dot(lightDirection, inNormal);

    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    outUV = inUV;
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