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

	class AssimpScene {
	public:

		struct CreateInfo {
			std::vector<Common::Byte> data_;
		};
		using CI = CreateInfo;

		AssimpScene(const CI& ci) {

			importer_ = std::make_shared<Assimp::Importer>();

			const unsigned int flags =
				aiProcess_Triangulate |
				aiProcess_GenNormals |
				aiProcess_FlipUVs |
				aiProcess_LimitBoneWeights |
				aiProcess_PopulateArmatureData |
				aiProcess_Debone |
				aiProcess_ConvertToLeftHanded;

			importer_->SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, VertexMaxBonesNumber);

			const aiScene* readScene = importer_->ReadFileFromMemory(
				ci.data_.data(),
				ci.data_.size(),
				flags,
				"gltf"
			);

			auto errorString = std::string(importer_->GetErrorString());

			ASSERT_FMSG(readScene && readScene->mRootNode,
				"Failed to load model: {}", errorString);

			scene_ = readScene;

			//Get bone node names.
			{
				std::set<std::string> bonesSet;//To discard same bone names.
				for (Common::Index i = 0; i < scene_->mNumMeshes; i++) {
					const aiMesh* mesh = scene_->mMeshes[i];
					for (Common::Index j = 0; j < mesh->mNumBones; j++) {

						const aiBone* bone = mesh->mBones[j];
						const aiString& boneName = bone->mName;
						bonesSet.insert(boneName.C_Str());
						nameToBone_[boneName.C_Str()] = bone;
						const aiNode* boneNode = scene_->mRootNode->FindNode(boneName.C_Str());
						nameToBoneNode_[boneName.C_Str()] = boneNode;

					}
				}
				//Move to std::vector to have definite order.
				for (auto& boneName : bonesSet) {
					boneNames_.push_back(boneName);
				}
			}


			{
				std::function<void(aiNode* node)> processNode = [&](aiNode* node) {

					boneNameToBone_[node->mName.C_Str()] = node;

					for (Common::Index i = 0; i < node->mNumChildren; i++) {
						aiNode* childrenNode = node->mChildren[i];
						processNode(childrenNode);
					}

					};

				processNode(scene_->mRootNode);

			}

			{

				std::function<void(aiNode* node)> processNode = [&](aiNode* node) {

					for (Common::Index i = 0; i < std::clamp((size_t)scene_->mNumAnimations, (size_t)0, Animation::Model::AnimationsMaxNumber); i++) {
						aiAnimation* animation = scene_->mAnimations[i];

						for (Common::Index channelIndex = 0; channelIndex < animation->mNumChannels; channelIndex++) {
							aiNodeAnim* nodeAnim = animation->mChannels[channelIndex];
							if (nodeAnim->mNodeName == node->mName) {
								auto nodeIt = nodeToNodeAnim_.find(node);
								if (nodeIt != nodeToNodeAnim_.end()) {
									nodeIt->second.push_back(nodeAnim);
								}
								else {
									nodeToNodeAnim_[node] = std::vector{ nodeAnim };
								}
							}
						}
					}

					for (Common::Index i = 0; i < node->mNumChildren; i++) {
						aiNode* childrenNode = node->mChildren[i];
						processNode(childrenNode);
					}

					};

				processNode(scene_->mRootNode);
			}



		}

		//auto getNodeFullName = [resource__Path3](const aiScene* scene, const aiNode* node) {
		//	std::string nodeName;

		//	const aiNode* currentNode = node;
		//	while (currentNode != nullptr) {

		//		nodeName = ("/") + nodeName;
		//		nodeName = currentNode->mName.C_Str() + nodeName;

		//		currentNode = currentNode->mParent;
		//	}

		//	nodeName = resource__Path3->path_ + "/" + scene->mName.C_Str() + "/" + nodeName;
		//	return nodeName;
		//	};
		
		bool IsThereBoneWithName(const std::string& boneName) {
			return nameToBone_.contains(boneName);
		}

		bool IsNodeHasAnimation(const aiNode* node) {
			return nodeToNodeAnim_.contains(node);
		}

		const aiNode* GetRootNode() {
			return scene_->mRootNode;
		}

		void ForEachNode(std::function<void(const aiNode* node)>&& processNode) {
			std::function<void(const aiNode*)> processNodeHier = [&](const aiNode* node) {
				if (node == nullptr) {
					return;
				}
				processNode(node);
				for (const aiNode* childrenNode : GetChildrenNodes(node)) {
					processNodeHier(childrenNode);
				}

				};
			processNodeHier(GetRootNode());
		}

		void ForEachMesh(std::function<void(const aiMesh* node)>&& processMesh) {
			for (Common::Index i = 0; i < scene_->mNumMeshes; i++) {
				aiMesh* mesh = scene_->mMeshes[i];
				processMesh(mesh);
			}
		}

		std::string GetNodeFullName(const aiNode* node) {
			std::string nodeName;

			const aiNode* currentNode = node;

			while (currentNode != nullptr) {

				nodeName = ("/") + nodeName;
				nodeName = currentNode->mName.C_Str() + nodeName;

				currentNode = currentNode->mParent;
			}

			nodeName = std::string{ scene_->mName.C_Str() } + "/" + nodeName;

			return nodeName;
		}

		const aiNode* GetBoneNodeByName(const std::string& boneNodeName) {
			return nameToBoneNode_[boneNodeName];
		}

		const aiBone* GetBoneByName(const std::string& boneName) {
			return nameToBone_[boneName];
		}

		void ForEachBoneNode(std::function<void(const aiNode* node)>&& processNode) {
			for (auto& [boneName, bone] : boneNameToBone_) {
				processNode(bone);
			}
		}

		std::vector<const aiNode*> GetChildrenNodes(const aiNode* node) const {
			if (node->mNumChildren > 0) {
				return std::vector<const aiNode*>{ node->mChildren, node->mChildren + node->mNumChildren };
			}
			return std::vector<const aiNode*>{};
			
		}
		const aiScene* scene_ = nullptr;

		Common::Size GetMeshesNumber() {
			return scene_->mNumMeshes;
		}

		Common::Size GetAnimationsNumber() {
			return scene_->mNumAnimations;
		}

		std::string GetSceneName() {
			return scene_->mName.C_Str();
		}

		std::string GetMeshFullName(const aiMesh* mesh) {
			return GetSceneName() + mesh->mName.C_Str();
		}
	private:
		
		std::unordered_map<std::string, const aiBone*> nameToBone_;
		std::unordered_map<std::string, const aiNode*> nameToBoneNode_;
		std::unordered_map<std::string, const aiNode*> boneNameToBone_;
		std::unordered_map<const aiNode*, std::vector<aiNodeAnim*>> nodeToNodeAnim_;
		std::vector<std::string> boneNames_;
		std::shared_ptr<Assimp::Importer> importer_ = nullptr;
	};

}