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
    uint64_t modelIds_[512];
};

layout(std430, set = 5, binding = 0) buffer ModelsNodeEntityIds {
    ModelNodeEntityIds modelNodeEntityIds_[];
};

layout(std430, set = 6, binding = 0) buffer BoneInverseBindPoseMatrices {
    mat4 boneInverseBindPoseMatrices[];
};

struct EntityIdToComponentIndex {
    uint64_t entityId_;
    uint64_t componentIndex_;
};

layout(std430, set = 7, binding = 0) buffer NodeEntityIdsToComponentIndices {
    EntityIdToComponentIndex nodeIdsToIndices[];
};

layout(std430, set = 8, binding = 0) buffer ModelEntityIdsToComponentIndices {
    EntityIdToComponentIndex modelIdsToIndices_[];
};

layout(std430, set = 9, binding = 0) buffer ModelEntityIdentifires{
    ModelEntityIds meshModelIds_[];
};

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 outUV;

layout(push_constant) uniform PushConstants {
    uint64_t nodeEntitiesNumber_; // Node bone entities number
    uint64_t modelEntitiesNumber_;
    uint64_t meshComponentsIndex_; // Components index for current mesh.
};

uint64_t GetComponentIndexByNodeEntityId(uint64_t entityId){

    //TODO: use binary search
    for(uint i = 0; i < nodeEntitiesNumber_; i++){
        if(nodeIdsToIndices[i].entityId_ == entityId){
            ASSERT_MSG(entityId != INVALID_ENTITY_ID, "ASSERT: Invalid bone node entity id.");
            return nodeIdsToIndices[i].componentIndex_;
        }
    }
    //ASSERT_FAIL_FMSG_1("Error while finding Component index by entity id %d.", entityId);
    return (-1);
    // //idsToIndices sorted
    // uint left = 0;
    // uint right = uint(entitiesNumber_) - 1;
    
    // while (left <= right) {
    //     uint mid = left + (right - left) / 2;
        
    //     if (idsToIndices[mid].entityId_ == entityId) {
    //         return mid;
    //     }
        
    //     if (idsToIndices[mid].entityId_ < entityId) {
    //         left = mid + 1;
    //     } else {
    //         right = mid - 1;
    //     }
    // }
    
    // return uint64_t(-1);

}

uint64_t GetComponentIndexByModelEntityId(uint64_t modelEntityId){

    //TODO: use binary search
    for(uint i = 0; i < modelEntitiesNumber_; i++){
        ASSERT_FMSG_1(modelIdsToIndices_[i].entityId_ != 0, "modelIdsToIndices_ contains invalid entity id 0. Index %d.", i);
        if(modelIdsToIndices_[i].entityId_ == modelEntityId){
            ASSERT_MSG(modelEntityId != INVALID_ENTITY_ID, "ASSERT: Invalid bone node entity id.");
            return modelIdsToIndices_[i].componentIndex_;
        }
    }
    //ASSERT_FAIL_FMSG_1("Error while finding Component index by entity id %d.", entityId);
    return (-1);
    // //idsToIndices sorted
    // uint left = 0;
    // uint right = uint(entitiesNumber_) - 1;
    
    // while (left <= right) {
    //     uint mid = left + (right - left) / 2;
        
    //     if (idsToIndices[mid].entityId_ == entityId) {
    //         return mid;
    //     }
        
    //     if (idsToIndices[mid].entityId_ < entityId) {
    //         left = mid + 1;
    //     } else {
    //         right = mid - 1;
    //     }
    // }
    
    // return uint64_t(-1);

}


bool IsBoneWeightSignificant(float weight) {
    ASSERT_MSG(weight <= 1.0, "ASSERT: Invalid bone weight.");
    return weight > 0.00001;
}

