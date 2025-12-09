#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_debug_printf : enable
#include "/common/OksEngine.Common.Math3D.glsl"
#include "/common/OksEngine.GPGPUECS.glsl"

layout(set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
} camera;

layout(std430, set = 2, binding = 0) buffer ModelEntityIdentifires{
    ModelEntityIds meshModelIds_[];
};

layout(std430, set = 3, binding = 0) buffer ModelEntityIdsToComponentIndices {
    uint64_t modelComponentsIndices_[];
};

layout(std430, set = 4, binding = 0) buffer ModelsNodeEntityIds {
    ModelNodeEntityIds modelNodeEntityIds_[];
};

layout(std430, set = 5, binding = 0) buffer ModelsNodeEntityIndices {
    ModelNodeEntityIndices modelNodeEntityIndices_[];
};

layout(std430, set = 6, binding = 0) buffer NodeEntityIdsToComponentIndices {
    uint64_t nodeComponentsIndices_[];
};

layout(std430, set = 7, binding = 0) buffer WorldPositions3D {
    WorldPosition3D positions_[];
};


layout(std430, set = 8, binding = 0) buffer WorldRotations3D {
    WorldRotation3D rotations_[];
};


layout(std430, set = 9, binding = 0) buffer WorldScales3D {
    WorldScale3D scales_[];
};

layout(push_constant) uniform PushConstants {
  
    uint64_t meshComponentsIndex_;      // Components index for current mesh.
};

//layout(set = 1, binding = 0) uniform Transform {
//    mat4 model;
//} transform;

// layout(std140, set = 2, binding = 0) uniform Transform {
//     vec4 rotation;
//     vec3 translation;
// 	vec3 scale;
// } transform;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 outUV;

void main() {


    uint64_t modelEntityId = meshModelIds_[uint(meshComponentsIndex_)].modelIds_[gl_InstanceIndex];

    ASSERT_FMSG_2(
        (modelEntityId != INVALID_ENTITY_ID) && (modelEntityId != 0), 
        "ASSERT: Model invalid entity id %d. meshComponentsIndex_ %d",
         int(modelEntityId),
         int(meshComponentsIndex_));

    //Get index of model components.
    uint modelComponentsIndex = uint(modelComponentsIndices_[uint(modelEntityId)]);//GetComponentIndexByModelEntityId(modelEntityId);
    ASSERT_FMSG_3(modelComponentsIndex != -1, 
         "ASSERT: Invalid model component index calculated for model entity id %i. meshComponentsIndex_ %i gl_InstanceIndex %i",
          int(modelEntityId),
            int(meshComponentsIndex_), int(gl_InstanceIndex));

    ModelNodeEntityIndices meshModelNodeIndices = modelNodeEntityIndices_[uint(meshComponentsIndex_)];
    
    uint64_t nodeIndex = meshModelNodeIndices.modelNodeIndices_[uint(0)];

    uint64_t nodeEntityId = modelNodeEntityIds_[modelComponentsIndex].nodeIds_[uint(nodeIndex)];
    
    ASSERT_MSG(
            (nodeEntityId != INVALID_ENTITY_ID) && (nodeEntityId != 0), 
            "ASSERT: Bone node invalid entity id.");

    uint nodeComponentsIndex = uint(nodeComponentsIndices_[uint(nodeEntityId)]);

    
        vec3 translate = vec3(positions_[nodeComponentsIndex].position_.x, positions_[nodeComponentsIndex].position_.y, positions_[nodeComponentsIndex].position_.z);
        vec4 rotation = rotations_[nodeComponentsIndex].rotation_;
        vec3 scale = vec3(scales_[nodeComponentsIndex].scale_.x, scales_[nodeComponentsIndex].scale_.y, scales_[nodeComponentsIndex].scale_.z);

     gl_Position 
     = camera.proj 
     * camera.view 
     * RTS_to_mat4_optimized(translate, rotation, scale) 
     * vec4(inPosition, 1.0);

    outUV = inUV;
}