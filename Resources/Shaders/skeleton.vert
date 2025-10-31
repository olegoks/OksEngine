#version 450 core

#extension GL_ARB_gpu_shader_int64 : enable
#extension GL_EXT_debug_printf : enable
#extension GL_GOOGLE_include_directive : require

#include "/common/OksEngine.Common.Math3D.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout (location = 3) in uvec4 inBoneIds;
layout (location = 4) in vec4 inBoneWeights;

layout(set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
} camera;

//NODES
struct WorldPosition3D {
    vec4 position_;
};

layout(std430, set = 2, binding = 0) buffer WorldPositions3D {
    WorldPosition3D positions_[];
};

struct WorldRotation3D {
    vec4 rotation_;
};

layout(std430, set = 3, binding = 0) buffer WorldRotations3D {
    WorldRotation3D rotations_[];
};

struct WorldScale3D {
    vec4 scale_;
};

layout(std430, set = 4, binding = 0) buffer WorldScales3D {
    WorldScale3D scales_[];
};

struct ModelNodeEntityIds {
    uint64_t nodeIds_[512];
};

struct ModelEntityIds {
    uint64_t modelIds_[1024];
};

layout(std430, set = 5, binding = 0) buffer ModelsNodeEntityIds {
    ModelNodeEntityIds modelNodeEntityIds_[];
};

layout(std430, set = 6, binding = 0) buffer BoneInverseBindPoseMatrices {
    mat4 boneInverseBindPoseMatrices[];
};


layout(std430, set = 7, binding = 0) buffer NodeEntityIdsToComponentIndices {
    uint64_t nodeComponentsIndices_[];
};

layout(std430, set = 8, binding = 0) buffer ModelEntityIdsToComponentIndices {
    uint64_t modelComponentsIndices_[];
};

layout(std430, set = 9, binding = 0) buffer ModelEntityIdentifires{
    ModelEntityIds meshModelIds_[];
};

// Model node data ids for model nodes.
layout(std430, set = 10, binding = 0) buffer ModelNodeDataEntityIds {
    uint64_t modelNodeDataEntityIds_[]; //[modelNodeComponentsIndex] -> model node data id
};

layout(std430, set = 11, binding = 0) buffer ModelNodeDataEntityIdsToComponentIndices {
    uint64_t modelNodeDataEntityIdsToComponentIndices_[];
};


layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 outUV;

layout(push_constant) uniform PushConstants {
    uint64_t nodeDataEntitiesNumber_;   // Node data entities number.
    uint64_t nodeEntitiesNumber_;       // Node entities number.
    uint64_t modelEntitiesNumber_;     
    uint64_t modelDataEntitiesNumber_;     
    uint64_t meshComponentsIndex_;      // Components index for current mesh.
};

// uint64_t GetComponentIndexByNodeEntityId(uint64_t entityId){

//     return ;

//     //TODO: use binary search
//     // for(uint i = 0; i < nodeEntitiesNumber_; i++){
//     //     if(nodeIdsToIndices[i].entityId_ == entityId){
//     //         //ASSERT_MSG(entityId != INVALID_ENTITY_ID, "ASSERT: Invalid bone node entity id.");
//     //         return nodeIdsToIndices[i].componentIndex_;
//     //     }
//     // }

//     // return (-1);


//     //idsToIndices sorted
//     // uint left = 0;
//     // uint right = uint(nodeEntitiesNumber_) - 1;
    
//     // while (left <= right) {
//     //     uint mid = left + (right - left) / 2;
        
//     //     if (nodeIdsToIndices[mid].entityId_ == entityId) {
//     //         return uint64_t(mid);
//     //     }
        
//     //     if (nodeIdsToIndices[mid].entityId_ < entityId) {
//     //         left = mid + 1;
//     //     } else {
//     //         right = mid - 1;
//     //     }
//     // }
    
//     // return (-1);

// }

// uint64_t GetComponentIndexByModelEntityId(uint64_t modelEntityId){

//     return modelComponentsIndices_[uint(modelEntityId)];


//     //TODO: use binary search
//     // for(uint i = 0; i < modelEntitiesNumber_; i++){
//     //     //ASSERT_FMSG_1(modelIdsToIndices_[i].entityId_ != 0, "modelIdsToIndices_ contains invalid entity id 0. Index %d.", i);
//     //     if(modelIdsToIndices_[i].entityId_ == modelEntityId){
//     //         //ASSERT_MSG(modelEntityId != INVALID_ENTITY_ID, "ASSERT: Invalid bone node entity id.");
//     //         return modelIdsToIndices_[i].componentIndex_;
//     //     }
//     // }
//     // return (-1);

//     // uint left = 0;
//     // uint right = uint(modelEntitiesNumber_) - 1;
    
//     // while (left <= right) {
//     //     uint mid = left + (right - left) / 2;
        
