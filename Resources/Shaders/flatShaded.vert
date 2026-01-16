#version 450

layout(set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
} camera;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    vec3 lightPosition = vec3(20.0, 30.0, 0.0);
    vec3 lightDirection = normalize(lightPosition - inPosition);
     float intensity = dot(lightDirection, inNormal);
     if(intensity < 0.0) {
        intensity = intensity * -1.0 - 1.0;
     }
    fragColor = inColor;// * intensity;

    gl_Position = camera.proj * camera.view * vec4(inPosition, 1.0);
}
