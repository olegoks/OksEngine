#define VERTEX_TYPE_VF3\
layout(location = 0) in vec3 inPosition;

#define VERTEX_TYPE_VF3_NF3_TF2         \
layout(location = 0) in vec3 inPosition;\
layout(location = 1) in vec3 inNormal;  \
layout(location = 2) in vec2 inUV;

#define VERTEX_TYPE_VF3_CF3\
layout(location = 0) in vec3 inPosition;\
layout(location = 1) in vec3 inColor;

// #define VERTEX_TYPE_VF3_NF3_CF4\

// "VF3_NF3_TF2_BIDUB4_WUB4", --Vertex float 3, normal float 3, bone ids unsigned byte 4, weight unsigned byte 4
//                 "VF3_CF3",
//                 "VF3_NF3_CF4",
//                 "VF2_TF2_CF3",


