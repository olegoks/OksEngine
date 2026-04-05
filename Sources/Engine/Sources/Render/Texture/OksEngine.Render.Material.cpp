#pragma once
#include <Render\Texture\auto_OksEngine.Render.Material.hpp>

namespace OksEngine
{

	namespace Render
	{
		namespace Material
		{

			void CreateTexturesResourceSet::Update(
				ECS2::Entity::Id entity0id,
				OksEngine::RenderDriver* renderDriver0) {

				//Resource set with all textures.

				////Storage buffer for Material structs with textures indices.
				//RAL::Driver::RS::Binding_::Layout  infosBindingLayout{
				//	.binding_ = 0,
				//	.type_ = RAL::Driver::Shader::Binding::Type::StorageBuffer,
				//	.count_ = 1,
				//	.stage_ = RAL::Driver::Shader::Stage::AllShaders,
				//};

				//All textures.
				RAL::Driver::RS::Binding_::Layout diffuseMapsBindingLayout{
					.binding_ = 0,
					.type_ = RAL::Driver::Shader::Binding::Type::Sampler,
					.count_ = 4096,
					.stage_ = RAL::Driver::Shader::Stage::FragmentShader,
				};

				////Normal maps binding array.
				//RAL::Driver::RS::Binding_::Layout normalMapsBindingLayout{
				//	.binding_ = 1,
				//	.type_ = RAL::Driver::Shader::Binding::Type::Sampler,
				//	.count_ = 4096,
				//	.stage_ = RAL::Driver::Shader::Stage::FragmentShader,
				//};

				////Metallic maps binding array.
				//RAL::Driver::RS::Binding_::Layout metallicMapsBindingLayout{
				//	.binding_ = 2,
				//	.type_ = RAL::Driver::Shader::Binding::Type::Sampler,
				//	.count_ = 4096,
				//	.stage_ = RAL::Driver::Shader::Stage::FragmentShader,
				//};

				RAL::Driver::RS::CI3 ci{
					.bindings_ = {
						//infosBindingLayout,
						diffuseMapsBindingLayout/*,
						normalMapsBindingLayout,
						metallicMapsBindingLayout*/ }
				};

				////Create material resource set.
				RAL::Driver::RS::Id rsId = renderDriver0->driver_->CreateResourceSet(ci);

				//RAL::Driver::RS::UpdateInfo updateInfo;
				//{
				//	updateInfo.id_ = rsId;
				//}

				//RAL::Driver::SB::CI SBCI{
				//	.size_ = 4096 * sizeof(Material::Info)
				//};

				//const RAL::Driver::SB::Id SBId = renderDriver0->driver_->CreateStorageBuffer(SBCI);

				//RAL::Driver::RS::Binding_::UpdateInfo::StorageBuffer SBUpdateInfo{
				//		.sbid_ = SBId,
				//		.offset_ = 0,
				//		.size_ = 4096 * sizeof(Material::Info)
				//};

				//RAL::Driver::RS::Binding_::UpdateInfo bindingUpdateInfo{
				//	.binding_ = 0,
				//	.arrayElement_ = 0,
				//	.resourcesCount_ = 1,
				//	.type_ = RAL::Driver::Shader::Binding::Type::StorageBuffer,
				//	.SBInfo_ = {SBUpdateInfo}
				//};

				//RAL::Driver::RS::UpdateInfo RSUpdateInfo{
				//	.id_ = rsId,
				//	.bindingUpdateInfos_ = { bindingUpdateInfo }
				//};
				////Bind storage buffer to binding 0.
				//renderDriver0->driver_->UpdateResourceSet(RSUpdateInfo);

				CreateComponent<Render::Material::ResourceSet_>(
					entity0id,
					rsId,
					//SBId,
					////Reserve binding array element with invalid index.
					//Common::Bitset<4096>{}.SetBit(Material::InvalidArrayIndex),
					Common::Bitset<4096>{}.SetBit(Material::InvalidArrayIndex));


			}

