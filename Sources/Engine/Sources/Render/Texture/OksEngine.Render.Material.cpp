#pragma once
#include <Render\Texture\auto_OksEngine.Render.Material.hpp>

namespace OksEngine
{

	namespace Render
	{
		namespace Material
		{
			void CreateResourceSet::Update(
				ECS2::Entity::Id entity0id,
				OksEngine::RenderDriver* renderDriver0,

				ECS2::Entity::Id entity1id,
				const OksEngine::Render::Material::Tag* material__Tag1) {

				const ECS2::ComponentsFilter materialCF = GetComponentsFilter(entity1id);

				RAL::Driver::ResourceSet::CI1 RSCI;
				RSCI.bindingsNumber_ = 4;
				const Texture::DriverId* diffuseDriverId = nullptr;
				if (materialCF.IsSet<Render::Texture::Type::DiffuseMap::EntityId>()) {
					const ECS2::Entity::Id diffuseMapEntityId = GetComponent<Render::Texture::Type::DiffuseMap::EntityId>(entity1id)->id_;
					const ECS2::ComponentsFilter textureCF = GetComponentsFilter(diffuseMapEntityId);
					if (!textureCF.IsSet<Texture::DriverId>()) {
						return;
					}
					const Texture::DriverId* diffuseDriverId = GetComponent<Texture::DriverId>(diffuseMapEntityId);
					RSCI.bindings_[0] = RAL::Driver::Resource::Binding
					{
						.stage_ = RAL::Driver::Shader::Stage::FragmentShader,
						.binding_ = 0,
						.textureId_ = diffuseDriverId->id_
					};
				}
				if (materialCF.IsSet<Render::Texture::Type::NormalMap::EntityId>()) {
					const ECS2::Entity::Id diffuseMapEntityId = GetComponent<Render::Texture::Type::NormalMap::EntityId>(entity1id)->id_;
					const ECS2::ComponentsFilter textureCF = GetComponentsFilter(diffuseMapEntityId);
					if (!textureCF.IsSet<Texture::DriverId>()) {
						return;
					}
					const Texture::DriverId* normalDriverId = GetComponent<Texture::DriverId>(diffuseMapEntityId);
					RSCI.bindings_[1] = RAL::Driver::Resource::Binding
					{
						.stage_ = RAL::Driver::Shader::Stage::FragmentShader,
						.binding_ = 1,
						.textureId_ = normalDriverId->id_
					};
				}

				if (materialCF.IsSet<Render::Texture::Type::MetallicMap::EntityId>()) {
					const ECS2::Entity::Id diffuseMapEntityId = GetComponent<Render::Texture::Type::MetallicMap::EntityId>(entity1id)->id_;
					const ECS2::ComponentsFilter textureCF = GetComponentsFilter(diffuseMapEntityId);
					if (!textureCF.IsSet<Texture::DriverId>()) {
						return;
					}
					const Texture::DriverId* metallicDriverId = GetComponent<Texture::DriverId>(diffuseMapEntityId);
					RSCI.bindings_[2] = RAL::Driver::Resource::Binding
					{
						.stage_ = RAL::Driver::Shader::Stage::FragmentShader,
						.binding_ = 2,
						.textureId_ = metallicDriverId->id_
					};
				}

				const RAL::Driver::Resource::Id textureRSId = renderDriver0->driver_->CreateResourceSet(RSCI);
				CreateComponent<Material::ResourceSet>(entity1id, textureRSId);
			};

		}

	} // namespace Render

} // namespace OksEngine