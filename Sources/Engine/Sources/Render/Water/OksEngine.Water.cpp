#pragma once
#include <Render\Water\auto_OksEngine.Water.hpp>

namespace OksEngine
{

	namespace Render
	{
		namespace Water
		{
			void CreatePipeline::Update(ECS2::Entity::Id entity0id, const OksEngine::RenderDriver* renderDriver0,
				const OksEngine::Render::MainRenderPass* mainRenderPass0, ECS2::Entity::Id entity1id,
				const OksEngine::ResourceSystem* resourceSystem1) {};



			void Render::Update(ECS2::Entity::Id entity0id, OksEngine::RenderDriver* renderDriver0,
				const OksEngine::Render::MainRenderPass* mainRenderPass0,
				const OksEngine::Render::AttachmentSet* attachmentSet0,
				const OksEngine::Render::Water::PipelineId* pipelineId0, ECS2::Entity::Id entity1id,
				const OksEngine::Camera* camera1, const OksEngine::Active* active1,
				const OksEngine::DriverViewProjectionUniformBuffer* driverViewProjectionUniformBuffer1,
				const OksEngine::CameraTransformResource* cameraTransformResource1, ECS2::Entity::Id entity2id,
				const OksEngine::Render::Water::Tag* water__Tag2,
				const OksEngine::Render::Water::PipelineId* water__PipelineId2,
				const OksEngine::Render::DiffuseMap::TextureResource* render__DiffuseMap__TextureResource2,
				const OksEngine::Render::NormalMap::TextureResource* render__NormalMap__TextureResource2,
				const OksEngine::Render::AmbientMap::TextureResource* render__AmbientMap__TextureResource2) {};

		}
	}

} // namespace OksEngine