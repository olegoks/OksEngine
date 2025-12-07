#pragma once
#include <Render\auto_OksEngine.Model.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stb_image.h>

#include <glm/gtc/quaternion.hpp>



//DEBUG
#include <random>

#include <Render/Texture/auto_OksEngine.Texture.hpp>


#include <Common/auto_OksEngine.Debug.Position3D.hpp>
#include <pix3.h>



namespace OksEngine
{


	void EditAnimationInProgress(std::shared_ptr<ECS2::World> ecsWorld, AnimationInProgress* animationInProgress) {
		ImGui::PushID(AnimationInProgress::GetTypeId());

		ImGui::TextDisabled("Animation index %d", animationInProgress->animationIndex_);
		ImGui::TextDisabled("Duration in ticks %f", animationInProgress->durationInTicks_);
		ImGui::TextDisabled("Current tick %f", animationInProgress->currentTick_);
		ImGui::TextDisabled("Ticks per second %f", animationInProgress->ticksPerSecond_);
		ImGui::TextDisabled("Duration in seconds %f", animationInProgress->durationInTicks_ / animationInProgress->ticksPerSecond_);

		//animationInProgress->durationInTicks_
		double min = 0.0;
		ImGui::SliderScalar("", ImGuiDataType_Double, &animationInProgress->currentTick_, &min, &animationInProgress->durationInTicks_, "%.3f");
		//ImGui::SliderDouble("Scale", &scale, 1.0f, max_scale, "%.3f", ImGuiSliderFlags_Logarithmic);

		ImGui::PopID();
	}

	void EditModelAnimations(std::shared_ptr<ECS2::World> ecsWorld, ModelAnimations* modelAnimations) {
		ImGui::PushID(ModelAnimations::GetTypeId());
		for (const auto& animation : modelAnimations->animations_) {
			ImGui::Indent(20.f);
			ImGui::TextDisabled("Animation name %s", animation.name_.c_str());
			ImGui::TextDisabled("Duration in ticks %f", animation.durationInTicks_);
			ImGui::TextDisabled("Ticks per second %f", animation.ticksPerSecond_);
			ImGui::TextDisabled("Duration in seconds %f", animation.durationInTicks_ / animation.ticksPerSecond_);
			ImGui::Unindent(20.f);
			ImGui::Separator();
		}
		ImGui::PopID();
	}

	namespace Render::Mdl {
		void EditModelNodeEntityIds(std::shared_ptr<ECS2::World> ecsWorld, Render::Mdl::ModelNodeEntityIds* modelNodeEntityIds) {
			ImGui::PushID(Render::Mdl::ChildModelNodeEntities::GetTypeId());
			for (ECS2::Entity::Id modelNodeEntityId : modelNodeEntityIds->nodeEntityIds_) {
				if (modelNodeEntityId.IsInvalid()) {
					const std::string idString = std::to_string(modelNodeEntityId);
					ImGui::TextDisabled(idString.c_str());
					continue;
				}
				ImGui::Indent(20.f);
				EditEntity(ecsWorld, modelNodeEntityId);
				ImGui::Unindent(20.0f);
			}
			ImGui::PopID();
		}
	}

	void BindModelAnimations(::Lua::Context& context) {
		context.GetGlobalNamespace()
			.beginClass<ModelAnimations>("ModelAnimations")
			.addFunction("GetAnimationsNumber", [](const ModelAnimations* modelAnimations) {
			ASSERT(!modelAnimations->animations_.empty());
			return modelAnimations->animations_.size();
				})
			.addFunction("GetAnimationName", [](const ModelAnimations* modelAnimations, Common::UInt64 animationIndex) {
			ASSERT(animationIndex < modelAnimations->animations_.size());
			return modelAnimations->animations_[animationIndex];
				})
			.endClass();
	}

	void BindRunModelAnimation(::Lua::Context& context) {
		context.GetGlobalNamespace()
			.beginClass<RunModelAnimation>("RunModelAnimation")
			.endClass();
	}
	namespace Render::Mdl {

		void BindModelEntity(::Lua::Context& context)
		{

			context.GetGlobalNamespace()
				.beginClass<ModelEntity>("Render_Mdl_ModelEntity")
				.addProperty("id",
					[](const ModelEntity* modelEntity) {
						return modelEntity->id_.GetRawValue();
					},
					[](ModelEntity* modelEntity, ECS2::Entity::Id::ValueType value) {
						return modelEntity->id_ = value;
					}
				)
				.endClass();
		};


		void ProcessModel::Update(

			ECS2::Entity::Id entity0id,
			const Clock* clock0,
			const TimeSinceEngineStart* timeSinceEngineStart0,

			ECS2::Entity::Id entityId1,
			const Mdl::Model* model1,
			const WorldPosition3D* worldPosition3D1,
			const WorldRotation3D* worldRotation3D1,
			const WorldScale3D* worldScale3D1,
			const Render::Mdl::ModelDataEntityId* render__Model__ModelDataEntityId1,
			const Mdl::ChildModelNodeEntities* childModelNodeEntities1) {

			ECS2::Entity::Id modelEntityId = entityId1;

			BEGIN_PROFILE("Processing model with entity id %d.", modelEntityId);

			auto components = GetComponents<
				WorldPosition3D,
				WorldRotation3D,
				WorldScale3D,
				RunModelAnimation,
				PauseAnimation,
				AnimationInProgress>(modelEntityId);

			auto modelComponentsFilter = GetComponentsFilter(modelEntityId);

			const auto* modelDataEntityId = render__Model__ModelDataEntityId1;
			const auto* modelAnimations = GetComponent<ModelAnimations>(modelDataEntityId->modelDataEntityId_);


			const auto* runModelAnimation = std::get<RunModelAnimation*>(components);
			const bool needToRunAnimation = modelComponentsFilter.IsSet<RunModelAnimation>();

			auto* animationInProgress = std::get<AnimationInProgress*>(components);
			const bool needToProcessAnimation = modelComponentsFilter.IsSet<AnimationInProgress>();

			const auto* pauseAnimation = std::get<PauseAnimation*>(components);
			const bool animationPaused = modelComponentsFilter.IsSet<PauseAnimation>();

			ASSERT(!(needToRunAnimation && needToProcessAnimation));

			//PROSESS RUNNING ANIMATION
			ModelAnimation runningModelAnimation;
			float currentAnimationStage = 0.0;

			if (needToProcessAnimation) {
				currentAnimationStage = animationInProgress->currentTick_ / animationInProgress->durationInTicks_;
			}

			bool isAnimationEnded = false;

			if (needToProcessAnimation) {
				runningModelAnimation = modelAnimations->animations_[animationInProgress->animationIndex_];
			}

			if (needToProcessAnimation && !animationPaused) {

				//Calculate current animation tick.

				const Common::UInt64 msSinceAnimationStart = timeSinceEngineStart0->microseconds_ - animationInProgress->animationStartTimeSinceEngineStart_;
				const float secSinceAnimationStart = msSinceAnimationStart / 1000000.0;
				const float ticksSinceAnimationStart = runningModelAnimation.ticksPerSecond_ * secSinceAnimationStart;
				const float animationDurationInTicks = runningModelAnimation.durationInTicks_;

				// Calculate current animation state from [0..1].
				currentAnimationStage = ticksSinceAnimationStart / animationDurationInTicks;

				isAnimationEnded = currentAnimationStage > 1.0;

				if (isAnimationEnded) {
					//Animation ended.

					Common::UInt64 animationIndex = animationInProgress->animationIndex_;

					RemoveComponent<AnimationInProgress>(modelEntityId);

					//const Common::Size animationsNumber = modelAnimations->animations_.size();

					//std::random_device rd;
					//std::mt19937 gen(rd());

					//Common::Index a = 0, b = animationsNumber - 1;
					//std::uniform_int_distribution<> dist(a, b);
					//const Common::Index randomAnimationIndex = dist(gen);
					//const ModelAnimation& randomModelAnimation = modelAnimations->animations_[randomAnimationIndex];


					////animationIndex++;
					////if (animationIndex == animationsNumber) {
					////	animationIndex = 0;
					////}

					////const ModelAnimation& randomModelAnimation = modelAnimations0->animations_[animationIndex];

					//CreateComponent<RunModelAnimation>(modelEntityId, randomModelAnimation.name_);
				}
				else {
					animationInProgress->currentTick_ = currentAnimationStage * animationInProgress->durationInTicks_;
				}
			}

			//PROSESS RUNNING ANIMATION

			//PROCESS ANIMATION START

			Common::UInt32 animationIndex = 0;
			auto modelAnimationIt = modelAnimations->animations_.cend();
			ECS2::ComponentsFilter animationComponentsFilter;
			animationComponentsFilter.SetBits<Animation::Mdl::Node::Animations>();
			if (needToRunAnimation) {
				//Find needed animation.
				modelAnimationIt = std::find_if(
					modelAnimations->animations_.cbegin(),
					modelAnimations->animations_.cend(),
					[&](const ModelAnimation& modelAnimation) {
						return modelAnimation.name_ == runModelAnimation->animationName_;
					});
				ASSERT(modelAnimationIt != modelAnimations->animations_.cend());
				//Get index of animation, shader that will calculate animation will use this index to access animation data.
				animationIndex = std::distance(modelAnimations->animations_.cbegin(), modelAnimationIt);

				CreateComponent<AnimationInProgress>(
					modelEntityId,
					//runModelAnimation->animationName_,
					animationIndex,
					0.0,
					modelAnimationIt->durationInTicks_,
					modelAnimationIt->ticksPerSecond_,
					timeSinceEngineStart0->microseconds_);

				RemoveComponent<RunModelAnimation>(modelEntityId);


			}
			//PROCESS ANIMATION START


			{
				/*auto nodesComponents = GetComponents<RENDER__MODEL__NODE>();
				childModelNodeEntities0->childEntityIds_;*/

			}


			std::function<void(ECS2::Entity::Id, const glm::fvec3&, const glm::quat&, const glm::fvec3&)> processModelNode
				= [&](ECS2::Entity::Id nodeEntityId,
					const glm::fvec3& parentPosition3D,
					const glm::quat& parentRotation3D,
					const glm::fvec3& parentScale3D) {


						BEGIN_PROFILE("Processing model node with entity id %d", nodeEntityId);

						auto components = GetComponents<
							LocalPosition3D,
							WorldPosition3D,
							LocalRotation3D,
							WorldRotation3D,
							LocalScale3D,
							WorldScale3D,
							Animation::Mdl::Node::RunningState,
							ChildModelNodeEntities
						>(nodeEntityId);

						//PROSESS RUNNING ANIMATION
						if (needToProcessAnimation) {


							auto* animationRunningState = std::get<Animation::Mdl::Node::RunningState*>(components);
							auto* childModelNodeEntities = std::get<ChildModelNodeEntities*>(components);

							if (animationRunningState != nullptr) {
								animationRunningState->animationIndex_ = animationInProgress->animationIndex_;
								animationRunningState->animationDuration_ = runningModelAnimation.durationInTicks_;
								animationRunningState->currentTime_ = currentAnimationStage;
								if (isAnimationEnded) {
									RemoveComponent<Animation::Mdl::Node::RunningState>(nodeEntityId);
									animationRunningState->currentTime_ = 1.0;
								}

							}
						}
						//PROSESS RUNNING ANIMATION

						//PROSESS START ANIMATION
						if (needToRunAnimation) {
							const ECS2::ComponentsFilter entityComponentFilter = GetComponentsFilter(nodeEntityId);

							ASSERT_MSG(
								entityComponentFilter.IsSet<ModelNode>(),
								"Entity in model hier is not node, check hier construction.");

							//Check if node has animation.
							if (animationComponentsFilter.IsSubsetOf(entityComponentFilter)) {
								CreateComponent<Animation::Mdl::Node::RunningState>(
									nodeEntityId,
									animationIndex,
									modelAnimationIt->durationInTicks_,
									0.0);
							}
						}
						//PROSESS START ANIMATION

						//UPDATE WORLD POSITION

						const auto* localNodeRotation3D = std::get<LocalRotation3D*>(components);
						auto* worldNodeRotation3D = std::get<WorldRotation3D*>(components);

						const glm::quat worldRotationQuat = parentRotation3D * glm::quat(
							localNodeRotation3D->w_,
							localNodeRotation3D->x_,
							localNodeRotation3D->y_,
							localNodeRotation3D->z_);

						worldNodeRotation3D->w_ = worldRotationQuat.w;
						worldNodeRotation3D->x_ = worldRotationQuat.x;
						worldNodeRotation3D->y_ = worldRotationQuat.y;
						worldNodeRotation3D->z_ = worldRotationQuat.z;

						const auto* localNodePosition3D = std::get<LocalPosition3D*>(components);
						auto* worldNodePosition3D = std::get<WorldPosition3D*>(components);

						{
							const glm::vec3 worldNodePosition3DVec = parentPosition3D + parentRotation3D * (parentScale3D * glm::vec3{ localNodePosition3D->x_, localNodePosition3D->y_, localNodePosition3D->z_ });

							ASSERT_FMSG(
								!std::isnan(worldNodePosition3DVec.x) &&
								!std::isnan(worldNodePosition3DVec.y) &&
								!std::isnan(worldNodePosition3DVec.z), "");

							worldNodePosition3D->x_ = worldNodePosition3DVec.x;
							worldNodePosition3D->y_ = worldNodePosition3DVec.y;
							worldNodePosition3D->z_ = worldNodePosition3DVec.z;
						}


						const auto* localNodeScale3D = std::get<LocalScale3D*>(components);

						auto* worldNodeScale3D = std::get<WorldScale3D*>(components);

						worldNodeScale3D->x_ = parentScale3D.x * localNodeScale3D->x_;
						worldNodeScale3D->y_ = parentScale3D.y * localNodeScale3D->y_;
						worldNodeScale3D->z_ = parentScale3D.z * localNodeScale3D->z_;


						//UPDATE WORLD POSITION
						auto* childModelNodeEntities = std::get<ChildModelNodeEntities*>(components);

						if (childModelNodeEntities != nullptr) {

							const glm::fvec3 currentNodePosition3D = { worldNodePosition3D->x_, worldNodePosition3D->y_, worldNodePosition3D->z_ };
							const glm::quat currentNodeRotation3D = { worldNodeRotation3D->w_, worldNodeRotation3D->x_, worldNodeRotation3D->y_, worldNodeRotation3D->z_ };
							const glm::fvec3 currentNodeScale3D = { worldNodeScale3D->x_, worldNodeScale3D->y_, worldNodeScale3D->z_ };

							const std::vector<ECS2::Entity::Id>& childEntityIds = childModelNodeEntities->childEntityIds_;
							for (ECS2::Entity::Id childModelNodeEntityId : childEntityIds) {

								processModelNode(childModelNodeEntityId, currentNodePosition3D, currentNodeRotation3D, currentNodeScale3D);
							}
						}
						END_PROFILE();
				};

			//auto& worldPosition3D0 = std::get<WorldPosition3D*>(components);
			//auto& worldRotation3D0 = std::get<WorldRotation3D*>(components);
			//auto& worldScale3D0 = std::get<WorldScale3D*>(components);
			const glm::fvec3 position3D = { worldPosition3D1->x_, worldPosition3D1->y_, worldPosition3D1->z_ };
			const glm::quat rotation3D = { worldRotation3D1->w_, worldRotation3D1->x_, worldRotation3D1->y_, worldRotation3D1->z_ };
			const glm::fvec3 scale3D = { worldScale3D1->x_, worldScale3D1->y_, worldScale3D1->z_ };
			const std::vector<ECS2::Entity::Id>& childEntityIds = childModelNodeEntities1->childEntityIds_;
			for (ECS2::Entity::Id childModelNodeEntityId : childEntityIds) {
				processModelNode(childModelNodeEntityId, position3D, rotation3D, scale3D);
			}

			END_PROFILE();

		}
	}

	//void ProcessModelAnimation::Update(
	//	ECS2::Entity::Id entity0id,
	//	const Model* model0,
	//	const ModelAnimations* modelAnimations0,
	//	const AnimationInProgress* animationInProgress0,
	//	const ChildModelNodeEntities* childModelNodeEntities0,

	//	ECS2::Entity::Id entity1id,
	//	const Clock* clock1,
	//	const TimeSinceEngineStart* timeSinceEngineStart1) {

	//	//

	//	//const ModelAnimation& modelAnimation = modelAnimations0->animations_[animationInProgress0->animationIndex_];
	//	////Calculate current animation tick.
	//	//const Common::UInt64 msSinceAnimationStart = timeSinceEngineStart1->microseconds_ - animationInProgress0->animationStartTimeSinceEngineStart_;
	//	//const float secSinceAnimationStart = msSinceAnimationStart / 1000000.0;
	//	//const float ticksSinceAnimationStart = modelAnimation.ticksPerSecond_ * secSinceAnimationStart;
	//	//const float animationDurationInTicks = modelAnimation.durationInTicks_;

	//	//// Calculate current animation state from [0..1].
	//	//const float currentAnimationStage = ticksSinceAnimationStart / animationDurationInTicks;

	//	//const bool isAnimationEnded = currentAnimationStage > 1.0;

	//	//if (isAnimationEnded) {
	//	//	//Animation ended.
	//	//	RemoveComponent<AnimationInProgress>(entity0id);

	//	//	const Common::Size animationsNumber = modelAnimations0->animations_.size();

	//	//	std::random_device rd;
	//	//	std::mt19937 gen(rd());

	//	//	Common::Index a = 0, b = animationsNumber - 1;
	//	//	std::uniform_int_distribution<> dist(a, b);
	//	//	const Common::Index randomAnimationIndex = dist(gen);
	//	//	const ModelAnimation& randomModelAnimation = modelAnimations0->animations_[randomAnimationIndex];

	//	//	CreateComponent<RunModelAnimation>(entity0id, randomModelAnimation.name_);
	//	//}

	//	//std::function<void(ECS2::Entity::Id)> processModelNode
	//	//	= [&](ECS2::Entity::Id nodeEntityId) {

	//	//	auto components = world_->GetComponents<
	//	//		Animation::Model::Node::RunningState,
	//	//		ChildModelNodeEntities
	//	//	>(nodeEntityId);

	//	//	auto* animationRunningState = std::get<Animation::Model::Node::RunningState*>(components);
	//	//	auto* childModelNodeEntities = std::get<ChildModelNodeEntities*>(components);

	//	//	if (animationRunningState != nullptr) {
	//	//		animationRunningState->animationDuration_ = modelAnimation.durationInTicks_;
	//	//		animationRunningState->currentTime_ = currentAnimationStage;
	//	//		if (isAnimationEnded) {
	//	//			RemoveComponent<Animation::Model::Node::RunningState>(nodeEntityId);
	//	//			animationRunningState->currentTime_ = 1.0;
	//	//		}

	//	//	}

	//	//	if (childModelNodeEntities != nullptr) {