//position - position that bone will effect.
//bone - index of one of 4 bones.
vec4 TakeBoneIntoAccount(vec4 position, vec4 vertexPosition, uint64_t modelComponentsIndex, uint bone) {

    ModelNodeEntityIds modelNodeEntityIds = modelNodeEntityIds_[uint(modelComponentsIndex)];

    float boneWeight = inBoneWeights[bone];
    if(IsBoneWeightSignificant(boneWeight)) {

        uint boneIndexInModelSpace = inBoneIds[bone];
        if(boneIndexInModelSpace == 512) {
            return position;
        }
        uint64_t nodeEntityId = modelNodeEntityIds.nodeIds_[boneIndexInModelSpace];

        ASSERT_FMSG_2(
            (nodeEntityId != INVALID_ENTITY_ID) && (nodeEntityId != 0), 
            "ASSERT: Bone node invalid entity id. boneIndexInModelSpace %d modelComponentsIndex %d",
            uint(boneIndexInModelSpace), 
            uint(modelComponentsIndex));

        uint64_t boneComponentIndex = GetComponentIndexByNodeEntityId(nodeEntityId);

        ASSERT_FMSG_3(boneComponentIndex != -1, 
         "ASSERT: Invalid bone component index calculated for bone entity id %d. Vertex bone index %d. Bone index in model space %d.",
          uint(nodeEntityId), bone, boneIndexInModelSpace);

        uint uintBoneComponentIndex = uint(boneComponentIndex); // cast to use in []


        vec3 translate = vec3(positions_[uintBoneComponentIndex].position_.x, positions_[uintBoneComponentIndex].position_.y, positions_[uintBoneComponentIndex].position_.z);
        vec4 rotation = rotations_[uintBoneComponentIndex].rotation_;
        vec3 scale = vec3(scales_[uintBoneComponentIndex].scale_.x, scales_[uintBoneComponentIndex].scale_.y, scales_[uintBoneComponentIndex].scale_.z);


        mat4 boneInverseBindPoseMatrix = boneInverseBindPoseMatrices[uintBoneComponentIndex];
        mat4 boneWorldTransform = RTS_to_mat4_optimized(translate, rotation, scale);

        position = position + boneWeight * boneWorldTransform * boneInverseBindPoseMatrix * vertexPosition;
    }

    return position;
}


void main() {

    vec4 position = vec4(inPosition, 1.0);

    vec4 newPosition = vec4(0, 0, 0, 0);



    //Get models ids that uses that mesh.
    ModelEntityIds modelEntityIds = meshModelIds_[uint(meshComponentsIndex_)]; 

    //Get model entity that we are rendering at the moment.
    uint64_t modelEntityId = modelEntityIds.modelIds_[gl_InstanceIndex];

    ASSERT_FMSG_2(
        (modelEntityId != INVALID_ENTITY_ID) && (modelEntityId != 0), 
        "ASSERT: Model invalid entity id %d. meshComponentsIndex_ %d",
         int(modelEntityId),
         int(meshComponentsIndex_));

    //if(modelEntityId == INVALID_ENTITY_ID){
         //outUV = inUV;
         //gl_Position = camera.proj * camera.view * (vec4(inPosition.xyz, 1.0));
    //}

    //Get index of model components.
    uint64_t modelComponentsIndex = GetComponentIndexByModelEntityId(modelEntityId);
    ASSERT_FMSG_3(modelComponentsIndex != -1, 
         "ASSERT: Invalid model component index calculated for model entity id %i. meshComponentsIndex_ %i gl_InstanceIndex %i",
          int(modelEntityId),
            int(meshComponentsIndex_), int(gl_InstanceIndex));

    newPosition = TakeBoneIntoAccount(newPosition, position, modelComponentsIndex, uint(0));
    newPosition = TakeBoneIntoAccount(newPosition, position, modelComponentsIndex, uint(1));
    newPosition = TakeBoneIntoAccount(newPosition, position, modelComponentsIndex, uint(2));
    newPosition = TakeBoneIntoAccount(newPosition, position, modelComponentsIndex, uint(3));

    outUV = inUV;

    ASSERT_MSG(!any(isnan(newPosition)), "Calculated vertex position is nan.");

    gl_Position = camera.proj * camera.view * (vec4(newPosition.xyz, 1.0));
}