			void CreateInfoResourceSet::Update(
				ECS2::Entity::Id entity0id,
				OksEngine::RenderDriver* renderDriver0,
				OksEngine::Render::Material::ResourceSet_* render__Material__ResourceSet_0,

				ECS2::Entity::Id entity1id,
				const OksEngine::Render::Material::Tag* material__Tag1) {

				const ECS2::ComponentsFilter materialCF = GetComponentsFilter(entity1id);

				//[[deprecated]]
				RAL::Driver::ResourceSet::CI1 RSCI;
				RSCI.bindingsNumber_ = 4;
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

				//Resource set per material.
				CreateComponent<Material::ResourceSet>(entity1id, textureRSId);
				//[[deprecated]]

				//One resource set for all materials.
				RAL::Driver::RS::UpdateInfo updateInfo;
				{
					updateInfo.id_ = render__Material__ResourceSet_0->id_;
				}

				Material::Info materialInfo;

				if (materialCF.IsSet<Render::Texture::Type::DiffuseMap::EntityId>()) {
					const ECS2::Entity::Id diffuseMapEntityId = GetComponent<Render::Texture::Type::DiffuseMap::EntityId>(entity1id)->id_;
					const ECS2::ComponentsFilter textureCF = GetComponentsFilter(diffuseMapEntityId);
					if (!textureCF.IsSet<Texture::DriverId>()) {
						return;
					}
					const Texture::DriverId* diffuseDriverId = GetComponent<Texture::DriverId>(diffuseMapEntityId);

					Common::Bitset<4096>::BitIndex freeIndex = render__Material__ResourceSet_0->freeElements_.FindFirstResettedBit();

					RAL::Driver::RS::Binding_::UpdateInfo::Image image{
						.textureId_ = diffuseDriverId->id_,
						.state_ = RAL::Driver::Texture::State::DataForShaderRead,
					};

					RAL::Driver::RS::Binding_::UpdateInfo bindingUpdateInfo{
						.binding_ = 0,// one binding for all textures.
						.arrayElement_ = freeIndex,
						.resourcesCount_ = 1,
						.type_ = RAL::Driver::Shader::Binding::Type::Sampler,
						.imageInfos_ = {image}
					};
					render__Material__ResourceSet_0->freeElements_.SetBit(freeIndex);
					materialInfo.diffuseMapIndex_ = freeIndex;
					updateInfo.bindingUpdateInfos_.push_back(bindingUpdateInfo);
					CreateComponent<Render::Texture::BindingArrayIndex>(diffuseMapEntityId, freeIndex);

				}
				if (materialCF.IsSet<Render::Texture::Type::NormalMap::EntityId>()) {
					const ECS2::Entity::Id normalMapEntityId = GetComponent<Render::Texture::Type::NormalMap::EntityId>(entity1id)->id_;
					const ECS2::ComponentsFilter textureCF = GetComponentsFilter(normalMapEntityId);
					if (!textureCF.IsSet<Texture::DriverId>()) {
						return;
					}
					const Texture::DriverId* diffuseDriverId = GetComponent<Texture::DriverId>(normalMapEntityId);

					Common::Bitset<4096>::BitIndex freeIndex = render__Material__ResourceSet_0->freeElements_.FindFirstResettedBit();

					RAL::Driver::RS::Binding_::UpdateInfo::Image image{
						.textureId_ = diffuseDriverId->id_,
						.state_ = RAL::Driver::Texture::State::DataForShaderRead,
					};

					RAL::Driver::RS::Binding_::UpdateInfo bindingUpdateInfo{
						.binding_ = 0, // one binding for all textures.
						.arrayElement_ = freeIndex,
						.resourcesCount_ = 1,
						.type_ = RAL::Driver::Shader::Binding::Type::Sampler,
						.imageInfos_ = {image}
					};
					render__Material__ResourceSet_0->freeElements_.SetBit(freeIndex);
					materialInfo.normalMapIndex_ = freeIndex;
					updateInfo.bindingUpdateInfos_.push_back(bindingUpdateInfo);
					CreateComponent<Render::Texture::BindingArrayIndex>(normalMapEntityId, freeIndex);
				}

				//Common::Bitset<4096>::BitIndex freeIndex = render__Material__ResourceSet_0->infoBinding_.FindFirstResettedBit();

				//RAL::Driver::RS::Binding_::UpdateInfo::StorageBuffer SBUpdateInfo{
				//		.sbid_ = render__Material__ResourceSet_0->infosSBId_,
				//		.offset_ = freeIndex * sizeof(Material::Info),
				//		.size_ = sizeof(Material::Info)
				//};

				//RAL::Driver::RS::Binding_::UpdateInfo bindingUpdateInfo{
				//	.binding_ = 0,
				//	.arrayElement_ = 0, // We have one SB that we share between many Material::Info structures
				//	.resourcesCount_ = 1,
				//	.type_ = RAL::Driver::Shader::Binding::Type::StorageBuffer,
				//	.SBInfo_ = { SBUpdateInfo }
				//};

				//CreateComponent<Material::InfoBindingArrayIndex>(entity1id, freeIndex);

				//updateInfo.bindingUpdateInfos_.push_back(bindingUpdateInfo);

				renderDriver0->driver_->UpdateResourceSet(updateInfo);

				//Create uniform buffer per material to save textures indices and use it in shader.

				RAL::Driver::UB::CreateInfo materialInfoUBCI{
					.size_ = sizeof(Material::Info),
					.type_ = RAL::Driver::UB::Type::Mutable
				};

				const RAL::Driver::UB::Id materialInfoUBId = renderDriver0->driver_->CreateUniformBuffer(materialInfoUBCI);

				renderDriver0->driver_->FillUniformBuffer(materialInfoUBId, &materialInfo);

				//Metallic maps binding array.
				RAL::Driver::RS::Binding_::Layout materialInfoRSBindingLayout{
					.binding_ = 0,
					.type_ = RAL::Driver::Shader::Binding::Type::UniformBuffer,
					.count_ = 1,
					.stage_ = RAL::Driver::Shader::Stage::FragmentShader,
				};

				RAL::Driver::RS::CI3 materialInfoRSCI{
					.bindings_ = { materialInfoRSBindingLayout }
				};

				//Create material resource set.
				const RAL::Driver::RS::Id rsId = renderDriver0->driver_->CreateResourceSet(materialInfoRSCI);

				RAL::Driver::RS::Binding_::UpdateInfo::UniformBuffer materialInfoRSUBUpdateInfo{
					.ubid_ = materialInfoUBId,
					.offset_ = 0,
					.size_ = sizeof(Material::Info)
				};

				RAL::Driver::RS::Binding_::UpdateInfo materialInfoRSBindingUpdateInfo{
					.binding_ = 0,
					.arrayElement_ = 0,
					.resourcesCount_ = 1,
					.type_ = RAL::Driver::Shader::Binding::Type::UniformBuffer,
					.UBInfo_ = { materialInfoRSUBUpdateInfo }
				};

				RAL::Driver::RS::UpdateInfo materialInfoRSUpdateInfo{
					.id_ = rsId,
					.bindingUpdateInfos_ = { materialInfoRSBindingUpdateInfo }
				};

				renderDriver0->driver_->UpdateResourceSet(materialInfoRSUpdateInfo);
				if (entity1id == 28) {
					Common::BreakPointLine();
				}
				ASSERT(!materialInfoUBId.IsInvalid());
				ASSERT(!rsId.IsInvalid());
				CreateComponent<Material::InfoResourceSet>(entity1id, materialInfoUBId, rsId);

			};

		}

	} // namespace Render

} // namespace OksEngine