	//	//		const std::vector<ECS2::Entity::Id>& childEntityIds = childModelNodeEntities->childEntityIds_;
	//	//		for (ECS2::Entity::Id childModelNodeEntityId : childEntityIds) {
	//	//			processModelNode(childModelNodeEntityId);
	//	//		}
	//	//	}

	//	//	};

	//	//for (ECS2::Entity::Id childModelNodeEntityId : childModelNodeEntities0->childEntityIds_) {

	//	//	processModelNode(childModelNodeEntityId);
	//	//}

	//}

	class MyIOStream : public Assimp::IOStream {
	public:
		MyIOStream(char* data, size_t length) : mData(data), mLength(length), mPos(0) { }

		size_t Read(void* pvBuffer, size_t pSize, size_t pCount) override {
			size_t readSize = pSize * pCount;
			if (mPos + readSize > mLength) readSize = mLength - mPos;
			memcpy(pvBuffer, mData + mPos, readSize);
			mPos += readSize;
			return readSize;
		}

		size_t Write(const void* /*pvBuffer*/, size_t /*pSize*/, size_t /*pCount*/) override {
			return 0; // Writing is not supported
		}

		aiReturn Seek(size_t pOffset, aiOrigin pOrigin) override {
			switch (pOrigin) {
			case aiOrigin_SET: mPos = pOffset; break;
			case aiOrigin_CUR: mPos += pOffset; break;
			case aiOrigin_END: mPos = mLength - pOffset; break;
			default: return aiReturn_FAILURE;
			}
			return aiReturn_SUCCESS;
		}

		size_t Tell() const override {
			return mPos;
		}

		size_t FileSize() const override {
			return mLength;
		}

		void Flush() override { }

	private:
		char* mData;
		size_t mLength;
		size_t mPos;
	};

	class GltfIOSystem : public Assimp::IOSystem {
	public:

		GltfIOSystem(std::shared_ptr<AsyncResourceSubsystem> resourceSystem) :
			resourceSystem_{ resourceSystem } {

		}

		bool Exists(const char* fileName) const override {

			//resourceSystem_->
			//return fileName == gltfName_;
			return true;
		}

		char getOsSeparator() const override {
			return '/';  // Use forward slash as the separator
		}

		Assimp::IOStream* Open(const char* fileName, const char* /*pMode*/ = "rb") override {
			auto data = resourceSystem_->GetResourceSynch(Subsystem::Type::ChildThread, "Root/" + std::string(fileName));
			data_ = std::move(data.data_);
			return new MyIOStream(data_.GetData(), data_.GetSize());
		}

		void Close(Assimp::IOStream* pFile) override {
			delete pFile;
		}

		std::shared_ptr<AsyncResourceSubsystem> resourceSystem_;
		DS::Vector<Common::Byte> data_;
		Common::Size size_;
	};

	namespace Ai {

		void CreateCache::Update() {

			const ECS2::Entity::Id cacheEntityId = CreateEntity();

			CreateComponent<Cache>(cacheEntityId, std::map<std::string, std::shared_ptr<const aiScene>>{});

		}

		void CreateModel::Update(
			ECS2::Entity::Id entity0id,
			Ai::Cache* ai__Cache0,

			ECS2::Entity::Id entity1id,
			const Render::Mdl::Msh::MeshsController* meshsController1,
			Render::Mdl::Msh::MeshNameToEntity* meshNameToEntity1,

			ECS2::Entity::Id entity2id,
			const ResourceSystem* resourceSystem2,

			ECS2::Entity::Id entity3id,
			const Render::Mdl::ModelFile* modelFile3,

			ECS2::Entity::Id entity4id,
			const Render::Mdl::DataController* render__Model__Data__DataController4,
			Render::Mdl::ModelNameToModelDataEntityId* render__Model__Data__ModelNameToDataEntityId4) {


			auto getNodeFullName = [modelFile3](const aiScene* scene, const aiNode* node) {
				std::string nodeName;

				const aiNode* currentNode = node;
				while (currentNode != nullptr) {

					nodeName = ("/") + nodeName;
					nodeName = currentNode->mName.C_Str() + nodeName;

					currentNode = currentNode->mParent;
				}

				nodeName = modelFile3->fileName_ + "/" + scene->mName.C_Str() + "/" + nodeName;
				return nodeName;
				};


			auto aiMatrixToGlmMatrix = [](const aiMatrix4x4& aiMatrix) {

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
				};


			const ECS2::Entity::Id modelEntityId = entity3id;

			const aiScene* scene = nullptr;

			//GET OR CREATE aiScene*
			if (ai__Cache0->nametToScene_.contains(modelFile3->fileName_)) {
				scene = ai__Cache0->nametToScene_[modelFile3->fileName_].get();
			}
			else {

				Resources::ResourceData resourceData = resourceSystem2->system_->GetResourceSynch(
					Subsystem::Type::ChildThread,
					"Root/" + modelFile3->fileName_);

				auto importer = std::make_shared<Assimp::Importer>();

				importer->SetIOHandler(new GltfIOSystem{ resourceSystem2->system_ });

				const unsigned int flags =
					aiProcess_Triangulate |
					aiProcess_GenNormals |
					aiProcess_FlipUVs |
					aiProcess_LimitBoneWeights |
					aiProcess_PopulateArmatureData |
					aiProcess_Debone |
					aiProcess_ConvertToLeftHanded;

				importer->SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, VertexMaxBonesNumber);

				const aiScene* readScene = importer->ReadFileFromMemory(
					resourceData.GetData<Common::Byte>(),
					resourceData.GetSize(),
					flags,
					"gltf"
				);

				auto errorString = std::string(importer->GetErrorString());

				ASSERT_FMSG(readScene && readScene->mRootNode,
					"Failed to load model: {}", errorString);

				std::shared_ptr<const aiScene> scenePtr(readScene, [importer](const aiScene* scene) { importer->FreeScene(); });

				ai__Cache0->nametToScene_[modelFile3->fileName_] = scenePtr;

				scene = readScene;
			}

			CreateComponent<Render::Mdl::Model>(modelEntityId);


			//Get bone node names.
			std::set<std::string> bonesSet;//To discard same bone names.

			std::map<std::string, const aiBone*> nameToBone;
			std::map<std::string, const aiNode*> nameToBoneNode;

			{
				for (Common::Index i = 0; i < scene->mNumMeshes; i++) {
					const aiMesh* mesh = scene->mMeshes[i];
					for (Common::Index j = 0; j < mesh->mNumBones; j++) {

						const aiBone* bone = mesh->mBones[j];
						const aiString& boneName = bone->mName;
						bonesSet.insert(boneName.C_Str());
						nameToBone[boneName.C_Str()] = bone;
						const aiNode* boneNode = scene->mRootNode->FindNode(boneName.C_Str());
						nameToBoneNode[boneName.C_Str()] = boneNode;

					}
				}
			}
			//Move to std::vector to have definite order.
			std::vector<std::string> boneNames;
			for (auto& boneName : bonesSet) {
				boneNames.push_back(boneName);
			}


			std::map<std::string, aiNode*> boneNameToBone;
			{
				std::function<void(aiNode* node)> processNode = [&](aiNode* node) {

					boneNameToBone[node->mName.C_Str()] = node;

					for (Common::Index i = 0; i < node->mNumChildren; i++) {
						aiNode* childrenNode = node->mChildren[i];
						processNode(childrenNode);
					}

					};

				processNode(scene->mRootNode);

			}

			std::map<aiNode*, std::vector<aiNodeAnim*>> nodeToNodeAnim;
			{

				std::function<void(aiNode* node)> processNode = [&](aiNode* node) {

					for (Common::Index i = 0; i < std::clamp((size_t)scene->mNumAnimations, (size_t)0, Animation::Mdl::AnimationsMaxNumber); i++) {
						aiAnimation* animation = scene->mAnimations[i];

						for (Common::Index channelIndex = 0; channelIndex < animation->mNumChannels; channelIndex++) {
							aiNodeAnim* nodeAnim = animation->mChannels[channelIndex];
							if (nodeAnim->mNodeName == node->mName) {
								auto nodeIt = nodeToNodeAnim.find(node);
								if (nodeIt != nodeToNodeAnim.end()) {
									nodeIt->second.push_back(nodeAnim);
								}
								else {
									nodeToNodeAnim[node] = std::vector{ nodeAnim };
								}
							}
						}
					}

					for (Common::Index i = 0; i < node->mNumChildren; i++) {
						aiNode* childrenNode = node->mChildren[i];
						processNode(childrenNode);
					}

					};

				processNode(scene->mRootNode);
			}


			decltype(Render::Mdl::ModelNodeEntityIds::nodeEntityIds_) modelNodeEntityIds;
			modelNodeEntityIds.fill(ECS2::Entity::Id::invalid_);

			std::map<const aiNode*, ECS2::Entity::Id> nodeToEntityId;


			[[maybe_unused]]
			Common::Size nodesNumber = 0;
			std::vector<ECS2::Entity::Id> foundNodesEntityIds;
			{

				std::function<void(const aiScene*, aiNode*)> createNodeEntity = [&](const aiScene* scene, aiNode* node) {

					const bool isNodeBone = nameToBone.contains(node->mName.C_Str());
					const bool isAnimatedNode = nodeToNodeAnim.contains(node);

					ECS2::Entity::Id nodeEntityId = ECS2::Entity::Id::invalid_;

					nodeEntityId = CreateEntity<RENDER__MDL__NODE>();

					foundNodesEntityIds.push_back(nodeEntityId);
					nodeToEntityId[node] = nodeEntityId;


					for (Common::Index i = 0; i < node->mNumChildren; i++) {
						aiNode* childrenNode = node->mChildren[i];
						createNodeEntity(scene, childrenNode);
					}

					};

				createNodeEntity(scene, scene->mRootNode);

				ASSERT(foundNodesEntityIds.size() <= modelNodeEntityIds.max_size());
				std::copy(
					foundNodesEntityIds.begin(),
					foundNodesEntityIds.end(),
					modelNodeEntityIds.begin());
			}


			CreateComponent<Render::Mdl::ModelNodeEntityIds>(modelEntityId, modelNodeEntityIds);

			decltype(Render::Mdl::BoneNodeEntities::boneEntityIds_) boneEntityIds;
			boneEntityIds.fill(ECS2::Entity::Id::invalid_);
			for (Common::Index i = 0; i < boneNames.size(); i++) {
				const auto& boneName = boneNames[i];
				boneEntityIds[i] = nodeToEntityId[nameToBoneNode[boneName]];
			}

			if (!boneEntityIds.empty()) {
				CreateComponent<Render::Mdl::BoneNodeEntities>(modelEntityId, boneEntityIds);
			}