//     //     if (modelIdsToIndices_[mid].entityId_ == modelEntityId) {
//     //         return mid;
//     //     }
        
//     //     if (modelIdsToIndices_[mid].entityId_ < modelEntityId) {
//     //         left = mid + 1;
//     //     } else {
//     //         right = mid - 1;
//     //     }
//     // }
    
//     // return uint64_t(-1);

// }


// bool IsBoneWeightSignificant(float weight) {
//     //ASSERT_MSG(weight <= 1.0, "ASSERT: Invalid bone weight.");
//     return weight > 0.00001;
// }

//position - position that bone will effect.
//bone - index of one of 4 bones.
vec4 TakeBoneIntoAccount(vec4 position, vec4 vertexPosition, uint modelComponentsIndex, uint bone) {

    //ModelNodeEntityIds modelNodeEntityIds = ;

    float boneWeight = inBoneWeights[bone];
    if(boneWeight > 0.00001) {

        uint boneIndexInModelSpace = inBoneIds[bone];
        if(boneIndexInModelSpace == 512) {
            return position;
        }
        uint64_t nodeEntityId = modelNodeEntityIds_[modelComponentsIndex].nodeIds_[boneIndexInModelSpace];

        // ASSERT_FMSG_2(
        //     (nodeEntityId != INVALID_ENTITY_ID) && (nodeEntityId != 0), 
        //     "ASSERT: Bone node invalid entity id. boneIndexInModelSpace %d modelComponentsIndex %d",
        //     uint(boneIndexInModelSpace), 
        //     uint(modelComponentsIndex));

        uint nodeComponentsIndex = uint(nodeComponentsIndices_[uint(nodeEntityId)]);
        uint64_t modelNodeDataEntityId = modelNodeDataEntityIds_[nodeComponentsIndex];
        uint64_t modelNodeDataComponentsIndex = modelNodeDataEntityIdsToComponentIndices_[uint(modelNodeDataEntityId)];
        

        // ASSERT_FMSG_3(nodeComponentsIndex != -1, 
        //  "ASSERT: Invalid bone component index calculated for bone entity id %d. Vertex bone index %d. Bone index in model space %d.",
        //   uint(nodeEntityId), bone, boneIndexInModelSpace);
        vec3 translate = vec3(positions_[nodeComponentsIndex].position_.x, positions_[nodeComponentsIndex].position_.y, positions_[nodeComponentsIndex].position_.z);
        vec4 rotation = rotations_[nodeComponentsIndex].rotation_;
        vec3 scale = vec3(scales_[nodeComponentsIndex].scale_.x, scales_[nodeComponentsIndex].scale_.y, scales_[nodeComponentsIndex].scale_.z);


        //mat4 boneInverseBindPoseMatrix = boneInverseBindPoseMatrices[uint(modelNodeDataComponentsIndex)];
        //mat4 boneWorldTransform = RTS_to_mat4_optimized(translate, rotation, scale);

        position = position + boneWeight * RTS_to_mat4_optimized(translate, rotation, scale) * boneInverseBindPoseMatrices[uint(modelNodeDataComponentsIndex)] * vertexPosition;
    }

    return position;
}


void main() {

    vec4 position = vec4(inPosition, 1.0);

    vec4 newPosition = vec4(0, 0, 0, 0);

    //Get model entity that we are rendering at the moment.
    uint64_t modelEntityId = meshModelIds_[uint(meshComponentsIndex_)].modelIds_[gl_InstanceIndex];

    // ASSERT_FMSG_2(
    //     (modelEntityId != INVALID_ENTITY_ID) && (modelEntityId != 0), 
    //     "ASSERT: Model invalid entity id %d. meshComponentsIndex_ %d",
    //      int(modelEntityId),
    //      int(meshComponentsIndex_));

    //Get index of model components.
    uint modelComponentsIndex = uint(modelComponentsIndices_[uint(modelEntityId)]);//GetComponentIndexByModelEntityId(modelEntityId);
    // ASSERT_FMSG_3(modelComponentsIndex != -1, 
    //      "ASSERT: Invalid model component index calculated for model entity id %i. meshComponentsIndex_ %i gl_InstanceIndex %i",
    //       int(modelEntityId),
    //         int(meshComponentsIndex_), int(gl_InstanceIndex));

    newPosition = TakeBoneIntoAccount(newPosition, position, modelComponentsIndex, uint(0));
    newPosition = TakeBoneIntoAccount(newPosition, position, modelComponentsIndex, uint(1));
    newPosition = TakeBoneIntoAccount(newPosition, position, modelComponentsIndex, uint(2));
    newPosition = TakeBoneIntoAccount(newPosition, position, modelComponentsIndex, uint(3));

    outUV = inUV;

    //ASSERT_MSG(!any(isnan(newPosition)), "Calculated vertex position is nan.");

    gl_Position = camera.proj * camera.view * (vec4(newPosition.xyz, 1.0));
}