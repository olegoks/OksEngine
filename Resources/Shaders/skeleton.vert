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
    uint64_t nodeIds_[128];
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

layout(std430, set = 7, binding = 0) buffer EntityIdsToComponentIndices {
    EntityIdToComponentIndex idsToIndices[];
};

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 outUV;

layout(push_constant) uniform PushConstants {
    uint64_t entitiesNumber_; // Node bone entities number
};

uint64_t GetComponentIndexByEntityId(uint64_t entityId){

    //debugPrintfEXT("entitiesNumber %d, Bone entity index %d",entitiesNumber_, entityId);
    //TODO: use binary search
    for(uint i = 0; i < entitiesNumber_; i++){
        if(idsToIndices[i].entityId_ == entityId){
            ASSERT_MSG(entityId != INVALID_ENTITY_ID, "Invalid bone node entity id.");
            return idsToIndices[i].componentIndex_;
        }
    }
    //debugPrintfEXT("Component index is not found for entity %d", entityId);
    return 0;
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
    ASSERT_MSG(weight <= 1.0, "Invalid bone weight.");
    return weight > 0.00001;
}

void main() {

    vec4 position = vec4(inPosition, 1.0);

    vec4 newPosition = vec4(0, 0, 0, 0);

    //mat4 bone1Transform = positions_[bone1Pos];

    ModelNodeEntityIds modelNodeEntityIds = modelNodeEntityIds_[gl_InstanceIndex];

    if(IsBoneWeightSignificant(inBoneWeights[0])) {

        uint64_t nodeEntityId = modelNodeEntityIds.nodeIds_[inBoneIds[0]];

        uint boneIndex = uint(GetComponentIndexByEntityId(nodeEntityId));

        vec3 translate = vec3(positions_[boneIndex].position_.x, positions_[boneIndex].position_.y, positions_[boneIndex].position_.z);
        vec4 rotation = rotations_[boneIndex].rotation_;
        vec3 scale = vec3(scales_[boneIndex].scale_.x, scales_[boneIndex].scale_.y, scales_[boneIndex].scale_.z);


        mat4 boneInverseBindPoseMatrix = boneInverseBindPoseMatrices[boneIndex];
        mat4 boneWorldTransform = RTS_to_mat4_optimized(translate, rotation, scale);

        newPosition = newPosition + inBoneWeights[0] * boneWorldTransform * boneInverseBindPoseMatrix * position;
    }

    if(IsBoneWeightSignificant(inBoneWeights[1])) {

        uint64_t nodeEntityId = modelNodeEntityIds.nodeIds_[inBoneIds[1]];

        uint boneIndex = uint(GetComponentIndexByEntityId(nodeEntityId));

        vec3 translate = vec3(positions_[boneIndex].position_.x, positions_[boneIndex].position_.y, positions_[boneIndex].position_.z);
        vec4 rotation = rotations_[boneIndex].rotation_;
        vec3 scale = vec3(scales_[boneIndex].scale_.x, scales_[boneIndex].scale_.y, scales_[boneIndex].scale_.z);

        mat4 boneInverseBindPoseMatrix = boneInverseBindPoseMatrices[boneIndex];

        mat4 boneWorldTransform = RTS_to_mat4_optimized(translate, rotation, scale);

        newPosition = newPosition + inBoneWeights[1] * boneWorldTransform * boneInverseBindPoseMatrix* position;
    }

    if(IsBoneWeightSignificant(inBoneWeights[2])) {

        uint64_t nodeEntityId = modelNodeEntityIds.nodeIds_[inBoneIds[2]];

        uint boneIndex = uint(GetComponentIndexByEntityId(nodeEntityId));

        vec3 translate = vec3(positions_[boneIndex].position_.x, positions_[boneIndex].position_.y, positions_[boneIndex].position_.z);
        vec4 rotation = rotations_[boneIndex].rotation_;
        vec3 scale = vec3(scales_[boneIndex].scale_.x, scales_[boneIndex].scale_.y, scales_[boneIndex].scale_.z);

        mat4 boneInverseBindPoseMatrix = boneInverseBindPoseMatrices[boneIndex];

        mat4 boneWorldTransform = RTS_to_mat4_optimized(translate, rotation, scale);

        newPosition = newPosition + inBoneWeights[2] * boneWorldTransform * boneInverseBindPoseMatrix* position;
    }

    if(IsBoneWeightSignificant(inBoneWeights[3])) {

        uint64_t nodeEntityId = modelNodeEntityIds.nodeIds_[inBoneIds[3]];

        uint boneIndex = uint(GetComponentIndexByEntityId(nodeEntityId));
    
        vec3 translate = vec3(positions_[boneIndex].position_.x, positions_[boneIndex].position_.y, positions_[boneIndex].position_.z);
        vec4 rotation = rotations_[boneIndex].rotation_;
        vec3 scale = vec3(scales_[boneIndex].scale_.x, scales_[boneIndex].scale_.y, scales_[boneIndex].scale_.z);

        mat4 boneInverseBindPoseMatrix = boneInverseBindPoseMatrices[boneIndex];

        mat4 boneWorldTransform = RTS_to_mat4_optimized(translate, rotation, scale);

        newPosition = newPosition + inBoneWeights[3] * boneWorldTransform * boneInverseBindPoseMatrix *position;
    }


    //newPosition = newPosition + (float(inBoneWeights[1]) / 255.0) * bonesPalette.matrices[inBoneIds[1]] * position;
    //newPosition = newPosition + (float(inBoneWeights[2]) / 255.0) * bonesPalette.matrices[inBoneIds[2]] * position;
    //newPosition = newPosition + (float(inBoneWeights[3]) / 255.0) * bonesPalette.matrices[inBoneIds[3]] * position;

    //newPosition = newPosition + (float(inBoneWeights[0]) / 255.0) * bonesPalette.matrices[inBoneIds[0]] * position;
    ///newPosition = newPosition + (float(inBoneWeights[1]) / 255.0) * bonesPalette.matrices[inBoneIds[1]] * position;
    //newPosition = newPosition + (float(inBoneWeights[2]) / 255.0) * bonesPalette.matrices[inBoneIds[2]] * position;
    //newPosition = newPosition + (float(inBoneWeights[3]) / 255.0) * bonesPalette.matrices[inBoneIds[3]] * position;


    outUV = inUV;

    ASSERT_MSG(!any(isnan(newPosition)), "Calculated vertex position is nan.");

    gl_Position = camera.proj * camera.view * (vec4(newPosition.xyz, 1.0));
}