			//CREATE MODEL DATA AND MODEL DATA NODES
			{
				ASSERT_MSG(!IsComponentExist<Render::Mdl::ModelDataEntityId>(modelEntityId),
					"ModelDataEntityId cant exist in the creating model step.");

				auto modelDataIt = render__Model__Data__ModelNameToDataEntityId4->modelNameToModelDataEntityId_.find(modelFile3->fileName_);
				if (modelDataIt == render__Model__Data__ModelNameToDataEntityId4->modelNameToModelDataEntityId_.end()) {
					//Controller doesnt contain model data.

					ECS2::Entity::Id modelDataEntityId = CreateEntity<MODEL_DATA>();

					CreateComponent<Name>(modelDataEntityId, "ModelData");

					std::map<const aiNode*, ECS2::Entity::Id> dataNodeToEntityId;
					decltype(Render::Mdl::ModelNodeEntityIds::nodeEntityIds_) dataNodeEntityIds;
					dataNodeEntityIds.fill(ECS2::Entity::Id::invalid_);

					std::vector<ECS2::Entity::Id> foundDataNodesEntityIds;
					std::function<void(const aiScene*, aiNode*)> createNodeDataEntity = [&](const aiScene* scene, aiNode* node) {

						const bool isNodeBone = nameToBone.contains(node->mName.C_Str());
						const bool isAnimatedNode = nodeToNodeAnim.contains(node);

						ECS2::Entity::Id nodeEntityId = ECS2::Entity::Id::invalid_;

						nodeEntityId = CreateEntity<RENDER__MDL__NODE_DATA>();
						foundDataNodesEntityIds.push_back(nodeEntityId);
						dataNodeToEntityId[node] = nodeEntityId;

						for (Common::Index i = 0; i < node->mNumChildren; i++) {
							aiNode* childrenNode = node->mChildren[i];
							createNodeDataEntity(scene, childrenNode);
						}

						};

					createNodeDataEntity(scene, scene->mRootNode);

					ASSERT(foundDataNodesEntityIds.size() <= dataNodeEntityIds.max_size());
					std::copy(
						foundDataNodesEntityIds.begin(),
						foundDataNodesEntityIds.end(),
						dataNodeEntityIds.begin());

					CreateComponent<Render::Mdl::ModelNodeEntityIds>(modelDataEntityId, dataNodeEntityIds);

					auto createDataNodesHierarchy = [&]() {


						auto aiMatrixToGlmMatrix = [](const aiMatrix4x4& aiMatrix) {

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
							};

						auto createDataNodeComponents = [&](const aiScene* scene, const aiNode* node, ECS2::Entity::Id dataNodeEntityId) {

							aiVector3D position3D;
							aiQuaternion rotation3D;
							aiVector3D scale3D;
							aiMatrix4x4 flipYtoZ;
							node->mTransformation.Decompose(scale3D, rotation3D, position3D);

							auto createDataNodeAnimationChannel = [&dataNodeToEntityId, this](const aiNode* node, const aiScene* scene) {

								{
									decltype(Animation::Mdl::Node::Animations::animations_) animationChannels;
									Common::Size nodeAnimationsNumber = 0;
									const ECS2::Entity::Id nodeEntityId = dataNodeToEntityId[node];
									//ASSERT_MSG(scene->mNumAnimations % animationChannels.max_size() , "Unsupported number of animations per model.");
									for (Common::Index i = 0; i < std::clamp((size_t)scene->mNumAnimations, (size_t)0, animationChannels.max_size()); i++) {
										aiAnimation* animation = scene->mAnimations[i];

										for (Common::Index channelIndex = 0; channelIndex < animation->mNumChannels; channelIndex++) {
											aiNodeAnim* nodeAnim = animation->mChannels[channelIndex];
											if (nodeAnim->mNodeName == node->mName) {
												++nodeAnimationsNumber;
												ASSERT_MSG(nodeAnimationsNumber <= animationChannels.max_size(), "Unsupported number of animations per mode node.");
												Animation::Mdl::Node::Animation modelNodeAnimation;

												STATIC_ASSERT_MSG(modelNodeAnimation.position3DKeys_.keys_.max_size() > 2, "");
												STATIC_ASSERT_MSG(modelNodeAnimation.rotationKeys_.keys_.max_size() > 2, "");
												STATIC_ASSERT_MSG(modelNodeAnimation.scale3DKeys_.keys_.max_size() > 2, "");

												{	//Position keys
													//avoid not important keys.
													if (nodeAnim->mNumPositionKeys > modelNodeAnimation.position3DKeys_.keys_.max_size()) {
														auto calculateKeyImportance = [](
															aiVectorKey previous,
															aiVectorKey current,
															aiVectorKey next) {

																const glm::vec3 previousValue{ previous.mValue.x, previous.mValue.y , previous.mValue.z };
																const glm::vec3 currentValue{ current.mValue.x, current.mValue.y, current.mValue.z };
																const glm::vec3 nextValue{ previous.mValue.x, previous.mValue.y , previous.mValue.z };

																const glm::vec3 middleValue = glm::mix(previousValue, nextValue, 0.5);

																const float distance = glm::distance(middleValue, currentValue);

																const float distanceFactor = std::abs(1.0 / distance);
																const float timeFactor = 1.0f / (next.mTime - previous.mTime);

																return distanceFactor * timeFactor;

															};

														std::vector<std::pair<Common::Index, float>> keyIndexImportance;

														//Calculte importance for keys except first and last.
														for (Common::Index positionKeyIndex = 1; positionKeyIndex < nodeAnim->mNumPositionKeys - 1; positionKeyIndex++) {
															keyIndexImportance.push_back({ positionKeyIndex , calculateKeyImportance(
																nodeAnim->mPositionKeys[positionKeyIndex - 1],
																nodeAnim->mPositionKeys[positionKeyIndex],
																nodeAnim->mPositionKeys[positionKeyIndex + 1]
															) });
														}

														//Sort by importance.

														std::ranges::sort(keyIndexImportance,
															[](std::pair<Common::Index, float>& first, std::pair<Common::Index, float>& second) {
																return first.second > second.second;
															});

														keyIndexImportance.resize(modelNodeAnimation.position3DKeys_.keys_.max_size() - 2); // minus first and last

														//Sort by index
														std::ranges::sort(keyIndexImportance,
															[](std::pair<Common::Index, float>& first, std::pair<Common::Index, float>& second) {
																return first.first < second.first;
															});

														//Set first.
														aiVectorKey firstPositionKey = nodeAnim->mPositionKeys[0];
														modelNodeAnimation.position3DKeys_.keys_[0] = {
															static_cast<float>(firstPositionKey.mTime),
															firstPositionKey.mValue.x, firstPositionKey.mValue.y, firstPositionKey.mValue.z };

														for (Common::Index resultPositionKeyIndex = 1; resultPositionKeyIndex < modelNodeAnimation.position3DKeys_.keys_.max_size() - 1; resultPositionKeyIndex++) {

															Common::Index positionKeyIndex = keyIndexImportance[resultPositionKeyIndex - 1].first;

															aiVectorKey positionKey = nodeAnim->mPositionKeys[positionKeyIndex];

															modelNodeAnimation.position3DKeys_.keys_[resultPositionKeyIndex] = {
																static_cast<float>(positionKey.mTime),
																positionKey.mValue.x, positionKey.mValue.y, positionKey.mValue.z };
														}

														//Set last.
														aiVectorKey lastPositionKey = nodeAnim->mPositionKeys[nodeAnim->mNumPositionKeys - 1];
														modelNodeAnimation.position3DKeys_.keys_[modelNodeAnimation.position3DKeys_.keys_.max_size() - 1] = {
															static_cast<float>(lastPositionKey.mTime),
															lastPositionKey.mValue.x, lastPositionKey.mValue.y, lastPositionKey.mValue.z };

													}
													else {
														//Fill with last position key.	
														aiVectorKey lastPositionKey = nodeAnim->mPositionKeys[nodeAnim->mNumPositionKeys - 1];
														modelNodeAnimation.position3DKeys_.keys_.fill(
															{
																static_cast<float>(lastPositionKey.mTime),
																lastPositionKey.mValue.x,
																lastPositionKey.mValue.y,
																lastPositionKey.mValue.z
															});

														for (Common::Index positionKeyIndex = 0; positionKeyIndex < nodeAnim->mNumPositionKeys; positionKeyIndex++) {
															aiVectorKey positionKey = nodeAnim->mPositionKeys[positionKeyIndex];
															modelNodeAnimation.position3DKeys_.keys_[positionKeyIndex] = {
																static_cast<float>(positionKey.mTime),
																positionKey.mValue.x, positionKey.mValue.y, positionKey.mValue.z };
														}
													}
												}
												{//Rotation keys
													//fill empty keys with last key

													if (nodeAnim->mNumRotationKeys > modelNodeAnimation.rotationKeys_.keys_.max_size()) {

														auto calculateRotationImportanceByAxis = [](
															aiQuatKey prevKey,
															aiQuatKey currentKey,
															aiQuatKey nextKey) {

																// 1. Извлекаем оси вращения для каждого интервала
																aiQuaternion delta1 = currentKey.mValue * prevKey.mValue.Conjugate();
																aiQuaternion delta2 = nextKey.mValue * currentKey.mValue.Conjugate();

																// Нормализуем для извлечения осей
																delta1.Normalize();
																delta2.Normalize();

																// 2. Оси вращения (vector part)
																aiVector3D axis1(delta1.x, delta1.y, delta1.z);
																aiVector3D axis2(delta2.x, delta2.y, delta2.z);

																// 3. Углы вращения
																float angle1 = 2.0f * std::acos(std::abs(delta1.w));
																float angle2 = 2.0f * std::acos(std::abs(delta2.w));

																// 4. Изменение оси вращения (dot product осей)
																float axisChange = 0.0f;
																if (axis1.Length() > 0.001f && axis2.Length() > 0.001f) {
																	axis1.Normalize();
																	axis2.Normalize();
																	float dot = axis1 * axis2; // dot product
																	axisChange = 1.0f - std::abs(dot); // 0 = одинаковые оси, 1 = перпендикулярные
																}

																// 5. Изменение угловой скорости
																float time1 = currentKey.mTime - prevKey.mTime;
																float time2 = nextKey.mTime - currentKey.mTime;
																float velocity1 = (time1 > 0.0f) ? angle1 / time1 : 0.0f;
																float velocity2 = (time2 > 0.0f) ? angle2 / time2 : 0.0f;
																float velocityChange = std::abs(velocity2 - velocity1);

																return axisChange * 0.6f + velocityChange * 0.4f;
															};


														std::vector<std::pair<Common::Index, float>> keyIndexImportance;

														//Calculte importance for keys except first and last.
														for (Common::Index rotationKeyIndex = 1; rotationKeyIndex < nodeAnim->mNumRotationKeys - 1; rotationKeyIndex++) {
															keyIndexImportance.push_back({ rotationKeyIndex , calculateRotationImportanceByAxis(
																nodeAnim->mRotationKeys[rotationKeyIndex - 1],
																nodeAnim->mRotationKeys[rotationKeyIndex],
																nodeAnim->mRotationKeys[rotationKeyIndex + 1]
															) });
														}

														//Sort by importance.

														std::ranges::sort(keyIndexImportance,
															[](std::pair<Common::Index, float>& first, std::pair<Common::Index, float>& second) {
																return first.second > second.second;
															});

														keyIndexImportance.resize(modelNodeAnimation.rotationKeys_.keys_.max_size() - 2); // minus first and last

														//Sort by index
														std::ranges::sort(keyIndexImportance,
															[](std::pair<Common::Index, float>& first, std::pair<Common::Index, float>& second) {
																return first.first < second.first;
															});

														//Set first.
														aiQuatKey firstRotationKey = nodeAnim->mRotationKeys[0];
														modelNodeAnimation.rotationKeys_.keys_[0] = {
															static_cast<float>(firstRotationKey.mTime),
															firstRotationKey.mValue.w, firstRotationKey.mValue.x, firstRotationKey.mValue.y, firstRotationKey.mValue.z };

														for (Common::Index resultRotationKeyIndex = 1; resultRotationKeyIndex < modelNodeAnimation.rotationKeys_.keys_.max_size() - 1; resultRotationKeyIndex++) {

															Common::Index rotationKeyIndex = keyIndexImportance[resultRotationKeyIndex - 1].first;

															aiQuatKey rotationKey = nodeAnim->mRotationKeys[rotationKeyIndex];

															modelNodeAnimation.rotationKeys_.keys_[resultRotationKeyIndex] = {
																static_cast<float>(rotationKey.mTime),
																rotationKey.mValue.w, rotationKey.mValue.x, rotationKey.mValue.y, rotationKey.mValue.z };
														}

														//Set last.
														aiQuatKey lastRotationKey = nodeAnim->mRotationKeys[nodeAnim->mNumRotationKeys - 1];
														modelNodeAnimation.rotationKeys_.keys_[modelNodeAnimation.rotationKeys_.keys_.max_size() - 1] = {
															static_cast<float>(lastRotationKey.mTime),
															lastRotationKey.mValue.w, lastRotationKey.mValue.x, lastRotationKey.mValue.y, lastRotationKey.mValue.z };


													}
													else {
														aiQuatKey lastRotationKey = nodeAnim->mRotationKeys[nodeAnim->mNumRotationKeys - 1];
														modelNodeAnimation.rotationKeys_.keys_.fill({
																static_cast<float>(lastRotationKey.mTime),
																lastRotationKey.mValue.w, lastRotationKey.mValue.x, lastRotationKey.mValue.y, lastRotationKey.mValue.z });

														for (Common::Index rotationKeyIndex = 0; rotationKeyIndex < nodeAnim->mNumRotationKeys; rotationKeyIndex++) {
															aiQuatKey rotationKey = nodeAnim->mRotationKeys[rotationKeyIndex];
															modelNodeAnimation.rotationKeys_.keys_[rotationKeyIndex] = {
																static_cast<float>(rotationKey.mTime),
																rotationKey.mValue.w, rotationKey.mValue.x, rotationKey.mValue.y, rotationKey.mValue.z };
														}
													}
												}
												{ // Scake keys
													//fill empty keys with last key
													if (nodeAnim->mNumScalingKeys > modelNodeAnimation.scale3DKeys_.keys_.max_size()) {
														auto calculateKeyImportance = [](
															aiVectorKey previous,
															aiVectorKey current,
															aiVectorKey next) {

																const glm::vec3 previousValue{ previous.mValue.x, previous.mValue.y , previous.mValue.z };
																const glm::vec3 currentValue{ current.mValue.x, current.mValue.y, current.mValue.z };
																const glm::vec3 nextValue{ previous.mValue.x, previous.mValue.y , previous.mValue.z };

																const glm::vec3 middleValue = glm::mix(previousValue, nextValue, 0.5);

																const float distance = glm::distance(middleValue, currentValue);

																const float distanceFactor = std::abs(1.0 / distance);
																const float timeFactor = 1.0f / (next.mTime - previous.mTime);

																return distanceFactor * timeFactor;

															};

														std::vector<std::pair<Common::Index, float>> keyIndexImportance;

														//Calculte importance for keys except first and last.
														for (Common::Index scaleKeyIndex = 1; scaleKeyIndex < nodeAnim->mNumScalingKeys - 1; scaleKeyIndex++) {
															keyIndexImportance.push_back({ scaleKeyIndex , calculateKeyImportance(
																nodeAnim->mScalingKeys[scaleKeyIndex - 1],
																nodeAnim->mScalingKeys[scaleKeyIndex],
																nodeAnim->mScalingKeys[scaleKeyIndex + 1]
															) });
														}

														//Sort by importance.

														std::ranges::sort(keyIndexImportance,
															[](std::pair<Common::Index, float>& first, std::pair<Common::Index, float>& second) {
																return first.second > second.second;
															});

														keyIndexImportance.resize(modelNodeAnimation.scale3DKeys_.keys_.max_size() - 2); // minus first and last

														//Sort by index
														std::ranges::sort(keyIndexImportance,
															[](std::pair<Common::Index, float>& first, std::pair<Common::Index, float>& second) {
																return first.first < second.first;
															});

														//Set first.
														aiVectorKey firstScaleKey = nodeAnim->mScalingKeys[0];
														modelNodeAnimation.scale3DKeys_.keys_[0] = {
															static_cast<float>(firstScaleKey.mTime),
															firstScaleKey.mValue.x, firstScaleKey.mValue.y, firstScaleKey.mValue.z };

														for (Common::Index resultScaleKeyIndex = 1; resultScaleKeyIndex < modelNodeAnimation.scale3DKeys_.keys_.max_size() - 1; resultScaleKeyIndex++) {

															Common::Index scaleKeyIndex = keyIndexImportance[resultScaleKeyIndex - 1].first;

															aiVectorKey scaleKey = nodeAnim->mScalingKeys[scaleKeyIndex];

															modelNodeAnimation.scale3DKeys_.keys_[resultScaleKeyIndex] = {
																static_cast<float>(scaleKey.mTime),
																scaleKey.mValue.x, scaleKey.mValue.y, scaleKey.mValue.z };
														}

														//Set last.
														aiVectorKey lastScaleKey = nodeAnim->mScalingKeys[nodeAnim->mNumScalingKeys - 1];
														modelNodeAnimation.scale3DKeys_.keys_[modelNodeAnimation.scale3DKeys_.keys_.max_size() - 1] = {
															static_cast<float>(lastScaleKey.mTime),
															lastScaleKey.mValue.x, lastScaleKey.mValue.y, lastScaleKey.mValue.z };
													}
													else {
														aiVectorKey lastScaleKey = nodeAnim->mScalingKeys[nodeAnim->mNumScalingKeys - 1];
														for (Common::Index scaleKeyIndex = nodeAnim->mNumScalingKeys; scaleKeyIndex < modelNodeAnimation.scale3DKeys_.keys_.max_size(); scaleKeyIndex++) {

															modelNodeAnimation.scale3DKeys_.keys_[scaleKeyIndex] = {
																static_cast<float>(lastScaleKey.mTime),
																lastScaleKey.mValue.x, lastScaleKey.mValue.y, lastScaleKey.mValue.z };
														}
														for (Common::Index scaleKeyIndex = 0; scaleKeyIndex < nodeAnim->mNumScalingKeys; scaleKeyIndex++) {
															aiVectorKey scaleKey = nodeAnim->mScalingKeys[scaleKeyIndex];
															modelNodeAnimation.scale3DKeys_.keys_[scaleKeyIndex] = {
																static_cast<float>(scaleKey.mTime),
																scaleKey.mValue.x, scaleKey.mValue.y, scaleKey.mValue.z };
														}
													}
												}
												if (
													nodeAnim->mNumPositionKeys > 0 ||
													nodeAnim->mNumRotationKeys > 0 ||
													nodeAnim->mNumScalingKeys > 0) {

													animationChannels[i] = modelNodeAnimation;
												}
											}
										}
									}
									if (nodeAnimationsNumber > 0) {
										CreateComponent<Animation::Mdl::Node::Animations>(nodeEntityId, animationChannels);
									}
								}
								};



							//Parse animations.
							if (scene->HasAnimations()) {
								createDataNodeAnimationChannel(node, scene);
							}

							//Mark that this node is Bone
							if (std::ranges::find(boneNames, node->mName.C_Str()) != boneNames.end()) {

								const aiBone* bone = nameToBone[node->mName.C_Str()];
								glm::mat4 transform = aiMatrixToGlmMatrix(bone->mOffsetMatrix);

								CreateComponent<BoneInverseBindPoseMatrix>(dataNodeEntityId, transform);
								CreateComponent<Render::Mdl::BoneNode>(dataNodeEntityId);

							}

							};

						std::function<void(const aiScene*, const aiNode*)> processChildrenDataNode = [&](const aiScene* scene, const aiNode* node) {

							const ECS2::Entity::Id dataNodeEntityId = dataNodeToEntityId[node];
							const ECS2::Entity::Id nodeEntityId = nodeToEntityId[node];

							CreateComponent<Render::Mdl::ModelNode>(dataNodeEntityId);

							//Model node -> model data node.
							CreateComponent<Render::Mdl::ModelNodeDataEntityId>(nodeEntityId, dataNodeEntityId);

							auto getNodeFullName = [&]() {
								std::string nodeName;

								const aiNode* currentNode = node;
								while (currentNode != nullptr) {

									nodeName = ("/") + nodeName;
									nodeName = currentNode->mName.C_Str() + nodeName;

									currentNode = currentNode->mParent;
								}

								nodeName = modelFile3->fileName_ + "/" + scene->mName.C_Str() + "/" + nodeName;
								return nodeName;
								};

							CreateComponent<Name>(dataNodeEntityId, getNodeFullName());
							CreateComponent<Render::Mdl::ModelDataEntityId>(dataNodeEntityId, modelEntityId);

							ASSERT_FMSG(dataNodeEntityId.IsValid(), "");

							createDataNodeComponents(scene, node, dataNodeEntityId);

							std::vector<ECS2::Entity::Id> childNodeEntityIds;
							for (Common::Index i = 0; i < node->mNumChildren; i++) {
								const aiNode* childrenNode = node->mChildren[i];
								processChildrenDataNode(scene, childrenNode);
								const ECS2::Entity::Id childDataNodeEntityId = dataNodeToEntityId[childrenNode];
								ASSERT_FMSG(childDataNodeEntityId.IsValid(), "");
								childNodeEntityIds.push_back(childDataNodeEntityId);
							}

							if (!childNodeEntityIds.empty()) {
								CreateComponent<Render::Mdl::ChildModelNodeEntities>(dataNodeEntityId, childNodeEntityIds);
							}
							return dataNodeEntityId;
							};

						processChildrenDataNode(scene, scene->mRootNode);

						//Create ModelAnimations.
						{
							std::vector<ModelAnimation> modelAnimations;
							modelAnimations.reserve(scene->mNumAnimations);

							for (Common::Index i = 0; i < std::clamp((size_t)scene->mNumAnimations, (size_t)0, (size_t)Animation::Mdl::AnimationsMaxNumber); i++) {
								aiAnimation* animation = scene->mAnimations[i];
								ModelAnimation modelAnimation{
									animation->mName.C_Str(),
									animation->mDuration,
									animation->mTicksPerSecond
								};
								modelAnimations.push_back(std::move(modelAnimation));
							}
							CreateComponent<ModelAnimations>(modelDataEntityId, std::move(modelAnimations));
						}

						const ECS2::Entity::Id rootNodeEntityId = nodeToEntityId[scene->mRootNode];

						CreateComponent<Render::Mdl::ChildModelNodeEntities>(modelDataEntityId, std::vector{ rootNodeEntityId });

						};

					createDataNodesHierarchy();

					render__Model__Data__ModelNameToDataEntityId4->modelNameToModelDataEntityId_[modelFile3->fileName_] = modelDataEntityId;


					CreateComponent<Render::Mdl::ModelDataEntityId>(
						modelEntityId,
						render__Model__Data__ModelNameToDataEntityId4->modelNameToModelDataEntityId_[modelFile3->fileName_]);
				}


			}

			//CREATE MODEL NODES
			auto createNodesHierarchy = [&]() {

				auto createNodeComponents = [&](const aiScene* scene, const aiNode* node, ECS2::Entity::Id nodeEntityId) {

					aiVector3D position3D;
					aiQuaternion rotation3D;
					aiVector3D scale3D;
					aiMatrix4x4 flipYtoZ;
					node->mTransformation.Decompose(scale3D, rotation3D, position3D);

					//Mark that this node is Bone
					if (std::ranges::find(boneNames, node->mName.C_Str()) != boneNames.end()) {

						const aiBone* bone = nameToBone[node->mName.C_Str()];
						glm::mat4 transform = aiMatrixToGlmMatrix(bone->mOffsetMatrix);

						CreateComponent<Render::Mdl::BoneNode>(nodeEntityId);

					}

					CreateComponent<LocalPosition3D>(nodeEntityId, position3D.x, position3D.y, position3D.z, 0.0);
					CreateComponent<WorldPosition3D>(nodeEntityId, 0.0f, 0.0f, 0.0f, 0.0f);

					CreateComponent<LocalRotation3D>(nodeEntityId, rotation3D.w, rotation3D.x, rotation3D.y, rotation3D.z);
					CreateComponent<WorldRotation3D>(nodeEntityId, 1.0f, 0.0f, 0.0f, 0.0f);

					CreateComponent<LocalScale3D>(nodeEntityId, scale3D.x, scale3D.y, scale3D.z);
					CreateComponent<WorldScale3D>(nodeEntityId, 0.0f, 0.0f, 0.0f);

					};

				std::function<void(const aiScene*, aiNode*)> processChildrenNode = [&](const aiScene* scene, aiNode* node) {

					const ECS2::Entity::Id nodeEntityId = nodeToEntityId[node];

					CreateComponent<Render::Mdl::ModelNode>(nodeEntityId);
					CreateComponent<Name>(nodeEntityId, getNodeFullName(scene, node));
					CreateComponent<Render::Mdl::ModelEntity>(nodeEntityId, modelEntityId);

					createNodeComponents(scene, node, nodeEntityId);

					std::vector<ECS2::Entity::Id> childNodeEntityIds;
					for (Common::Index i = 0; i < node->mNumChildren; i++) {
						aiNode* childrenNode = node->mChildren[i];
						processChildrenNode(scene, childrenNode);
						const ECS2::Entity::Id childNodeEntityId = nodeToEntityId[childrenNode];

						ASSERT_FMSG(childNodeEntityId.IsValid(), "");

						childNodeEntityIds.push_back(childNodeEntityId);
					}

					if (!childNodeEntityIds.empty()) {
						CreateComponent<Render::Mdl::ChildModelNodeEntities>(nodeEntityId, childNodeEntityIds);
					}
					return nodeEntityId;
					};

				processChildrenNode(scene, scene->mRootNode);

				const ECS2::Entity::Id rootNodeEntityId = nodeToEntityId[scene->mRootNode];

				CreateComponent<Render::Mdl::ChildModelNodeEntities>(modelEntityId, std::vector{ rootNodeEntityId });

				};

			createNodesHierarchy();

