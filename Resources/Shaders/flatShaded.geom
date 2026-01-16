#version 460
// Расширения для Vulkan
#extension GL_EXT_geometry_shader : require


layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

layout(location = 0) in vec3 vColor[];  

layout(location = 0) out vec3 fColor; 
layout(location = 1) out vec3 fNormal; 
layout(location = 2) out vec3 position;

vec3 computeFaceNormal(vec3 worldPos0, vec3 worldPos1, vec3 worldPos2) {
    vec3 edge1 = worldPos1 - worldPos0;
    vec3 edge2 = worldPos2 - worldPos0;
    vec3 normal = cross(edge1, edge2);
    
    // Защита от вырожденного треугольника
    float len = length(normal);
    return len > 0.0 ? normalize(normal) : vec3(0.0, 0.0, 1.0);
}

void main() {

 vec3 faceNormal = computeFaceNormal(gl_in[0].gl_Position.xyz, gl_in[1].gl_Position.xyz, gl_in[2].gl_Position.xyz);

for(int i = 0; i < gl_in.length(); i++) {
        gl_Position = gl_in[i].gl_Position;
        position = gl_in[i].gl_Position.xyz;
        fNormal = faceNormal;
        fColor = vColor[i];

        EmitVertex();
    }
    EndPrimitive();
}