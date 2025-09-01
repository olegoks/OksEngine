#version 450 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout (location = 3) in uvec4 inBoneIds;
layout (location = 4) in uvec4 inBoneWeights;

layout(set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
} camera;

layout(set = 1, binding = 0) uniform Transform {
    mat4 model;
} transform;

layout(set = 2, binding = 0) uniform BonesPalette {
    mat4 matrices[128];
} bonesPalette;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 outUV;

void main() {

    vec4 position = vec4(inPosition, 1.0);

    vec4 newPosition = vec4(0, 0, 0, 0);
    newPosition = newPosition + (float(inBoneWeights[0]) / 255.0) * bonesPalette.matrices[inBoneIds[0]] * position;
    newPosition = newPosition + (float(inBoneWeights[1]) / 255.0) * bonesPalette.matrices[inBoneIds[1]] * position;
    newPosition = newPosition + (float(inBoneWeights[2]) / 255.0) * bonesPalette.matrices[inBoneIds[2]] * position;
    newPosition = newPosition + (float(inBoneWeights[3]) / 255.0) * bonesPalette.matrices[inBoneIds[3]] * position;

    outUV = inUV;

    gl_Position = camera.proj * camera.view * (vec4(newPosition.xyz, 1.0));//* transform.model * (vec4(newPosition.xyz, 1.0));
}