			//CREATE MESHES
			auto createMeshEntities = [&]() {

				auto createMeshComponents = [&](
					const aiScene* scene,
					const aiMesh* mesh,
					ECS2::Entity::Id meshEntityId) {

						//Create texture.
						auto createDiffuseTexture = [&](ECS2::Entity::Id meshEntityId) {

							//TODO: remove copy-paste
							aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
							{
								aiString diffuseTexturePath;
								material->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &diffuseTexturePath);

								const aiTexture* texture = scene->GetEmbeddedTexture(diffuseTexturePath.C_Str());

								Common::UInt32 textureWidth = texture->mWidth;
								Common::UInt32 textureHeight = texture->mHeight;
								if (textureHeight > 0) {
									CreateComponent<Render::DiffuseMap::TextureInfo>(meshEntityId, diffuseTexturePath.C_Str());
									CreateComponent<Render::DiffuseMap::TextureData>(
										meshEntityId,
										textureWidth, textureHeight,
										std::vector<Geom::Color4b>{
										(Geom::Color4b*)texture->pcData,
											(Geom::Color4b*)texture->pcData + textureWidth * textureHeight});
								}
								else {
									//Texture is compressed
									const unsigned char* compressed_data = reinterpret_cast<const unsigned char*>(texture->pcData);

									int width, height, channels;

									unsigned char* pixels = stbi_load_from_memory(
										compressed_data,
										texture->mWidth,
										&width, &height, &channels,
										STBI_rgb_alpha
									);
									CreateComponent<Render::DiffuseMap::TextureInfo>(meshEntityId, diffuseTexturePath.C_Str());
									CreateComponent<Render::DiffuseMap::TextureData>(
										meshEntityId,
										width, height,
										std::vector<Geom::Color4b>{
										(Geom::Color4b*)pixels,
											(Geom::Color4b*)pixels + width * height});
								}
							}
							{
								aiString normalTexturePath;
								material->GetTexture(aiTextureType::aiTextureType_NORMALS, 0, &normalTexturePath);
								const aiTexture* normalTexture = scene->GetEmbeddedTexture(normalTexturePath.C_Str());
								Common::UInt32 textureWidth = normalTexture->mWidth;
								Common::UInt32 textureHeight = normalTexture->mHeight;
								if (textureHeight > 0) {
									CreateComponent<Render::NormalMap::TextureInfo>(meshEntityId, normalTexturePath.C_Str());
									CreateComponent<Render::NormalMap::TextureData>(
										meshEntityId,
										textureWidth, textureHeight,
										std::vector<Geom::Color4b>{
										(Geom::Color4b*)normalTexture->pcData,
											(Geom::Color4b*)normalTexture->pcData + textureWidth * textureHeight});
								}
								else {
									//Texture is compressed
									const unsigned char* compressed_data = reinterpret_cast<const unsigned char*>(normalTexture->pcData);

									int width, height, channels;

									unsigned char* pixels = stbi_load_from_memory(
										compressed_data,
										normalTexture->mWidth,
										&width, &height, &channels,
										STBI_rgb_alpha
									);
									CreateComponent<Render::NormalMap::TextureInfo>(meshEntityId, normalTexturePath.C_Str());
									CreateComponent<Render::NormalMap::TextureData>(
										meshEntityId,
										width, height,
										std::vector<Geom::Color4b>{
										(Geom::Color4b*)pixels,
											(Geom::Color4b*)pixels + width * height});
								}
							}
							{
								aiString ambientTexturePath;
								//material->
								aiReturn isAmbientExist = material->GetTexture(aiTextureType::aiTextureType_AMBIENT, 0, &ambientTexturePath);
								if (isAmbientExist == aiReturn_SUCCESS) {
									const aiTexture* ambientTexture = scene->GetEmbeddedTexture(ambientTexturePath.C_Str());
									Common::UInt32 textureWidth = ambientTexture->mWidth;
									Common::UInt32 textureHeight = ambientTexture->mHeight;
									if (textureHeight > 0) {
										CreateComponent<Render::NormalMap::TextureInfo>(meshEntityId, ambientTexturePath.C_Str());
										CreateComponent<Render::NormalMap::TextureData>(
											meshEntityId,
											textureWidth, textureHeight,
											std::vector<Geom::Color4b>{
											(Geom::Color4b*)ambientTexture->pcData,
												(Geom::Color4b*)ambientTexture->pcData + textureWidth * textureHeight});
									}
									else {
										//Texture is compressed
										const unsigned char* compressed_data = reinterpret_cast<const unsigned char*>(ambientTexture->pcData);

										int width, height, channels;

										unsigned char* pixels = stbi_load_from_memory(
											compressed_data,
											ambientTexture->mWidth,
											&width, &height, &channels,
											STBI_rgb_alpha
										);
										CreateComponent<Render::AmbientMap::TextureInfo>(meshEntityId, ambientTexturePath.C_Str());
										CreateComponent<Render::AmbientMap::TextureData>(
											meshEntityId,
											width, height,
											std::vector<Geom::Color4b>{
											(Geom::Color4b*)pixels,
												(Geom::Color4b*)pixels + width * height});
									}
								}
							}

							};
						createDiffuseTexture(meshEntityId);

						//Create vertices, normals, uvs, indices, vertex bones
						auto createMeshData = [&](ECS2::Entity::Id meshEntityId) {

							static Geom::VertexCloud<Geom::Vertex3f>	vertices;
							static DS::Vector<Geom::Normal3f>			normals;
							static DS::Vector<Geom::UV2f>		        uvs;
							static Geom::IndexBuffer<Geom::Index32>		indices;
							static std::vector<VertexBonesInfo>			vertexBonesInfos;


							vertexBonesInfos.resize(mesh->mNumVertices,
								VertexBonesInfo{
									//If vertex has bones, will be at least one bone and we will be use this for empty cells of array: we will be use first bone with 0.0 weight.
									{
										Render::Mdl::ModelNodesMaxNumber,
										Render::Mdl::ModelNodesMaxNumber,
										Render::Mdl::ModelNodesMaxNumber,
										Render::Mdl::ModelNodesMaxNumber
									},
									{ 0.0f, 0.0f, 0.0f, 0.0f }	// no influence on vertex by default  
								});

							{
								for (Common::Index i = 0; i < mesh->mNumBones; i++) {
									//Save vertex bone info
									aiBone* bone = mesh->mBones[i];
									for (Common::Index j = 0; j < bone->mNumWeights; j++) {
										const aiVertexWeight* weight = bone->mWeights + j;
										if (Math::IsEqual(weight->mWeight, 0.0f)) {
											//Skip bones with zero weight. We will use zero weight to mark empty cells.
											continue;
										}
										//Find free cell and set value
										[[maybe_unused]]
										bool isFind = false;
										for (Common::Index k = 0; k < VertexMaxBonesNumber; k++) {
											if (vertexBonesInfos[weight->mVertexId].boneWeights_[k] == 0) {
												//Free cell found.


												const ECS2::Entity::Id boneEntityId = nodeToEntityId[nameToBoneNode[bone->mName.C_Str()]];
												Common::Index boneEntityIdIndex = Common::Limits<Common::Index>::Max();
												for (Common::Index l = 0; l < boneEntityIds.size(); l++) {
													if (boneEntityId == boneEntityIds[l]) {
														boneEntityIdIndex = l;
													}
												}
												ASSERT(boneEntityIdIndex != Common::Limits<Common::Index>::Max());
												ASSERT(boneEntityIdIndex < Render::Mdl::ModelNodesMaxNumber);

												vertexBonesInfos[weight->mVertexId].boneEntityIndices_[k] = boneEntityIdIndex;
#pragma region Assert
												ASSERT_FMSG(Math::IsLess(weight->mWeight, 1.0), "");
#pragma endregion
												vertexBonesInfos[weight->mVertexId].boneWeights_[k] = weight->mWeight;
												isFind = true;
												break;
											}
										}
#pragma region Assert
										ASSERT_FMSG(isFind, "");
#pragma endregion
									}

									//Save ids of bones entities for the mesh entity.
									const aiNode* boneNode = nameToBoneNode[bone->mName.C_Str()];
									const ECS2::Entity::Id boneEntityId = nodeToEntityId[boneNode];
									ASSERT(boneEntityId.IsValid());
									//boneEntityIds.push_back(boneEntityId);
								}
								if (mesh->mNumBones > 0) {
									//	CreateComponent<BoneNodeEntities>(meshEntityId, boneEntityIds);
									CreateComponent<VertexBones>(meshEntityId, std::move(vertexBonesInfos));
								}
							}
							{
								//Иерархическая анимация
								if (mesh->mNumBones == 0) {
									std::vector<ECS2::Entity::Id> meshNodeEntityIds;
									for (auto [node, entityId] : nodeToEntityId) {
										for (Common::Index k = 0; k < node->mNumMeshes; k++) {
											aiMesh* maybeThisMesh = scene->mMeshes[node->mMeshes[k]];
											if (maybeThisMesh == mesh) {
												meshNodeEntityIds.push_back(entityId);
											}
										}
									}
									std::vector<Common::Index> nodeEntityIndices;
									for (ECS2::Entity::Id nodeEntityId : meshNodeEntityIds) {
										auto nodeEntityIdIt = std::find(modelNodeEntityIds.begin(), modelNodeEntityIds.end(), nodeEntityId);
										nodeEntityIndices.push_back(std::distance(modelNodeEntityIds.begin(), nodeEntityIdIt));
									}

									CreateComponent<Render::Mdl::ModelNodeEntityIndices>(meshEntityId, nodeEntityIndices);

								}
							}


							//std::vector<Draw> drawInfos;
							//{
							//	std::vector<glm::mat4> palette;
							//	palette.resize(128, glm::mat4{ 1 });
							//	Draw draw{
							//		boneEntityIds,
							//		palette
							//	};
							//	drawInfos.push_back(draw);
							//	vertexBonesInfos.clear();

							//	CreateComponent<DrawInfos>(meshEntityId, drawInfos);
							//}

							vertices.Reserve(mesh->mNumVertices);
							normals.Reserve(mesh->mNumVertices);
							uvs.Reserve(mesh->mNumVertices);
							for (unsigned j = 0; j < mesh->mNumVertices; j++) {
								vertices.Add(Geometry::Vertex3f{ mesh->mVertices[j].x,
																	mesh->mVertices[j].y,
																	mesh->mVertices[j].z });
								normals.PushBack(Geom::Normal3f{ mesh->mVertices[j].x,
																		mesh->mVertices[j].y,
																		mesh->mVertices[j].z });
								uvs.PushBack(Geom::UV2f{ mesh->mTextureCoords[0][j].x,
																mesh->mTextureCoords[0][j].y });
							}

							indices.Reserve(mesh->mNumFaces * 3);
							for (unsigned j = 0; j < mesh->mNumFaces; j++) {
								indices.Add(mesh->mFaces[j].mIndices[0]);
								indices.Add(mesh->mFaces[j].mIndices[1]);
								indices.Add(mesh->mFaces[j].mIndices[2]);
							}
							CreateComponent<Vertices3D>(meshEntityId, vertices);
							CreateComponent<Normals>(meshEntityId, normals);
							CreateComponent<UVs>(meshEntityId, uvs);
							CreateComponent<Indices>(meshEntityId, indices);

							vertices.Clear();
							normals.Clear();
							uvs.Clear();
							indices.Clear();

							};
						createMeshData(meshEntityId);
					};


				std::vector<std::string> meshNames;
				meshNames.reserve(scene->mNumMeshes);

				std::vector<ECS2::Entity::Id> meshEntityIds;

				meshEntityIds.resize(scene->mNumMeshes, ECS2::Entity::Id::invalid_);

				for (Common::Index i = 0; i < scene->mNumMeshes; i++) {
					aiMesh* mesh = scene->mMeshes[i];

					std::string meshName = modelFile3->fileName_ + scene->mName.C_Str() + std::string{ mesh->mName.C_Str() };

					meshNames.push_back(meshName);

					auto it = meshNameToEntity1->meshNameToEntityId_.find(meshName);

					if (it != meshNameToEntity1->meshNameToEntityId_.end()) {

						continue;
					}

					if (!meshNameToEntity1->meshNameToEntityId_.contains(meshName)) {
						//Create mesh info.
						const ECS2::Entity::Id meshEntity = CreateEntity<MESH>();

						CreateComponent<Render::Mdl::Msh::Mesh>(meshEntity);
						CreateComponent<Name>(meshEntity, meshName);
						CreateComponent<Render::Mdl::ModelName>(meshEntity, modelFile3->fileName_ + scene->mName.C_Str());
						CreateComponent<Render::Mdl::ModelEntityIds>(meshEntity, std::array<ECS2::Entity::Id, Render::Mdl::MeshMaxModelsNumber>{ modelEntityId });

						createMeshComponents(scene, mesh, meshEntity);

						meshNameToEntity1->meshNameToEntityId_[meshName] = meshEntity;
						meshEntityIds[i] = (meshEntity);

					}
					else {

					}
				}

				CreateComponent<Render::Mdl::Msh::MeshNames>(modelEntityId, meshNames);
				CreateComponent<Render::Mdl::Msh::MeshEntities>(modelEntityId, meshEntityIds);

				Common::Size foundMeshsCount = 0;
				for (Common::Index i = 0; i < meshEntityIds.size(); i++) {
					if (meshEntityIds[i].IsValid()) {
						foundMeshsCount++;
					}

				}
				if (foundMeshsCount == scene->mNumMeshes) {
					CreateComponent<Render::Mdl::MeshsFound>(modelEntityId);
				}

				};

