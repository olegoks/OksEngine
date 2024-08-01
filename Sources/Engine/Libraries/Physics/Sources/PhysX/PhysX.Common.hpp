#pragma once


#include <Math.Common.hpp>
#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

namespace PhysX
{
    inline glm::mat4 convertToGlmMat4(const physx::PxMat44& pxMatrix) {
        glm::mat4 glmMatrix;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                glmMatrix[i][j] = pxMatrix[i][j];
            }
        }

        return glmMatrix;
    }

    inline physx::PxMat44 convertToPxMat44(const glm::mat4& glmMatrix) {
    	physx::PxMat44 pxMatrix;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                pxMatrix[i][j] = glmMatrix[i][j];
            }
        }

        return pxMatrix;
    }

}