#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fNormal; 
layout(location = 2) in vec3 position; 

layout(location = 0) out vec4 outColor;

void main() {
    vec3 lightPosition = vec3(1000.0, 1000.0, 1000.0);
    vec3 lightDirection = normalize(position - lightPosition);
     float intensity = dot(lightDirection, fNormal);
     if(intensity < 0.0) {
        intensity = intensity * -1.0 - 1.0;
     }
    outColor = vec4(fragColor * intensity, 1.0);
}