			createMeshEntities();

		}
	}

	namespace Render::Mdl {
		void CreateDataController::Update() {
			const ECS2::Entity::Id modelDataControllerEntity = CreateEntity();
			CreateComponent<DataController>(modelDataControllerEntity);
			CreateComponent<ModelNameToModelDataEntityId>(modelDataControllerEntity, std::map<std::string, ECS2::Entity::Id>{});
		}

	}

	namespace Render::Mdl {
		void FindModelData::Update(
			ECS2::Entity::Id entity0id,
			const Model* model0,
			const ModelFile* modelFile0,
			const ModelNodeEntityIds* modelNodeEntityIds0,

			ECS2::Entity::Id entity1id,
			const Render::Mdl::DataController* render__Model__DataController1,
			const Render::Mdl::ModelNameToModelDataEntityId* render__Model__ModelNameToModelDataEntityId1) {

			auto modelDataIt = render__Model__ModelNameToModelDataEntityId1->modelNameToModelDataEntityId_.find(modelFile0->fileName_);
			if (modelDataIt != render__Model__ModelNameToModelDataEntityId1->modelNameToModelDataEntityId_.end()) {
				const ECS2::Entity::Id modelDataEntityId = modelDataIt->second;
				if (IsEntityExist(modelDataEntityId)) {

					auto* modelDataNodeEntityIds = GetComponent<ModelNodeEntityIds>(modelDataEntityId);

					for (Common::Index i = 0; i < modelNodeEntityIds0->nodeEntityIds_.size(); i++) {
						const ECS2::Entity::Id nodeEntityId = modelNodeEntityIds0->nodeEntityIds_[i];
						if (nodeEntityId.IsValid()) {
							ASSERT(!IsComponentExist<Render::Mdl::ModelNodeDataEntityId>(nodeEntityId));
							CreateComponent<Render::Mdl::ModelNodeDataEntityId>(nodeEntityId, modelDataNodeEntityIds->nodeEntityIds_[i]);

						}
					}
					CreateComponent<Render::Mdl::ModelDataEntityId>(entity0id, modelDataEntityId);
				}
			}

		}


		void FindModelMeshs::Update(
			ECS2::Entity::Id entity0id,
			const Model* model0,
			const Render::Mdl::Msh::MeshNames* meshNames0,
			Render::Mdl::Msh::MeshEntities* meshEntities0,

			ECS2::Entity::Id entity1id,
			const Render::Mdl::Msh::Mesh* mesh1,
			const Name* name1,
			ModelEntityIds* modelEntityIds1) {

			const ECS2::Entity::Id& modelEntityId = entity0id;
			const ECS2::Entity::Id& meshEntityId = entity1id;

			ASSERT(meshNames0->meshNames_.size() == meshEntities0->meshEntityIds_.size());


			if (
				std::find(
					meshEntities0->meshEntityIds_.begin(),
					meshEntities0->meshEntityIds_.end(),
					ECS2::Entity::Id::invalid_) == meshEntities0->meshEntityIds_.end()) {

				//Skip models that already have all required meshs
				return;
			}

			for (Common::Index i = 0; i < meshNames0->meshNames_.size(); i++) {
				if (
					meshEntities0->meshEntityIds_[i].IsInvalid() &&
					meshNames0->meshNames_[i] == name1->value_) {
					meshEntities0->meshEntityIds_[i] = meshEntityId;

					bool isFoundFreeCell = false;
					for (Common::Index j = 0; j < modelEntityIds1->modelEntityIds_.max_size(); j++) {
						if (modelEntityIds1->modelEntityIds_[j].IsInvalid()) {
							modelEntityIds1->modelEntityIds_[j] = modelEntityId;
							isFoundFreeCell = true;
							break;
						}
					}
					ASSERT(isFoundFreeCell);
					//modelEntityIds1->modelEntityIds_.push_back(entity0id);
				}
			}
			if (
				std::find(
					meshEntities0->meshEntityIds_.begin(),
					meshEntities0->meshEntityIds_.end(),
					ECS2::Entity::Id::invalid_) == meshEntities0->meshEntityIds_.end()) {

				ASSERT(!IsComponentExist<MeshsFound>(modelEntityId));

				CreateComponent<MeshsFound>(modelEntityId);
			}
		}
	}

	namespace Render::Mdl::Msh {
		void CreateMeshsController::Update() {

			const ECS2::Entity::Id meshsControllerEntity = CreateEntity();
			CreateComponent<MeshsController>(meshsControllerEntity);
			CreateComponent<MeshNameToEntity>(meshsControllerEntity, std::map<std::string, ECS2::Entity::Id>{});

		}
	}



	//	void CreateDriverBonesPallet::Update(
	//		ECS2::Entity::Id entity0id,
	//		RenderDriver* renderDriver0,
	//
	//		ECS2::Entity::Id entity1id,
	//		const BonesPallet* bonesPallet1) {
	//
	//#pragma region Assert
	//		ASSERT_FMSG(bonesPallet1->bonesPallets_.size() <= 128, "");
	//#pragma endregion
	//
	//		RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
	//			.size_ = bonesPallet1->bonesPallets_.size() * sizeof(glm::mat4),
	//			.type_ = RAL::Driver::UniformBuffer::Type::Mutable
	//		};
	//		RAL::Driver::UniformBuffer::Id ubId = renderDriver0->driver_->CreateUniformBuffer(UBCreateInfo);
	//		renderDriver0->driver_->FillUniformBuffer(ubId, (void*)bonesPallet1->bonesPallets_.data());
	//
	//		CreateComponent<DriverBonesPallet>(entity1id, ubId);
	//
	//	}

		//void CreateBonesPalletResource::Update(
		//	ECS2::Entity::Id entity0id,
		//	RenderDriver* renderDriver0,

		//	ECS2::Entity::Id entity1id,
		//	const BonesPallet* bonesPallet1,
		//	const DriverBonesPallet* driverBonesPallet1) {

		//	RAL::Driver::ResourceSet::Binding transformUBBinding
		//	{
		//		.stage_ = RAL::Driver::Shader::Stage::VertexShader,
		//		.binding_ = 0,
		//		.ubid_ = driverBonesPallet1->id_,
		//		.offset_ = 0,
		//		.size_ = 128 * sizeof(glm::mat4)
		//	};

		//	RAL::Driver::Resource::Id resourceId = renderDriver0->driver_->CreateResource(transformUBBinding);

		//	CreateComponent<BonesPalletResource>(entity1id, resourceId);

		//}

		//glm::mat4 RTS_to_mat4_optimized(glm::vec3 translation, glm::quat rotation, glm::vec3 scale) {
		//	float w = rotation.w;
		//	float x = rotation.x;
		//	float y = rotation.y;
		//	float z = rotation.z;

		//	// Предварительные вычисления
		//	float x2 = x * x;
		//	float y2 = y * y;
		//	float z2 = z * z;
		//	float xy = x * y;
		//	float xz = x * z;
		//	float yz = y * z;
		//	float wx = w * x;
		//	float wy = w * y;
		//	float wz = w * z;

		//	// Создаём матрицу напрямую
		//	return glm::mat4(
		//		// Первый столбец
		//		scale.x * (1.0 - 2.0 * (y2 + z2)),
		//		scale.x * (2.0 * (xy + wz)),
		//		scale.x * (2.0 * (xz - wy)),
		//		0.0,

		//		// Второй столбец
		//		scale.y * (2.0 * (xy - wz)),
		//		scale.y * (1.0 - 2.0 * (x2 + z2)),
		//		scale.y * (2.0 * (yz + wx)),
		//		0.0,

		//		// Третий столбец
		//		scale.z * (2.0 * (xz + wy)),
		//		scale.z * (2.0 * (yz - wx)),
		//		scale.z * (1.0 - 2.0 * (x2 + y2)),
		//		0.0,

		//		// Четвертый столбец
		//		translation.x,
		//		translation.y,
		//		translation.z,
		//		1.0
		//	);
		//}

		//void UpdateBonePallet::Update(
		//	ECS2::Entity::Id entity0id,
		//	const Model* model0,
		//	const BoneNodeEntities* boneNodeEntities0,
		//	BonesPallet* bonesPallet0) {

		//	//return;
		//	//const std::vector<ECS2::Entity::Id>& boneEntityIds = boneNodeEntities0->boneEntityIds_;

		//	//const Common::Size bineEntityIdsNumber = boneEntityIds.size();

		//	//ASSERT(bineEntityIdsNumber <= 128);

		//	//for (Common::Index i = 0; i < bineEntityIdsNumber; i++) {

		//	//	const ECS2::Entity::Id boneEntityId = boneEntityIds[i];

		//	//	auto components = world_->GetComponents<
		//	//		WorldPosition3D,
		//	//		WorldRotation3D,
		//	//		WorldScale3D,
		//	//		BoneInverseBindPoseMatrix>(boneEntityId);

		//	//	auto* worldPosition3D = std::get<WorldPosition3D*>(components);//GetComponent<WorldPosition3D>(boneEntityId);
		//	//	auto* worldRotation3D = std::get<WorldRotation3D*>(components);//GetComponent<WorldRotation3D>(boneEntityId);
		//	//	auto* worldScale3D = std::get<WorldScale3D*>(components);//GetComponent<WorldScale3D>(boneEntityId);


		//	//	const glm::mat4 boneTransformMatrix = RTS_to_mat4_optimized(
		//	//		glm::vec3(
		//	//			worldPosition3D->x_,
		//	//			worldPosition3D->y_,
		//	//			worldPosition3D->z_),
		//	//		glm::quat{
		//	//			worldRotation3D->w_,
		//	//			worldRotation3D->x_,
		//	//			worldRotation3D->y_,
		//	//			worldRotation3D->z_ },
		//	//			glm::vec3(
		//	//				worldScale3D->x_,
		//	//				worldScale3D->y_,
		//	//				worldScale3D->z_)

		//	//	);
		//	//	//const glm::mat4 nodeTranslateMatrix
		//	//	//	= glm::mat4{ glm::translate(
		//	//	//		glm::vec3(
		//	//	//			worldPosition3D->x_,
		//	//	//			worldPosition3D->y_,
		//	//	//			worldPosition3D->z_)
		//	//	//	) };

		//	//	//const glm::mat4 nodeRotationMatrix
		//	//	//	= glm::toMat4(
		//	//	//		glm::quat{
		//	//	//			worldRotation3D->w_,
		//	//	//			worldRotation3D->x_,
		//	//	//			worldRotation3D->y_,
		//	//	//			worldRotation3D->z_ }
		//	//	//			);

		//	//	//const glm::mat4 nodeScaleMatrix
		//	//	//	= glm::scale(
		//	//	//		glm::vec3(
		//	//	//			worldScale3D->x_,
		//	//	//			worldScale3D->y_,
		//	//	//			worldScale3D->z_));

		//	//	//const glm::mat4 boneTransformMatrix
		//	//	//	= nodeTranslateMatrix * nodeRotationMatrix * nodeScaleMatrix;

		//	//	const auto* boneInverseBindPoseMatrix = std::get<BoneInverseBindPoseMatrix*>(components);// GetComponent<BoneInverseBindPoseMatrix>(boneEntityId);

		//	//	bonesPallet0->bonesPallets_[i] = boneTransformMatrix * boneInverseBindPoseMatrix->matrix_;

		//	//}

		//}

		//void UpdateDriverBonesPallet::Update(

		//	ECS2::Entity::Id entity0id,
		//	RenderDriver* renderDriver0,

		//	ECS2::Entity::Id entity1id,
		//	const BonesPallet* bonesPallet1,
		//	DriverBonesPallet* driverBonesPallet1) {

		//	return;
		//	renderDriver0->driver_->FillUniformBuffer(
		//		driverBonesPallet1->id_,
		//		(void*)bonesPallet1->bonesPallets_.data());

		//}

	namespace Render {
		void CreateRenderPass::Update(ECS2::Entity::Id entity0id, const RenderDriver* renderDriver0) {


			auto driver = renderDriver0->driver_;

			std::vector<RAL::Driver::RP::AttachmentUsage> attachmentsUsage;
			{
				RAL::Driver::RP::AttachmentUsage depthTestAttachment{
					.format_ = RAL::Driver::Texture::Format::D_32_SFLOAT,
					.initialState_ = RAL::Driver::Texture::State::DataForDepthStencilWrite,
					.loadOperation_ = RAL::Driver::RP::AttachmentUsage::LoadOperation::Clear,
					.storeOperation_ = RAL::Driver::RP::AttachmentUsage::StoreOperation::Store,
					.finalState_ = RAL::Driver::Texture::State::DataForDepthStencilWrite,
					.samplesCount_ = RAL::Driver::SamplesCount::SamplesCount_8
				};
				attachmentsUsage.push_back(depthTestAttachment);

				RAL::Driver::RP::AttachmentUsage GBufferAttachment{
					.format_ = RAL::Driver::Texture::Format::RGBA_32_UNORM,
					.initialState_ = RAL::Driver::Texture::State::DataIsUndefined,
					.loadOperation_ = RAL::Driver::RP::AttachmentUsage::LoadOperation::Clear,
					.storeOperation_ = RAL::Driver::RP::AttachmentUsage::StoreOperation::Store,
					.finalState_ = RAL::Driver::Texture::State::DataForColorWrite,
					.samplesCount_ = RAL::Driver::SamplesCount::SamplesCount_8
				};
				attachmentsUsage.push_back(GBufferAttachment);

				RAL::Driver::RP::AttachmentUsage multisamplingAttachment{
					.format_ = RAL::Driver::Texture::Format::RGBA_32_UNORM,
					.initialState_ = RAL::Driver::Texture::State::DataForColorWrite,
					.loadOperation_ = RAL::Driver::RP::AttachmentUsage::LoadOperation::Clear,
					.storeOperation_ = RAL::Driver::RP::AttachmentUsage::StoreOperation::Store,
					.finalState_ = RAL::Driver::Texture::State::DataForColorWrite,
					.samplesCount_ = RAL::Driver::SamplesCount::SamplesCount_1
				};
				attachmentsUsage.push_back(multisamplingAttachment);
			}

			std::vector<RAL::Driver::RP::Subpass> subpasses;
			{
				RAL::Driver::RP::Subpass subpass{
					.colorAttachments_ = { 1 }, // Index of attachment in attachment set.
					.resolveAttachment_ = 2,
					.depthStencilAttachment_ = 0
				};
				subpasses.push_back(subpass);
			}

			RAL::Driver::RP::CI rpCI{
				.name_ = "RenderPass",
				.attachments_ = attachmentsUsage,
				.subpasses_ = subpasses
			};

			const RAL::Driver::RP::Id renderPassId = driver->CreateRenderPass(rpCI);


			CreateComponent<MainRenderPass>(entity0id, renderPassId);
		}


		void CreateRenderAttachment::Update(ECS2::Entity::Id entity0id, const RenderDriver* renderDriver0) {


			//RENDER BUFFER
			RAL::Driver::Texture::CreateInfo1 renderBufferCreateInfo{
				.name_ = "Render buffer",
				.format_ = RAL::Driver::Texture::Format::RGBA_32_UNORM,
				.size_ = { 2560, 1440 },
				.targetState_ = RAL::Driver::Texture::State::DataForColorWrite,
				.targetAccess_ = RAL::Driver::Texture::Access::ColorWrite,
				.targetPipelineStages_ = { RAL::Driver::Pipeline::Stage::ColorAttachmentOutput },
				.usages_ = { RAL::Driver::Texture::Usage::ColorAttachment },
				.mipLevels_ = 1,
				.samplesCount_ = RAL::Driver::SamplesCount::SamplesCount_8
			};
			const RAL::Driver::Texture::Id renderBufferId = renderDriver0->driver_->CreateTexture(renderBufferCreateInfo);

			CreateComponent<RenderAttachment>(entity0id, renderBufferId);

		}

		void CreateDepthAttachment::Update(ECS2::Entity::Id entity0id, const RenderDriver* renderDriver0) {


			//DEPTH BUFFER
			RAL::Driver::Texture::CreateInfo1 depthBufferCreateInfo{
				.name_ = "Depth buffer",
				.format_ = RAL::Driver::Texture::Format::D_32_SFLOAT,
				.size_ = { 2560, 1440 },
				.targetState_ = RAL::Driver::Texture::State::DataForDepthStencilWrite,
				.targetAccess_ = RAL::Driver::Texture::Access::DepthStencilWrite,
				.targetPipelineStages_ = { RAL::Driver::Pipeline::Stage::EarlyFragmentTests, RAL::Driver::Pipeline::Stage::LateFragmentTests },
				.usages_ = { RAL::Driver::Texture::Usage::DepthStencilAttachment },
				.mipLevels_ = 1,
				.samplesCount_ = RAL::Driver::SamplesCount::SamplesCount_8
			};

			const RAL::Driver::Texture::Id depthBufferId = renderDriver0->driver_->CreateTexture(depthBufferCreateInfo);

			CreateComponent<DepthAttachment>(entity0id, depthBufferId);

		}

		void CreateMultisamplingAttachment::Update(ECS2::Entity::Id entity0id, const RenderDriver* renderDriver0) {

			////Multisampling
			//{
			//	auto multisamplingData = std::make_shared<MultisamplingData>();
			//	{
			//		Image::CreateInfo multisamplingCreateInfo;
			//		{
			//			multisamplingCreateInfo.physicalDevice_ = objects_.physicalDevice_;
			//			multisamplingCreateInfo.LD_ = objects_.LD_;
			//			multisamplingCreateInfo.format_ = objects_.swapChain_->GetFormat().format;
			//			multisamplingCreateInfo.size_ = objects_.swapChain_->GetSize();
			//			multisamplingCreateInfo.tiling_ = VK_IMAGE_TILING_OPTIMAL;
			//			multisamplingCreateInfo.usage_ = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			//			multisamplingCreateInfo.samplesCount_ = objects_.physicalDevice_->GetMaxUsableSampleCount();
			//		}
			//		multisamplingData->image_ = std::make_shared<AllocatedImage>(multisamplingCreateInfo);
			//		multisamplingData->imageView_ = CreateImageViewByImage(objects_.LD_, multisamplingData->image_, VK_IMAGE_ASPECT_COLOR_BIT, 1);
			//	}
			//	objects_.multisamplingData_ = multisamplingData;
			//}
			//RENDER BUFFER
			RAL::Driver::Texture::CreateInfo1 renderBufferCreateInfo{
				.name_ = "Multisampling buffer",
				.format_ = RAL::Driver::Texture::Format::RGBA_32_UNORM,
				.size_ = { 2560, 1440 },
				.targetState_ = RAL::Driver::Texture::State::DataForColorWrite,
				.targetAccess_ = RAL::Driver::Texture::Access::ColorWrite,
				.targetPipelineStages_ = { RAL::Driver::Pipeline::Stage::ColorAttachmentOutput },
				.usages_ = { RAL::Driver::Texture::Usage::ColorAttachment, RAL::Driver::Texture::Usage::TransferSource },
				.mipLevels_ = 1,
				.samplesCount_ = RAL::Driver::SamplesCount::SamplesCount_1
			};
			const RAL::Driver::Texture::Id multisamplingBufferId = renderDriver0->driver_->CreateTexture(renderBufferCreateInfo);

			CreateComponent<MultisamplingAttachment>(entity0id, multisamplingBufferId);

		}

		void CreateAttachmentSet::Update(
			ECS2::Entity::Id entity0id,
			const RenderDriver* renderDriver0,
			const MainRenderPass* renderPass0,
			const RenderAttachment* renderAttachment0,
			const MultisamplingAttachment* multisamplingAttachment0,
			const DepthAttachment* depthAttachment0) {

			RAL::Driver::RP::AttachmentSet::CI attachmentSetCI{
				.rpId_ = renderPass0->rpId_,
				.textures_ = { depthAttachment0->textureId_, renderAttachment0->textureId_, multisamplingAttachment0->textureId_ },
				.size_ = glm::u32vec2{ 2560, 1440 }
			};

			RAL::Driver::RP::AttachmentSet::Id rpAttachmentsSetId = renderDriver0->driver_->CreateAttachmentSet(attachmentSetCI);

			CreateComponent<AttachmentSet>(entity0id, rpAttachmentsSetId);

		}

		void CreatePipeline::Update(
			ECS2::Entity::Id entity0id,
			const RenderDriver* renderDriver0,
			const MainRenderPass* renderPass0,

			ECS2::Entity::Id entity1id,
			const ResourceSystem* resourceSystem1) {

			Resources::ResourceData vertexTextureShaderResource = resourceSystem1->system_->GetResourceSynch(Subsystem::Type::Engine, "Root/textured.vert");
			Resources::ResourceData fragmentTextureShaderResource = resourceSystem1->system_->GetResourceSynch(Subsystem::Type::Engine, "Root/textured.frag");

			std::string vertexShaderCode{ vertexTextureShaderResource.GetData<Common::Byte>(), vertexTextureShaderResource.GetSize() };
			std::string fragmentShaderCode{ fragmentTextureShaderResource.GetData<Common::Byte>(), fragmentTextureShaderResource.GetSize() };


			RAL::Driver::Shader::CreateInfo vertexShaderCreateInfo{
				.name_ = "TexturedVertexShader",
				.type_ = RAL::Driver::Shader::Type::Vertex,
				.code_ = vertexShaderCode
			};
			auto vertexShader = renderDriver0->driver_->CreateShader(vertexShaderCreateInfo);

			RAL::Driver::Shader::CreateInfo fragmentShaderCreateInfo{
				.name_ = "TexturedFragmentShader",
				.type_ = RAL::Driver::Shader::Type::Fragment,
				.code_ = fragmentShaderCode
			};
			auto fragmentShader = renderDriver0->driver_->CreateShader(fragmentShaderCreateInfo);

			std::vector<RAL::Driver::Shader::Binding::Layout> shaderBindings;

			RAL::Driver::Shader::Binding::Layout cameraBinding{
				.binding_ = 0,
				.type_ = RAL::Driver::Shader::Binding::Type::Uniform,
				.stage_ = RAL::Driver::Shader::Stage::VertexShader
			};


			RAL::Driver::Shader::Binding::Layout samplerBinding{
				.binding_ = 0,
				.type_ = RAL::Driver::Shader::Binding::Type::Sampler,
				.stage_ = RAL::Driver::Shader::Stage::FragmentShader
			};


			RAL::Driver::Shader::Binding::Layout transformBinding{
				.binding_ = 0,
				.type_ = RAL::Driver::Shader::Binding::Type::Uniform,
				.stage_ = RAL::Driver::Shader::Stage::VertexShader
			};

			shaderBindings.push_back(cameraBinding);		//set 0
			shaderBindings.push_back(samplerBinding);		//set 1
			shaderBindings.push_back(transformBinding);		//set 2


			auto multisamplingInfo = std::make_shared<RAL::Driver::Pipeline::MultisamplingInfo>();
			{
				multisamplingInfo->samplesCount_ = RAL::Driver::SamplesCount::SamplesCount_8;
			}


			RAL::Driver::Pipeline::CI pipelineCI{
				.name_ = "Textured Pipeline",
				.renderPassId_ = renderPass0->rpId_,
				.vertexShader_ = vertexShader,
				.fragmentShader_ = fragmentShader,
				.topologyType_ = RAL::Driver::Pipeline::Topology::TriangleList,
				.vertexType_ = RAL::Driver::VertexType::VF3_NF3_TF2,
				.indexType_ = RAL::Driver::IndexType::UI32,
				.frontFace_ = RAL::Driver::FrontFace::CounterClockwise,
				.cullMode_ = RAL::Driver::CullMode::Back,
				.shaderBindings_ = shaderBindings,
				.enableDepthTest_ = true,
				.dbCompareOperation_ = RAL::Driver::Pipeline::DepthBuffer::CompareOperation::Less,
				.multisamplingInfo_ = multisamplingInfo

			};

			const RAL::Driver::Pipeline::Id pipelineId = renderDriver0->driver_->CreatePipeline(pipelineCI);

			CreateComponent<Pipeline>(entity0id, pipelineId);

		}

	}

	//TEST

	void GPGPUECS::StorageBuffer::WriteComponentsToGPU::Update(
		ECS2::Entity::Id entity0id,
		RenderDriver* renderDriver0,
		const GPGPUECS::StorageBuffer::NodeDataEntityIdsToComponentIndices* gPGPUECS__StorageBuffer__NodeDataEntityIdsToComponentIndices0,
		const GPGPUECS::StorageBuffer::ModelNodeDataEntityIds* gPGPUECS__StorageBuffer__ModelNodeDataEntityIds0,
		const GPGPUECS::StorageBuffer::NodeEntityIdsToComponentIndices* gPGPUECS__StorageBuffer__NodeEntityIdsToComponentIndices0,
		const GPGPUECS::StorageBuffer::ModelEntityIdsToComponentIndices* gPGPUECS__StorageBuffer__ModelEntityIdsToComponentIndices0,
		const GPGPUECS::StorageBuffer::ModelEntityIds* gPGPUECS__StorageBuffer__ModelEntityIds0,
		const GPGPUECS::StorageBuffer::WorldPositions3D* gPGPUECS__StorageBuffer__WorldPositions3D0,
		const GPGPUECS::StorageBuffer::WorldRotations3D* gPGPUECS__StorageBuffer__WorldRotations3D0,
		const GPGPUECS::StorageBuffer::WorldScales3D* gPGPUECS__StorageBuffer__WorldScales3D0,
		const GPGPUECS::StorageBuffer::BoneNodeEntities* gPGPUECS__StorageBuffer__BoneNodeEntities0,
		const GPGPUECS::StorageBuffer::BoneInverseBindPoseMatrices* gPGPUECS__StorageBuffer__BoneInverseBindPoseMatrices0) {


		auto driver = renderDriver0->driver_;

		auto createEntityIndices = [](ECS2::Entity::Id* entityIds, Common::Size entitiesNumber) {
			std::vector<Common::UInt64> indices;
			for (Common::Index i = 0; i < entitiesNumber; i++) {

				const ECS2::Entity::Id entityId = entityIds[i];

				if (entityId >= indices.size()) {
					indices.resize(entityId + 1, 0);
				}

				indices[entityId] = i;
			}
			return indices;
			};

		//Model nodes.
		Common::Size nodesEntitiesNumber = world_->GetEntitiesNumber<RENDER__MDL__NODE>();
		auto nodesComponents = GetComponents<RENDER__MDL__NODE>();
		auto* nodeEntitiesIds = std::get<ECS2::Entity::Id*>(nodesComponents);
		auto* worldPositions = std::get<WorldPosition3D*>(nodesComponents);
		auto* worldRotations = std::get<WorldRotation3D*>(nodesComponents);
		auto* worldScales = std::get<WorldScale3D*>(nodesComponents);
		auto* nodesDataEntityIds = std::get<Render::Mdl::ModelNodeDataEntityId*>(nodesComponents);
		std::vector<Common::UInt64> modelNodeIndices = createEntityIndices(nodeEntitiesIds, nodesEntitiesNumber);
		
		if (nodesEntitiesNumber > 0) {

			driver->StorageBufferWrite(
				gPGPUECS__StorageBuffer__WorldPositions3D0->sbid_,
				0,
				worldPositions,
				nodesEntitiesNumber * sizeof(WorldPosition3D));

			driver->StorageBufferWrite(
				gPGPUECS__StorageBuffer__WorldRotations3D0->sbid_,
				0,
				worldRotations,
				nodesEntitiesNumber * sizeof(WorldRotation3D));

			driver->StorageBufferWrite(
				gPGPUECS__StorageBuffer__WorldScales3D0->sbid_,
				0,
				worldScales,
				nodesEntitiesNumber * sizeof(WorldScale3D));
			driver->StorageBufferWrite(
				gPGPUECS__StorageBuffer__NodeEntityIdsToComponentIndices0->sbid_,
				0,
				modelNodeIndices.data(),
				modelNodeIndices.size() * sizeof(ECS2::Entity::Id));
			driver->StorageBufferWrite(
				gPGPUECS__StorageBuffer__ModelNodeDataEntityIds0->sbid_,
				0,
				nodesDataEntityIds,
				nodesEntitiesNumber * sizeof(Render::Mdl::ModelNodeDataEntityId));
		}

		//Model nodes data.
		Common::Size nodesDataEntitiesNumber = world_->GetEntitiesNumber<RENDER__MDL__NODE_DATA>();
		auto nodesDataComponents = GetComponents<RENDER__MDL__NODE_DATA>();
		auto* nodeDataEntitiesIds = std::get<ECS2::Entity::Id*>(nodesDataComponents);
		auto* boneInverseBindPoseMatrix = std::get<BoneInverseBindPoseMatrix*>(nodesDataComponents);
		std::vector<Common::UInt64> modelNodeDataIndices = createEntityIndices(nodeDataEntitiesIds, nodesDataEntitiesNumber);

		if (nodesDataEntitiesNumber > 0) {
			driver->StorageBufferWrite(
				gPGPUECS__StorageBuffer__BoneInverseBindPoseMatrices0->sbid_,
				0,
				boneInverseBindPoseMatrix,
				nodesDataEntitiesNumber * sizeof(BoneInverseBindPoseMatrix));
			driver->StorageBufferWrite(
				gPGPUECS__StorageBuffer__NodeDataEntityIdsToComponentIndices0->sbid_,
				0,
				modelNodeDataIndices.data(),
				modelNodeDataIndices.size() * sizeof(ECS2::Entity::Id));
		}

		//MODELS
		Common::Size modelEntitiesNumber = world_->GetEntitiesNumber<MODEL>();
		auto modelComponents = GetComponents<MODEL>();
		auto* modelsBoneNodesIds = std::get<Render::Mdl::BoneNodeEntities*>(modelComponents);
		auto* modelEntitiesIds = std::get<ECS2::Entity::Id*>(modelComponents);
		std::vector<Common::UInt64> modelIndices = createEntityIndices(modelEntitiesIds, modelEntitiesNumber);
		if (modelEntitiesNumber > 0) {
			driver->StorageBufferWrite(
				gPGPUECS__StorageBuffer__BoneNodeEntities0->sbid_,
				0,
				modelsBoneNodesIds,
				modelEntitiesNumber * sizeof(Render::Mdl::BoneNodeEntities));
			driver->StorageBufferWrite(
				gPGPUECS__StorageBuffer__ModelEntityIdsToComponentIndices0->sbid_,
				0,
				modelIndices.data(),
				modelIndices.size() * sizeof(ECS2::Entity::Id));
		}

		//MODELS DATA
		Common::Size modelDataEntitiesNumber = world_->GetEntitiesNumber<MODEL_DATA>();
		auto modelDataComponents = GetComponents<MODEL_DATA>();
		auto* modelDataBoneNodesIds = std::get<Render::Mdl::BoneNodeEntities*>(modelDataComponents);
		auto* modelDataEntitiesIds = std::get<ECS2::Entity::Id*>(modelDataComponents);
		std::vector<Common::UInt64> modelDataIndices = createEntityIndices(modelDataEntitiesIds, modelDataEntitiesNumber);

		//MESHS
		Common::Size meshEntitiesNumber = world_->GetEntitiesNumber<MESH>();
		auto meshComponents = GetComponents<MESH>();
		auto* meshVertexBuffers = std::get<DriverVertexBuffer*>(meshComponents);
		auto* meshIndexBuffers = std::get<DriverIndexBuffer*>(meshComponents);
		auto* meshModelEntityIds = std::get<Render::Mdl::ModelEntityIds*>(meshComponents);
		auto* meshIndices = std::get<Indices*>(meshComponents);
		auto* meshTextureResources = std::get<Render::DiffuseMap::TextureResource*>(meshComponents);
		auto* meshNormalTextureResources = std::get<Render::NormalMap::TextureResource*>(meshComponents);
		auto* meshEntitiesIds = std::get<ECS2::Entity::Id*>(meshComponents);
		std::vector<Common::UInt64> meshComponentsIndices = createEntityIndices(meshEntitiesIds, meshEntitiesNumber);

		if (meshEntitiesNumber > 0) {

			driver->StorageBufferWrite(
				gPGPUECS__StorageBuffer__ModelEntityIds0->sbid_,
				0,
				meshModelEntityIds,
				meshEntitiesNumber * sizeof(Render::Mdl::ModelEntityIds));
		}
	};


	void Compute::CreatePipeline::Update(
		ECS2::Entity::Id entity0id,
		const ResourceSystem* resourceSystem0,

		ECS2::Entity::Id entity1id,
		const RenderDriver* renderDriver1) {

		auto driver = renderDriver1->driver_;

		Resources::ResourceData computeTextureShaderResource
			= resourceSystem0->system_->GetResourceSynch(Subsystem::Type::Engine, "Root/Test.comp");

		std::string computeShaderCode{
			computeTextureShaderResource.GetData<Common::Byte>(),
			computeTextureShaderResource.GetSize() };

		RAL::Driver::Shader::CreateInfo computeShaderCreateInfo{
			.name_ = "ComputeShader",
			.type_ = RAL::Driver::Shader::Type::Compute,
			.code_ = computeShaderCode
		};
		auto computeShader = driver->CreateShader(computeShaderCreateInfo);

		std::vector<RAL::Driver::Shader::Binding::Layout> shaderBindings;

		RAL::Driver::Shader::Binding::Layout localPositionsBinding{
			.binding_ = 0,
			.type_ = RAL::Driver::Shader::Binding::Type::Storage,
			.stage_ = RAL::Driver::Shader::Stage::ComputeShader
		};
		shaderBindings.push_back(localPositionsBinding);

		RAL::Driver::Shader::Binding::Layout localRotationsBinding{
			.binding_ = 0,
			.type_ = RAL::Driver::Shader::Binding::Type::Storage,
			.stage_ = RAL::Driver::Shader::Stage::ComputeShader
		};
		shaderBindings.push_back(localRotationsBinding);

		RAL::Driver::Shader::Binding::Layout modelNodeAnimationStatesBinding{
			.binding_ = 0,
			.type_ = RAL::Driver::Shader::Binding::Type::Storage,
			.stage_ = RAL::Driver::Shader::Stage::ComputeShader
		};
		shaderBindings.push_back(modelNodeAnimationStatesBinding);

		RAL::Driver::Shader::Binding::Layout modelNodeAnimationsBinding{
			.binding_ = 0,
			.type_ = RAL::Driver::Shader::Binding::Type::Storage,
			.stage_ = RAL::Driver::Shader::Stage::ComputeShader
		};
		shaderBindings.push_back(modelNodeAnimationsBinding);

		RAL::Driver::Shader::Binding::Layout modelNodeDataEntityIdsBinding{
			.binding_ = 0,
			.type_ = RAL::Driver::Shader::Binding::Type::Storage,
			.stage_ = RAL::Driver::Shader::Stage::ComputeShader
		};
		shaderBindings.push_back(modelNodeDataEntityIdsBinding);

		RAL::Driver::Shader::Binding::Layout nodeDataEntityIdsToComponentIndicesBinding{
			.binding_ = 0,
			.type_ = RAL::Driver::Shader::Binding::Type::Storage,
			.stage_ = RAL::Driver::Shader::Stage::ComputeShader
		};
		shaderBindings.push_back(nodeDataEntityIdsToComponentIndicesBinding);



		std::vector<RAL::Driver::PushConstant> pushConstants;
		{
			RAL::Driver::PushConstant pushConstant{
				.shaderStage_ = RAL::Driver::Shader::Stage::ComputeShader,
				.offset_ = 0,
				.size_ = sizeof(Common::Size)
			};
			pushConstants.emplace_back(pushConstant);
		}

		RAL::Driver::ComputePipeline::CreateInfo cpci{
			.name_ = "Test compute pipeline",
			.computeShader_ = computeShader,
			.pushConstants_ = pushConstants,
			.shaderBindings_ = shaderBindings
		};

		RAL::Driver::ComputePipeline::Id coimputePipelineId = driver->CreateComputePipeline(cpci);

		CreateComponent<Compute::Pipeline>(entity1id, coimputePipelineId);

	}


	void Compute::CalculateAnimations::Update(
		ECS2::Entity::Id entity0id, const RenderDriver* renderDriver0, const Compute::Pipeline* pipeline0,
		const Animation::ModelNodeDataEntityIds* animation__ModelNodeDataEntityIds0,
		const Animation::NodeDataEntityIdsToComponentIndices* animation__NodeDataEntityIdsToComponentIndices0,
		const Animation::Mdl::Node::DriverAnimationsComponents* animation__Model__Node__DriverAnimationsComponents0,
		const Animation::Mdl::Node::AnimationsComponentsResource
		* animation__Model__Node__AnimationsComponentsResource0,
		const Animation::Mdl::Node::DriverRunningStates* animation__Model__Node__DriverRunningStates0,
		const Animation::Mdl::Node::RunningStatesResource* animation__Model__Node__RunningStatesResource0,
		const Animation::DriverLocalPosition3DComponents* animation__DriverLocalPosition3DComponents0,
		const Animation::LocalPosition3DComponentsResource* animation__LocalPosition3DComponentsResource0,
		const Animation::DriverLocalRotation3DComponents* animation__DriverLocalRotation3DComponents0,
		const Animation::LocalRotation3DComponentsResource* animation__LocalRotation3DComponentsResource0) {

		//DEBUG CODE
			{
				//RAL::Driver::ResourceSet::Id localPositionsSBResId = RAL::Driver::ResourceSet::Id::Invalid();
				//RAL::Driver::StorageBuffer::Id localPositionsSBId = RAL::Driver::ResourceSet::Id::Invalid();
				//{
				//	auto componentPointers = world_->GetComponents<NODE>();
				//	Common::Size entitiesNumber = world_->GetEntitiesNumber<NODE_ANIMATED>();
				//	auto* localPositions = std::get<LocalPosition3D*>(componentPointers);
				//	if (localPositions == nullptr) {
				//		return;
				//	}
				//	RAL::Driver::StorageBuffer::CreateInfo localPositionsSBCI{
				//		.size_ = entitiesNumber * sizeof(LocalPosition3D)
				//	};

				//	localPositionsSBId = renderDriver0->driver_->CreateStorageBuffer(localPositionsSBCI);
				//	RAL::Driver::ResourceSet::Binding localPositionsStorageBinding
				//	{
				//		.stage_ = RAL::Driver::Shader::Stage::ComputeShader,
				//		.binding_ = 0,
				//		.sbid_ = localPositionsSBId
				//	};
				//	localPositionsSBResId = renderDriver0->driver_->CreateResource(localPositionsStorageBinding);
				//	PIXBeginEvent(PIX_COLOR(255, 0, 0), "Test write");
				//	renderDriver0->driver_->StorageBufferWrite(
				//		localPositionsSBId,
				//		0,
				//		localPositions,
				//		100 * sizeof(LocalPosition3D));
				//	PIXEndEvent();
				//}

				//return;
			}
			//DEBUG CODE

			//CALCULATE ANIMATIONS FOR BONES AND HIER ANIMATED NODES!!
			auto createEntityIndices = [](ECS2::Entity::Id* entityIds, Common::Size entitiesNumber) {
				std::vector<Common::UInt64> indices;
				for (Common::Index i = 0; i < entitiesNumber; i++) {

					const ECS2::Entity::Id entityId = entityIds[i];

					if (entityId >= indices.size()) {
						indices.resize(entityId + 1, 0);
					}

					indices[entityId] = i;
				}
				return indices;
				};
			auto driver = renderDriver0->driver_;
			//{
			//	Common::Size entitiesNumber = world_->GetEntitiesNumber<NODE_ANIMATED>();

			//	if (entitiesNumber > 0) {
			//		Common::BreakPointLine();
			//	}
			//	else {
			//		return;
			//	}

			//	auto componentPointers = world_->GetComponents<NODE_ANIMATED>();

			//	if (std::get<Animation::Model::Node::RunningState*>(componentPointers) == nullptr) {
			//		return;
			//	}
			//	auto* localPositions = std::get<LocalPosition3D*>(componentPointers);
			//	auto* localRotations = std::get<LocalRotation3D*>(componentPointers);
			//	auto* modelNodeAnimationStates = std::get<Animation::Model::Node::RunningState*>(componentPointers);
			//	auto* modelNodeAnimations = std::get<Animation::Model::Node::Animations*>(componentPointers);

			//	//driver->WaitRenderEnd();

			//	PIXBeginEvent(PIX_COLOR(255, 0, 0), "Write animation data for nodes");
			//	driver->StorageBufferWrite(
			//		animation__DriverLocalPosition3DComponents0->id_,
			//		0,
			//		localPositions,
			//		entitiesNumber * sizeof(LocalPosition3D));

			//	driver->StorageBufferWrite(
			//		animation__DriverLocalRotation3DComponents0->id_,
			//		0,
			//		localRotations,
			//		entitiesNumber * sizeof(LocalRotation3D));

			//	driver->StorageBufferWrite(
			//		animation__Model__Node__DriverRunningStates0->id_,
			//		0,
			//		modelNodeAnimationStates,
			//		entitiesNumber * sizeof(Animation::Model::Node::RunningState));

			//	driver->StorageBufferWrite(
			//		animation__Model__Node__DriverAnimationsComponents0->id_,
			//		0,
			//		modelNodeAnimations,
			//		entitiesNumber * sizeof(Animation::Model::Node::Animations));
			//	PIXEndEvent();

			//	driver->StartCompute();
			//	driver->BindComputePipeline(pipeline0->pipelineId_);

			//	driver->ComputePushConstants(
			//		pipeline0->pipelineId_,
			//		sizeof(decltype(entitiesNumber)),
			//		&entitiesNumber);

			//	driver->ComputeBind(
			//		pipeline0->pipelineId_,
			//		0,
			//		{
			//			animation__LocalPosition3DComponentsResource0->id_,
			//			animation__LocalRotation3DComponentsResource0->id_,
			//			animation__Model__Node__RunningStatesResource0->id_,
			//			animation__Model__Node__AnimationsComponentsResource0->id_,

			//		});
			//	//Total Threads = Workgroups × Local Size
			//	//
			//	//Где:
			//	//- Workgroups: (X, Y, Z) из vkCmdDispatch
			//	//- Local Size: (X, Y, Z) из layout(local_size_*)
			//	// 
			//	// Шейдер:
			//	//layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;
			//	// 
			//	// Dispatch:
			//	//vkCmdDispatch(cmd, 16, 8, 1);
			//	//Всего потоков : 16 × 8 × 1 = 128 workgroups
			//	//Потоков в каждой группе : 64 × 1 × 1 = 64 threads
			//	//Общее количество потоков : 128 × 64 = 8192 threads

			//	PIXBeginEvent(PIX_COLOR(255, 0, 0), "Compute shader Dispatch");
			//	//Calculate work group number.
			//	Common::Size fullWorkGroupNumber = entitiesNumber / 64;
			//	if (entitiesNumber % 64 > 0) {
			//		++fullWorkGroupNumber;
			//	}

			//	driver->Dispatch(fullWorkGroupNumber, 1, 1);
			//	PIXEndEvent();
			//	PIXBeginEvent(PIX_COLOR(255, 0, 0), "Wait for computation");
			//	driver->EndCompute();
			//	PIXEndEvent();

			//	PIXBeginEvent(PIX_COLOR(255, 0, 0), "Compute shader read data.");
			//	driver->StorageBufferRead(
			//		animation__DriverLocalPosition3DComponents0->id_,
			//		0, entitiesNumber * sizeof(LocalPosition3D), localPositions);

			//	driver->StorageBufferRead(
			//		animation__DriverLocalRotation3DComponents0->id_,
			//		0, entitiesNumber * sizeof(LocalRotation3D), localRotations);
			//	PIXEndEvent();
			//}
			{
				//Model nodes.
				Common::Size nodeEntitiesNumber = world_->GetEntitiesNumber<RENDER__MDL__NODE>();
				auto nodesComponents = world_->GetComponents<RENDER__MDL__NODE>();
				auto* nodesEntityIds = std::get<ECS2::Entity::Id*>(nodesComponents);
				auto* localPositions = std::get<LocalPosition3D*>(nodesComponents);
				auto* localRotations = std::get<LocalRotation3D*>(nodesComponents);
				auto* modelNodeAnimationStates = std::get<Animation::Mdl::Node::RunningState*>(nodesComponents);
				auto* modelNodeDataEntityIds = std::get<Render::Mdl::ModelNodeDataEntityId*>(nodesComponents);

				//Model nodes data.
				Common::Size nodeDataEntitiesNumber = world_->GetEntitiesNumber<RENDER__MDL__NODE_DATA>();
				auto nodesDataComponents = world_->GetComponents<RENDER__MDL__NODE_DATA>();
				auto* nodesDataEntityIds = std::get<ECS2::Entity::Id*>(nodesDataComponents);
				auto* modelNodeDataAnimations = std::get<Animation::Mdl::Node::Animations*>(nodesDataComponents);
				std::vector<Common::UInt64> nodesDataIds = createEntityIndices(nodesDataEntityIds, nodeDataEntitiesNumber);

				//Models.
				Common::Size modelEntitiesNumber = world_->GetEntitiesNumber<MODEL>();
				auto modelsComponents = world_->GetComponents<MODEL>();
				auto* modelsEntityIds = std::get<ECS2::Entity::Id*>(modelsComponents);
				auto* animationInProgressEntityIds = std::get<AnimationInProgress*>(modelsComponents);



				if (nodeEntitiesNumber > 0 && nodeDataEntitiesNumber > 0) {
					Common::BreakPointLine();
				}
				else {
					return; // TODO: remove to calculate not bone nodes animation later 
				}

				BEGIN_PROFILE("Write animation data for bones");

				BEGIN_PROFILE("Write rotations for bones");
				driver->StorageBufferWrite(
					animation__DriverLocalRotation3DComponents0->id_,
					0,
					localRotations,
					nodeEntitiesNumber * sizeof(LocalRotation3D));
				END_PROFILE();


				BEGIN_PROFILE("Write positions for bones");
				driver->StorageBufferWrite(
					animation__DriverLocalPosition3DComponents0->id_,
					0,
					localPositions,
					nodeEntitiesNumber * sizeof(LocalPosition3D));
				END_PROFILE();


				BEGIN_PROFILE("Write running states for bones");
				driver->StorageBufferWrite(
					animation__Model__Node__DriverRunningStates0->id_,
					0,
					modelNodeAnimationStates,
					nodeEntitiesNumber * sizeof(Animation::Mdl::Node::RunningState));
				END_PROFILE();

				BEGIN_PROFILE("Write animations for bones");
				driver->StorageBufferWrite(
					animation__Model__Node__DriverAnimationsComponents0->id_,
					0,
					modelNodeDataAnimations,
					nodeDataEntitiesNumber * sizeof(Animation::Mdl::Node::Animations));
				END_PROFILE();

				driver->StorageBufferWrite(
					animation__ModelNodeDataEntityIds0->sbid_,
					0,
					modelNodeDataEntityIds,
					nodeEntitiesNumber * sizeof(Render::Mdl::ModelNodeDataEntityId));

				driver->StorageBufferWrite(
					animation__NodeDataEntityIdsToComponentIndices0->sbid_,
					0,
					nodesDataIds.data(),
					nodesDataIds.size() * sizeof(ECS2::Entity::Id));

				END_PROFILE();

				driver->StartCompute();
				driver->BindComputePipeline(pipeline0->id_);


				//struct AnimationInfo {
				//	Common::UInt32 modelNodesNumber_ = Common::Limits<Common::UInt32>::Max();
				//	Common::UInt32 animationIndex_ = Common::Limits<Common::UInt32>::Max();
				//	float animationDuration_ = 0.0; // Duration in ticks.
				//	float currentTime_ = 0.0; //[0..1].
				//} animationInfo{
				//	.modelNodesNumber_ = nodeEntitiesNumber,
				//	.animationIndex_ = 
				//}

				driver->ComputePushConstants(
					pipeline0->id_,
					sizeof(decltype(nodeEntitiesNumber)),
					&nodeEntitiesNumber);

				driver->ComputeBind(
					pipeline0->id_,
					0,
					{
						animation__LocalPosition3DComponentsResource0->id_,								//set 0 
						animation__LocalRotation3DComponentsResource0->id_,								//set 1
						animation__Model__Node__RunningStatesResource0->id_,							//set 2 
						animation__Model__Node__AnimationsComponentsResource0->id_,						//set 3 
						animation__ModelNodeDataEntityIds0->resourceSetId_,				//set 4 
						animation__NodeDataEntityIdsToComponentIndices0->resourceSetId_,	//set 5 

					});
				//Total Threads = Workgroups × Local Size
				//
				//Где:
				//- Workgroups: (X, Y, Z) из vkCmdDispatch
				//- Local Size: (X, Y, Z) из layout(local_size_*)
				// 
				// Шейдер:
				//layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;
				// 
				// Dispatch:
				//vkCmdDispatch(cmd, 16, 8, 1);
				//Всего потоков : 16 × 8 × 1 = 128 workgroups
				//Потоков в каждой группе : 64 × 1 × 1 = 64 threads
				//Общее количество потоков : 128 × 64 = 8192 threads

				BEGIN_PROFILE("Compute shader Dispatch");
				//Calculate work group number.
				Common::Size fullWorkGroupNumber = nodeEntitiesNumber / 64;
				if (nodeEntitiesNumber % 64 > 0) {
					++fullWorkGroupNumber;
				}

				driver->Dispatch(fullWorkGroupNumber, 1, 1);
				END_PROFILE();
				BEGIN_PROFILE("Wait for computation");
				driver->EndCompute();
				END_PROFILE();

				BEGIN_PROFILE("Compute shader read data.");
				driver->StorageBufferRead(
					animation__DriverLocalPosition3DComponents0->id_,
					0, nodeEntitiesNumber * sizeof(LocalPosition3D), localPositions);

				driver->StorageBufferRead(
					animation__DriverLocalRotation3DComponents0->id_,
					0, nodeEntitiesNumber * sizeof(LocalRotation3D), localRotations);
				END_PROFILE();
			}

	}

	//TEST

	//namespace Render {
	void BeginRenderPass::Update(
		ECS2::Entity::Id entity0id, RenderDriver* renderDriver0,
		const Render::MainRenderPass* render__MainRenderPass0,
		const Render::AttachmentSet* render__AttachmentSet0, const Render::Pipeline* render__Pipeline0) {

		auto driver = renderDriver0->driver_;

		std::vector<RAL::Driver::RP::ClearValue> clearValues;
		{
			RAL::Driver::RP::ClearValue clearValue;
			{
				clearValue.depthStencil_.depth_ = 1.0f;
			}
			clearValues.push_back(clearValue);
		}
		{
			RAL::Driver::RP::ClearValue clearValue;
			{
				clearValue.color_.uint32[0] = 0;
				clearValue.color_.uint32[1] = 0;
				clearValue.color_.uint32[2] = 0;
				clearValue.color_.uint32[3] = 255;
			}
			clearValues.push_back(clearValue);
		}
		{
			RAL::Driver::RP::ClearValue clearValue;
			{
				clearValue.color_.uint32[0] = 0;
				clearValue.color_.uint32[1] = 0;
				clearValue.color_.uint32[2] = 0;
				clearValue.color_.uint32[3] = 255;
			}
			clearValues.push_back(clearValue);
		}


		driver->BeginRenderPass(
			render__MainRenderPass0->rpId_,
			render__AttachmentSet0->attachmentsSetId_,
			clearValues,
			{ 0, 0 },
			{ 2560, 1440 });

		driver->SetViewport(0, 0, 2560, 1440);
		driver->SetScissor(0, 0, 2560, 1440);

	}

	//}
	//	void UpdateLocalPosition3D::Update(
	//		ECS2::Entity::Id entity0id,
	//		const Model* model0,
	//		const ChildModelNodeEntities* childModelNodeEntities0,
	//		const WorldPosition3D* worldPosition3D0,
	//		const WorldRotation3D* worldRotation3D0,
	//		const WorldScale3D* worldScale3D0) {
	////
	////		std::function<void(ECS2::Entity::Id,
	////			const glm::fvec3& position3D,
	////			const glm::quat& rotation3D,
	////			const glm::fvec3& scale3D)> processModelNode = [&processModelNode, this](
	////				ECS2::Entity::Id nodeEntityId,
	////				const glm::fvec3& parentPosition3D,
	////				const glm::quat& parentRotation3D,
	////				const glm::fvec3& parentScale3D) {
	////
	////					//if (IsComponentExist<Name>(nodeEntityId)) {
	////					//	if (auto name = GetComponent<Name>(nodeEntityId)->value_ == "Object_55") {
	////					//		Common::BreakPointLine();
	////					//	}
	////					//}
	////
	////					auto components = world_->GetComponents<
	////						LocalPosition3D,
	////						WorldPosition3D,
	////						LocalRotation3D,
	////						WorldRotation3D,
	////						LocalScale3D,
	////						WorldScale3D,
	////						ChildModelNodeEntities>(nodeEntityId);
	////
	////					const auto* localNodeRotation3D = std::get<LocalRotation3D*>(components);//GetComponent<LocalRotation3D>(nodeEntityId);
	////					auto* worldNodeRotation3D = std::get<WorldRotation3D*>(components);
	////
	////					const glm::quat worldRotationQuat = parentRotation3D * glm::quat(localNodeRotation3D->w_, localNodeRotation3D->x_, localNodeRotation3D->y_, localNodeRotation3D->z_);
	////
	////					worldNodeRotation3D->w_ = worldRotationQuat.w;
	////					worldNodeRotation3D->x_ = worldRotationQuat.x;
	////					worldNodeRotation3D->y_ = worldRotationQuat.y;
	////					worldNodeRotation3D->z_ = worldRotationQuat.z;
	////
	////					const auto* localNodePosition3D = std::get<LocalPosition3D*>(components);//GetComponent<LocalPosition3D>(nodeEntityId);
	////					auto* worldNodePosition3D = std::get<WorldPosition3D*>(components);//GetComponent<WorldPosition3D>(nodeEntityId);
	////
	////					{
	////						const glm::vec3 worldNodePosition3DVec = parentPosition3D + parentRotation3D * (parentScale3D * glm::vec3{ localNodePosition3D->x_, localNodePosition3D->y_, localNodePosition3D->z_ });
	////
	////
	////#pragma region Assert
	////						ASSERT_FMSG(
	////							!std::isnan(worldNodePosition3DVec.x) &&
	////							!std::isnan(worldNodePosition3DVec.y) &&
	////							!std::isnan(worldNodePosition3DVec.z), "");
	////#pragma endregion
	////
	////
	////						worldNodePosition3D->x_ = worldNodePosition3DVec.x;
	////						worldNodePosition3D->y_ = worldNodePosition3DVec.y;
	////						worldNodePosition3D->z_ = worldNodePosition3DVec.z;
	////					}
	////
	////					const auto* localNodeScale3D = std::get<LocalScale3D*>(components);//GetComponent<LocalScale3D>(nodeEntityId);
	////					auto* worldNodeScale3D = std::get<WorldScale3D*>(components);//GetComponent<WorldScale3D>(nodeEntityId);
	////
	////					worldNodeScale3D->x_ = parentScale3D.x * localNodeScale3D->x_;
	////					worldNodeScale3D->y_ = parentScale3D.y * localNodeScale3D->y_;
	////					worldNodeScale3D->z_ = parentScale3D.z * localNodeScale3D->z_;
	////
	////					const auto* childModelNodeEntities = std::get<ChildModelNodeEntities*>(components);
	////					if (childModelNodeEntities != nullptr) {
	////						const std::vector<ECS2::Entity::Id>& childEntityIds = childModelNodeEntities->childEntityIds_;
	////						for (ECS2::Entity::Id childModelNodeEntityId : childEntityIds) {
	////							const glm::fvec3 currentNodePosition3D = { worldNodePosition3D->x_, worldNodePosition3D->y_, worldNodePosition3D->z_ };
	////							const glm::quat currentNodeRotation3D = { worldNodeRotation3D->w_, worldNodeRotation3D->x_, worldNodeRotation3D->y_, worldNodeRotation3D->z_ };
	////							const glm::fvec3 currentNodeScale3D = { worldNodeScale3D->x_, worldNodeScale3D->y_, worldNodeScale3D->z_ };
	////							processModelNode(childModelNodeEntityId, currentNodePosition3D, currentNodeRotation3D, currentNodeScale3D);
	////						}
	////					}
	////
	////			};
	////
	////
	////
	////		const std::vector<ECS2::Entity::Id>& childEntityIds = childModelNodeEntities0->childEntityIds_;
	////
	////		for (ECS2::Entity::Id childModelNodeEntityId : childEntityIds) {
	////
	////			const glm::fvec3 position3D = { worldPosition3D0->x_, worldPosition3D0->y_, worldPosition3D0->z_ };
	////			const glm::quat rotation3D = { worldRotation3D0->w_, worldRotation3D0->x_, worldRotation3D0->y_, worldRotation3D0->z_ };
	////			const glm::fvec3 scale3D = { worldScale3D0->x_, worldScale3D0->y_, worldScale3D0->z_ };
	////			processModelNode(childModelNodeEntityId, position3D, rotation3D, scale3D);
	////		}
	//
	//
	//	}

	struct RTS {
		alignas(16) float rotation[4];
		alignas(16) float transform[3]; // + 4 padding
		alignas(16) float scale[3];		// + 4 padding
	};

	void UpdateHierarchicalNodeDriverTransform3D::Update(ECS2::Entity::Id entity0id, const WorldPosition3D* position3D0,
		const WorldRotation3D* rotation3D0, const WorldScale3D* scale3D0, DriverTransform3D* driverTransform3D0,
		ECS2::Entity::Id entity1id, RenderDriver* renderDriver1) {

		RTS rts{
			{ rotation3D0->w_, rotation3D0->x_, rotation3D0->y_, rotation3D0->z_ },
			{ position3D0->x_, position3D0->y_, position3D0->z_ },
			{ scale3D0->x_, scale3D0->y_, scale3D0->z_ }
		};


		RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
			.size_ = sizeof(RTS),
			.type_ = RAL::Driver::UniformBuffer::Type::Mutable
		};

		renderDriver1->driver_->FillUniformBuffer(driverTransform3D0->id_, &rts);


	};

	void AddModelToRender::Update(
		ECS2::Entity::Id entity0id, 
		RenderDriver* renderDriver0,
		const Render::MainRenderPass* render__MainRenderPass0,
		const Render::Pipeline* render__Pipeline0,

		ECS2::Entity::Id entity1id,
		const Camera* camera1,
		const Active* active1,
		const DriverViewProjectionUniformBuffer* driverViewProjectionUniformBuffer1,
		const CameraTransformResource* cameraTransformResource1, 
		
		ECS2::Entity::Id entity2id,
		const Indices* indices2,
		const DriverIndexBuffer* driverIndexBuffer2,
		const DriverVertexBuffer* driverVertexBuffer2,
		const Render::DiffuseMap::TextureResource* render__DiffuseMap__TextureResource2,
		const Render::Mdl::ModelEntityIds* render__Mdl__ModelEntityIds2,
		const Render::Mdl::ModelNodeEntityIndices* render__Mdl__ModelNodeEntityIndices2) {

		return;

		auto driver = renderDriver0->driver_;

		driver->BindPipeline(render__Pipeline0->id_);

		driver->BindVertexBuffer(driverVertexBuffer2->id_, 0);
		driver->BindIndexBuffer(driverIndexBuffer2->id_, 0);

		driver->Bind(render__Pipeline0->id_, 0,
			{
				cameraTransformResource1->id_,	// set 0
				render__DiffuseMap__TextureResource2->id_			// set 1
			});

		const std::vector<Common::Index>& nodeEntityIndices = render__Mdl__ModelNodeEntityIndices2->nodeEntityIndices_;
		struct RTS {
			alignas(16) float rotation[4];
			alignas(16) float transform[3]; // + 4 padding
			alignas(16) float scale[3];		// + 4 padding
		} rts;

		for (ECS2::Entity::Id modelEntityId : render__Mdl__ModelNodeEntityIndices2->nodeEntityIndices_) {
			const auto* modelNodeEntityIds = GetComponent<Render::Mdl::ModelNodeEntityIds>(modelEntityId);
			for (Common::Index nodeEntityIndex : nodeEntityIndices) {

				const ECS2::Entity::Id modelNodeEntity = modelNodeEntityIds->nodeEntityIds_[nodeEntityIndex];

				const auto* position = GetComponent<WorldPosition3D>(modelNodeEntity);
				const auto* rotation = GetComponent<WorldRotation3D>(modelNodeEntity);
				const auto* scale = GetComponent<WorldScale3D>(modelNodeEntity);

				rts.rotation[0] = rotation->w_;
				rts.rotation[1] = rotation->x_;
				rts.rotation[2] = rotation->y_;
				rts.rotation[3] = rotation->z_;

				rts.transform[0] = position->x_;
				rts.transform[1] = position->y_;
				rts.transform[2] = position->z_;

				//driver->PushConstants(
				//	pipeline2->id_,
				//	RAL::Driver::Shader::Stage::VertexShader, );
				//driver->Bind(pipeline2->id_, 2,
				//	{
				//		transform3DResource->id_, // set 2
				//	});
				//driver->DrawIndexed(indices1->indices_.GetIndicesNumber());

			}
		}

	}

	struct MeshData {
		Common::UInt64 nodeDataEntitiesNumber_ = 0;
		Common::UInt64 nodeEntitiesNumber_ = 0;
		Common::UInt64 modelEntitiesNumber_ = 0;
		Common::UInt64 modelDataEntitiesNumber_ = 0;
		Common::UInt64 meshComponentsIndex_ = 0;
	};

	namespace Render {
		void CreateSkeletonModelPipeline::Update(
			ECS2::Entity::Id entity0id, const RenderDriver* renderDriver0, const Render::MainRenderPass* renderPass0,
			ECS2::Entity::Id entity1id, const ResourceSystem* resourceSystem1) {


			Resources::ResourceData vertexTextureShaderResource
				= resourceSystem1->system_->GetResourceSynch(Subsystem::Type::Engine, "Root/skeleton.vert");
			Resources::ResourceData fragmentTextureShaderResource
				= resourceSystem1->system_->GetResourceSynch(Subsystem::Type::Engine, "Root/skeleton.frag");

			std::string vertexShaderCode{
				vertexTextureShaderResource.GetData<Common::Byte>(),
				vertexTextureShaderResource.GetSize() };

			std::string fragmentShaderCode{
				fragmentTextureShaderResource.GetData<Common::Byte>(),
				fragmentTextureShaderResource.GetSize() };

			RAL::Driver::Shader::CreateInfo vertexShaderCreateInfo{
				.name_ = "SkeletonVertexShader",
				.type_ = RAL::Driver::Shader::Type::Vertex,
				.code_ = vertexShaderCode
			};
			auto vertexShader = renderDriver0->driver_->CreateShader(vertexShaderCreateInfo);

			RAL::Driver::Shader::CreateInfo fragmentShaderCreateInfo{
				.name_ = "SkeletonFragmentShader",
				.type_ = RAL::Driver::Shader::Type::Fragment,
				.code_ = fragmentShaderCode
			};
			auto fragmentShader = renderDriver0->driver_->CreateShader(fragmentShaderCreateInfo);

			std::vector<RAL::Driver::Shader::Binding::Layout> shaderBindings;

			RAL::Driver::Shader::Binding::Layout cameraBinding{
				.binding_ = 0,
				.type_ = RAL::Driver::Shader::Binding::Type::Uniform,
				.stage_ = RAL::Driver::Shader::Stage::VertexShader
			};

			RAL::Driver::Shader::Binding::Layout samplerBinding{
				.binding_ = 0,
				.type_ = RAL::Driver::Shader::Binding::Type::Sampler,
				.stage_ = RAL::Driver::Shader::Stage::FragmentShader
			};

			RAL::Driver::Shader::Binding::Layout worldPositionsBinding{
				.binding_ = 0,
				.type_ = RAL::Driver::Shader::Binding::Type::Storage,
				.stage_ = RAL::Driver::Shader::Stage::VertexShader
			};

			RAL::Driver::Shader::Binding::Layout worldRotationsBinding{
				.binding_ = 0,
				.type_ = RAL::Driver::Shader::Binding::Type::Storage,
				.stage_ = RAL::Driver::Shader::Stage::VertexShader
			};

			RAL::Driver::Shader::Binding::Layout worldScalesBinding{
				.binding_ = 0,
				.type_ = RAL::Driver::Shader::Binding::Type::Storage,
				.stage_ = RAL::Driver::Shader::Stage::VertexShader
			};

			RAL::Driver::Shader::Binding::Layout modelNodeEntityIdsBinding{
				.binding_ = 0,
				.type_ = RAL::Driver::Shader::Binding::Type::Storage,
				.stage_ = RAL::Driver::Shader::Stage::VertexShader
			};

			RAL::Driver::Shader::Binding::Layout boneInverseBindPoseMaticesBinding{
				.binding_ = 0,
				.type_ = RAL::Driver::Shader::Binding::Type::Storage,
				.stage_ = RAL::Driver::Shader::Stage::VertexShader
			};

			RAL::Driver::Shader::Binding::Layout nodeIdsToIndicesBinding{
				.binding_ = 0,
				.type_ = RAL::Driver::Shader::Binding::Type::Storage,
				.stage_ = RAL::Driver::Shader::Stage::VertexShader
			};

			RAL::Driver::Shader::Binding::Layout modelIdsToIndicesBinding{
				.binding_ = 0,
				.type_ = RAL::Driver::Shader::Binding::Type::Storage,
				.stage_ = RAL::Driver::Shader::Stage::VertexShader
			};

			RAL::Driver::Shader::Binding::Layout modelEntityIdsBinding{
				.binding_ = 0,
				.type_ = RAL::Driver::Shader::Binding::Type::Storage,
				.stage_ = RAL::Driver::Shader::Stage::VertexShader
			};

			RAL::Driver::Shader::Binding::Layout modelNodeDataEntityIds{
				.binding_ = 0,
				.type_ = RAL::Driver::Shader::Binding::Type::Storage,
				.stage_ = RAL::Driver::Shader::Stage::VertexShader
			};

			RAL::Driver::Shader::Binding::Layout modelNodeDataEntityIdsToComponentIndices{
				.binding_ = 0,
				.type_ = RAL::Driver::Shader::Binding::Type::Storage,
				.stage_ = RAL::Driver::Shader::Stage::VertexShader
			};

			RAL::Driver::Shader::Binding::Layout normalSamplerBinding{
				.binding_ = 0,
				.type_ = RAL::Driver::Shader::Binding::Type::Sampler,
				.stage_ = RAL::Driver::Shader::Stage::FragmentShader
			};



			std::vector<RAL::Driver::PushConstant> pushConstants;
			{
				RAL::Driver::PushConstant pushConstant{
					.shaderStage_ = RAL::Driver::Shader::Stage::VertexShader,
					.offset_ = 0,
					.size_ = sizeof(MeshData)
				};
				pushConstants.emplace_back(pushConstant);
			}

			shaderBindings.push_back(cameraBinding);
			shaderBindings.push_back(samplerBinding);
			shaderBindings.push_back(worldPositionsBinding);
			shaderBindings.push_back(worldRotationsBinding);
			shaderBindings.push_back(worldScalesBinding);
			shaderBindings.push_back(modelNodeEntityIdsBinding);
			shaderBindings.push_back(boneInverseBindPoseMaticesBinding);
			shaderBindings.push_back(nodeIdsToIndicesBinding);
			shaderBindings.push_back(modelIdsToIndicesBinding);
			shaderBindings.push_back(modelEntityIdsBinding);
			shaderBindings.push_back(modelNodeDataEntityIds);
			shaderBindings.push_back(modelNodeDataEntityIdsToComponentIndices);
			shaderBindings.push_back(normalSamplerBinding);


			auto multisamplingInfo = std::make_shared<RAL::Driver::Pipeline::MultisamplingInfo>();
			{
				multisamplingInfo->samplesCount_ = RAL::Driver::SamplesCount::SamplesCount_8;
			}

			RAL::Driver::Pipeline::CI pipelineCI{
				.name_ = "Skeleton Pipeline",
				.renderPassId_ = renderPass0->rpId_,
				.vertexShader_ = vertexShader,
				.fragmentShader_ = fragmentShader,
				.topologyType_ = RAL::Driver::Pipeline::Topology::TriangleList,
				.vertexType_ = RAL::Driver::VertexType::VF3_NF3_TF2_BIDUB4_WUB4,
				.indexType_ = RAL::Driver::IndexType::UI32,
				.frontFace_ = RAL::Driver::FrontFace::CounterClockwise,
				.cullMode_ = RAL::Driver::CullMode::Back,
				.pushConstants_ = pushConstants,
				.shaderBindings_ = shaderBindings,
				.enableDepthTest_ = true,
				.dbCompareOperation_ = RAL::Driver::Pipeline::DepthBuffer::CompareOperation::Less,
				.multisamplingInfo_ = multisamplingInfo
			};

			const RAL::Driver::Pipeline::Id pipelineId = renderDriver0->driver_->CreatePipeline(pipelineCI);

			CreateComponent<SkeletonModelPipeline>(entity0id, pipelineId);

		}
	}


	void AddSkeletonModelToRender::Update(
		ECS2::Entity::Id entity0id, RenderDriver* renderDriver0, const Render::MainRenderPass* renderPass0,
		const Render::SkeletonModelPipeline* skeletonModelPipeline0,
		const GPGPUECS::StorageBuffer::NodeDataEntityIdsToComponentIndices* gPGPUECS__StorageBuffer__NodeDataEntityIdsToComponentIndices0,
		const GPGPUECS::StorageBuffer::ModelNodeDataEntityIds* gPGPUECS__StorageBuffer__ModelNodeDataEntityIds0,
		const GPGPUECS::StorageBuffer::NodeEntityIdsToComponentIndices* gPGPUECS__StorageBuffer__NodeEntityIdsToComponentIndices0,
		const GPGPUECS::StorageBuffer::ModelEntityIdsToComponentIndices* gPGPUECS__StorageBuffer__ModelEntityIdsToComponentIndices0,
		const GPGPUECS::StorageBuffer::ModelEntityIds* gPGPUECS__StorageBuffer__ModelEntityIds0,
		const GPGPUECS::StorageBuffer::WorldPositions3D* gPGPUECS__StorageBuffer__WorldPositions3D0,
		const GPGPUECS::StorageBuffer::WorldRotations3D* gPGPUECS__StorageBuffer__WorldRotations3D0,
		const GPGPUECS::StorageBuffer::WorldScales3D* gPGPUECS__StorageBuffer__WorldScales3D0,
		const GPGPUECS::StorageBuffer::BoneNodeEntities* gPGPUECS__StorageBuffer__BoneNodeEntities0,
		const GPGPUECS::StorageBuffer::BoneInverseBindPoseMatrices* gPGPUECS__StorageBuffer__BoneInverseBindPoseMatrices0,
		ECS2::Entity::Id entity1id,
		const Camera* camera1,
		const Active* active1,
		const DriverViewProjectionUniformBuffer* driverViewProjectionUniformBuffer1,
		const CameraTransformResource* cameraTransformResource1) {

		auto createEntityIndices = [](ECS2::Entity::Id* entityIds, Common::Size entitiesNumber) {
			std::vector<Common::UInt64> indices;
			for (Common::Index i = 0; i < entitiesNumber; i++) {

				const ECS2::Entity::Id entityId = entityIds[i];

				if (entityId >= indices.size()) {
					indices.resize(entityId + 1, 0);
				}

				indices[entityId] = i;
			}
			return indices;
			};

		auto driver = renderDriver0->driver_;

		////Model nodes.
		Common::Size nodesEntitiesNumber = world_->GetEntitiesNumber<RENDER__MDL__NODE>();
		//auto nodesComponents = GetComponents<RENDER__MDL__NODE>();
		//auto* nodeEntitiesIds = std::get<ECS2::Entity::Id*>(nodesComponents);
		//auto* worldPositions = std::get<WorldPosition3D*>(nodesComponents);
		//auto* worldRotations = std::get<WorldRotation3D*>(nodesComponents);
		//auto* worldScales = std::get<WorldScale3D*>(nodesComponents);
		//auto* nodesDataEntityIds = std::get<Render::Mdl::ModelNodeDataEntityId*>(nodesComponents);
		//std::vector<Common::UInt64> modelNodeIndices = createEntityIndices(nodeEntitiesIds, nodesEntitiesNumber);

		////Model nodes data.
		Common::Size nodesDataEntitiesNumber = world_->GetEntitiesNumber<RENDER__MDL__NODE_DATA>();
		//auto nodesDataComponents = GetComponents<RENDER__MDL__NODE_DATA>();
		//auto* nodeDataEntitiesIds = std::get<ECS2::Entity::Id*>(nodesDataComponents);
		//auto* boneInverseBindPoseMatrix = std::get<BoneInverseBindPoseMatrix*>(nodesDataComponents);
		//std::vector<Common::UInt64> modelNodeDataIndices = createEntityIndices(nodeDataEntitiesIds, nodesDataEntitiesNumber);

		////MODELS
		Common::Size modelEntitiesNumber = world_->GetEntitiesNumber<MODEL>();
		//auto modelComponents = GetComponents<MODEL>();
		//auto* modelsBoneNodesIds = std::get<Render::Mdl::BoneNodeEntities*>(modelComponents);
		//auto* modelEntitiesIds = std::get<ECS2::Entity::Id*>(modelComponents);
		//std::vector<Common::UInt64> modelIndices = createEntityIndices(modelEntitiesIds, modelEntitiesNumber);

		////MODELS DATA
		Common::Size modelDataEntitiesNumber = world_->GetEntitiesNumber<MODEL_DATA>();
		//auto modelDataComponents = GetComponents<MODEL_DATA>();
		//auto* modelDataBoneNodesIds = std::get<Render::Mdl::BoneNodeEntities*>(modelDataComponents);
		//auto* modelDataEntitiesIds = std::get<ECS2::Entity::Id*>(modelDataComponents);
		//std::vector<Common::UInt64> modelDataIndices = createEntityIndices(modelDataEntitiesIds, modelDataEntitiesNumber);

		////MESHS
		Common::Size meshEntitiesNumber = world_->GetEntitiesNumber<MESH>();
		auto meshComponents = GetComponents<MESH>();
		auto* meshVertexBuffers = std::get<DriverVertexBuffer*>(meshComponents);
		auto* meshIndexBuffers = std::get<DriverIndexBuffer*>(meshComponents);
		auto* meshModelEntityIds = std::get<Render::Mdl::ModelEntityIds*>(meshComponents);
		auto* meshIndices = std::get<Indices*>(meshComponents);
		auto* meshTextureResources = std::get<Render::DiffuseMap::TextureResource*>(meshComponents);
		auto* meshNormalTextureResources = std::get<Render::NormalMap::TextureResource*>(meshComponents);
		auto* meshEntitiesIds = std::get<ECS2::Entity::Id*>(meshComponents);
		std::vector<Common::UInt64> meshComponentsIndices = createEntityIndices(meshEntitiesIds, meshEntitiesNumber);

		//driver->StorageBufferWrite(
		//	gPGPUECS__StorageBuffer__BoneNodeEntities0->sbid_,
		//	0,
		//	modelsBoneNodesIds,
		//	modelEntitiesNumber * sizeof(Render::Mdl::BoneNodeEntities));

		//driver->StorageBufferWrite(
		//	gPGPUECS__StorageBuffer__WorldPositions3D0->sbid_,
		//	0,
		//	worldPositions,
		//	nodesEntitiesNumber * sizeof(WorldPosition3D));

		//driver->StorageBufferWrite(
		//	gPGPUECS__StorageBuffer__WorldRotations3D0->sbid_,
		//	0,
		//	worldRotations,
		//	nodesEntitiesNumber * sizeof(WorldRotation3D));

		//driver->StorageBufferWrite(
		//	gPGPUECS__StorageBuffer__WorldScales3D0->sbid_,
		//	0,
		//	worldScales,
		//	nodesEntitiesNumber * sizeof(WorldScale3D));

		//driver->StorageBufferWrite(
		//	gPGPUECS__StorageBuffer__BoneInverseBindPoseMatrices0->sbid_,
		//	0,
		//	boneInverseBindPoseMatrix,
		//	nodesDataEntitiesNumber * sizeof(BoneInverseBindPoseMatrix));

		//driver->StorageBufferWrite(
		//	gPGPUECS__StorageBuffer__NodeDataEntityIdsToComponentIndices0->sbid_,
		//	0,
		//	modelNodeDataIndices.data(),
		//	modelNodeDataIndices.size() * sizeof(ECS2::Entity::Id));

		//driver->StorageBufferWrite(
		//	gPGPUECS__StorageBuffer__NodeEntityIdsToComponentIndices0->sbid_,
		//	0,
		//	modelNodeIndices.data(),
		//	modelNodeIndices.size() * sizeof(ECS2::Entity::Id));

		//driver->StorageBufferWrite(
		//	gPGPUECS__StorageBuffer__ModelEntityIdsToComponentIndices0->sbid_,
		//	0,
		//	modelIndices.data(),
		//	modelIndices.size() * sizeof(ECS2::Entity::Id));

		//driver->StorageBufferWrite(
		//	gPGPUECS__StorageBuffer__ModelEntityIds0->sbid_,
		//	0,
		//	meshModelEntityIds,
		//	meshEntitiesNumber * sizeof(Render::Mdl::ModelEntityIds));

		//driver->StorageBufferWrite(
		//	gPGPUECS__StorageBuffer__ModelNodeDataEntityIds0->sbid_,
		//	0,
		//	nodesDataEntityIds,
		//	nodesEntitiesNumber * sizeof(Render::Mdl::ModelNodeDataEntityId));

		driver->BindPipeline(skeletonModelPipeline0->id_);

		driver->Bind(skeletonModelPipeline0->id_, 2,
			{
			gPGPUECS__StorageBuffer__WorldPositions3D0->resourceSetId_,						// set 2
			gPGPUECS__StorageBuffer__WorldRotations3D0->resourceSetId_,						// set 3
			gPGPUECS__StorageBuffer__WorldScales3D0->resourceSetId_,						// set 4
			gPGPUECS__StorageBuffer__BoneNodeEntities0->resourceSetId_,						// set 5
			gPGPUECS__StorageBuffer__BoneInverseBindPoseMatrices0->resourceSetId_,			// set 6
			gPGPUECS__StorageBuffer__NodeEntityIdsToComponentIndices0->resourceSetId_,		// set 7
			gPGPUECS__StorageBuffer__ModelEntityIdsToComponentIndices0->resourceSetId_,		// set 8
			gPGPUECS__StorageBuffer__ModelEntityIds0->resourceSetId_,						// set 9
			gPGPUECS__StorageBuffer__ModelNodeDataEntityIds0->resourceSetId_,				// set 10
			gPGPUECS__StorageBuffer__NodeDataEntityIdsToComponentIndices0->resourceSetId_	// set 11
			}
		);

		for (Common::Index i = 0; i < meshEntitiesNumber; i++) {
			if (meshEntitiesIds[i].IsValid()) {

				auto* textureResource = meshTextureResources + i;
				const ECS2::ComponentsFilter meshComponentsFilter = GetComponentsFilter(meshEntitiesIds[i]);
				if (!meshComponentsFilter.IsSet<
					DriverVertexBuffer,
					DriverIndexBuffer,
					Render::DiffuseMap::TextureResource,
					Render::NormalMap::TextureResource,
					Render::Mdl::ModelEntityIds,
					VertexBones,
					Indices>()) {
					continue;
				}

				if (textureResource->id_.IsInvalid()) {
					continue;
				}

				auto* driverVertexBuffer = meshVertexBuffers + i;
				driver->BindVertexBuffer(driverVertexBuffer->id_, 0);

				auto* driverIndexBuffer = meshIndexBuffers + i;
				driver->BindIndexBuffer(driverIndexBuffer->id_, 0);

				auto* indices = meshIndices + i;

				auto* modelEntityIds = meshModelEntityIds + i;

				driver->Bind(skeletonModelPipeline0->id_, 0,
					{
						cameraTransformResource1->id_,													// set 0
						textureResource->id_															// set 1

					});

				driver->Bind(skeletonModelPipeline0->id_, 12,
					{
						(meshNormalTextureResources + i)->id_												// set 12
					});

				auto invalidEntityIdIt = std::find(
					modelEntityIds->modelEntityIds_.begin(),
					modelEntityIds->modelEntityIds_.end(),
					ECS2::Entity::Id::invalid_);

				auto meshModelEntitiesNumber = std::distance(modelEntityIds->modelEntityIds_.begin(), invalidEntityIdIt);

				MeshData meshInfo{  };


				meshInfo.nodeDataEntitiesNumber_ = nodesDataEntitiesNumber;
				meshInfo.nodeEntitiesNumber_ = nodesEntitiesNumber;
				meshInfo.modelEntitiesNumber_ = modelEntitiesNumber;
				meshInfo.modelDataEntitiesNumber_ = modelDataEntitiesNumber;
				meshInfo.meshComponentsIndex_ = i;


				driver->PushConstants(
					skeletonModelPipeline0->id_,
					RAL::Driver::Shader::Stage::VertexShader,
					sizeof(MeshData),
					&meshInfo);

				driver->DrawIndexed(
					indices->indices_.GetIndicesNumber(),
					0,
					meshModelEntitiesNumber);

			}
		}

	}


	void EndRenderPass::Update(ECS2::Entity::Id entity0id, RenderDriver* renderDriver0, const Render::MainRenderPass* renderPass0,
		const Render::Pipeline* pipeline0) {

		auto driver = renderDriver0->driver_;

		driver->EndSubpass();
		driver->EndRenderPass();

		//driver->Show(renderPass0->textureIds_[1]);

	}
}