#pragma once 
#include <glm/glm.hpp>
#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>


namespace OksEngine::Render::Model {

	static inline glm::mat4 AssimpToGlmMatrix(const aiMatrix4x4& aiMatrix) {

		aiVector3D position3D;
		aiQuaternion rotation3D;
		aiVector3D scale3D;
		aiMatrix4x4 flipYtoZ;
		aiMatrix.Decompose(scale3D, rotation3D, position3D);

		const glm::mat4 translateMatrix = glm::mat4{ glm::translate(glm::vec3(position3D.x, position3D.y, position3D.z)) };
		const glm::mat4 rotationMatrix = glm::toMat4(glm::quat{ rotation3D.w, rotation3D.x, rotation3D.y, rotation3D.z });;
		const glm::mat4 scaleMatrix = glm::scale(glm::vec3(scale3D.x, scale3D.y, scale3D.z));

		glm::mat4 transform = glm::mat4{ 1 }  *translateMatrix * rotationMatrix * scaleMatrix;

		return transform;
	}

}