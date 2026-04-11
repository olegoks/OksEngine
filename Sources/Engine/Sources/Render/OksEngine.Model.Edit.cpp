#pragma once

#include <Render\auto_OksEngine.Model.hpp>

namespace OksEngine
{
	void EditAnimationInProgress(std::shared_ptr<ECS2::World> ecsWorld, ECS2::Entity::Id ecsEntityId, AnimationInProgress* animationInProgress) {
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

	void EditModelAnimations(std::shared_ptr<ECS2::World> ecsWorld, ECS2::Entity::Id ecsEntityId, ModelAnimations* modelAnimations) {
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

	namespace Render::Model {

		namespace Node {

			namespace Data {
				void EditEntityIds(std::shared_ptr<ECS2::World> ecsWorld, ECS2::Entity::Id ecsEntityId, Render::Model::Node::Data::EntityIds* modelNodeEntityIds) {
					ImGui::PushID(Render::Model::Node::ChildNodeEntityIds::GetTypeId());
					for (ECS2::Entity::Id modelNodeEntityId : modelNodeEntityIds->nodeDataEntityIds_) {
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
				void EditEntityIdIndices(std::shared_ptr<ECS2::World> ecsWorld, ECS2::Entity::Id ecsEntityId, Node::Data::EntityIdIndices* modelNodeEntityIndices) {
					ImGui::PushID(Render::Model::Node::EntityIndices::GetTypeId());

					//auto nodeEntityIdIt = std::find(modelNodeEntityIds.begin(), modelNodeEntityIds.end(), nodeEntityId);
					//nodeEntityIndices.push_back(std::distance(modelNodeEntityIds.begin(), nodeEntityIdIt));

					auto firstInvalidIndexIt = std::find(
						modelNodeEntityIndices->indices_.begin(),
						modelNodeEntityIndices->indices_.end(),
						Common::Limits<Common::UInt64>::Max());

					Common::UInt64 firstInvalidIndexIndex =
						(firstInvalidIndexIt != modelNodeEntityIndices->indices_.end())
						? (std::distance(modelNodeEntityIndices->indices_.begin(), firstInvalidIndexIt))
						: modelNodeEntityIndices->indices_.max_size();

					ImGui::Begin("Mesh node indices.");
					for (int i = 0; i < firstInvalidIndexIndex; i++) {
						ImGui::Text("Index %d", i, modelNodeEntityIndices->indices_[i]);
					}
					ImGui::End();

					ImGui::PopID();
				}
			}
			void EditEntityIds(std::shared_ptr<ECS2::World> ecsWorld, ECS2::Entity::Id ecsEntityId, Render::Model::Node::EntityIds* modelNodeEntityIds) {
				ImGui::PushID(Render::Model::Node::ChildNodeEntityIds::GetTypeId());
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

			void EditEntityIndices(std::shared_ptr<ECS2::World> ecsWorld, ECS2::Entity::Id ecsEntityId, Node::EntityIndices* modelNodeEntityIndices) {
				ImGui::PushID(Render::Model::Node::EntityIndices::GetTypeId());

				//auto nodeEntityIdIt = std::find(modelNodeEntityIds.begin(), modelNodeEntityIds.end(), nodeEntityId);
				//nodeEntityIndices.push_back(std::distance(modelNodeEntityIds.begin(), nodeEntityIdIt));

				auto firstInvalidIndexIt = std::find(
					modelNodeEntityIndices->nodeEntityIndices_.begin(),
					modelNodeEntityIndices->nodeEntityIndices_.end(),
					Common::Limits<Common::UInt64>::Max());

				Common::UInt64 firstInvalidIndexIndex =
					(firstInvalidIndexIt != modelNodeEntityIndices->nodeEntityIndices_.end())
					? (std::distance(modelNodeEntityIndices->nodeEntityIndices_.begin(), firstInvalidIndexIt))
					: modelNodeEntityIndices->nodeEntityIndices_.max_size();

				ImGui::Begin("Mesh node indices.");
				for (int i = 0; i < firstInvalidIndexIndex; i++) {
					ImGui::Text("Index %d", i, modelNodeEntityIndices->nodeEntityIndices_[i]);
				}
				ImGui::End();

				ImGui::PopID();
			}
		}
		void BindEntityId(::Lua::Context& context) {

			context.GetGlobalNamespace()
				.beginClass<Model::EntityId>("Render_Model_EntityId")
				.addProperty("id",
					[](const Model::EntityId* modelEntity) {
						return modelEntity->id_.GetRawValue();
					},
					[](Model::EntityId* modelEntity, ECS2::Entity::Id::ValueType value) {
						return modelEntity->id_ = value;
					}
				)
				.endClass();
		};
	}

}