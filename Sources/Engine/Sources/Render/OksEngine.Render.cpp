#pragma once
#include <Render\auto_OksEngine.Render.hpp>

#include <Common/auto_OksEngine.Position3D.hpp>
#include <Common/auto_OksEngine.Rotation3D.hpp>
#include <Render/auto_OksEngine.Model.hpp>
#include <Render/auto_OksEngine.Model.Animation.hpp>

namespace OksEngine
{

	void CreateRenderDriver::Update(
		ECS2::Entity::Id entity0id,
		const MainWindow* mainWindow0,

		ECS2::Entity::Id entity1id,
		const Config* config1,
		const LuaScript* luaScript1,

		ECS2::Entity::Id entity2id,
		ResourceSystem* resourceSystem2) {

		auto api = RAL::CreateAPI();

		RAL::Driver::CreateInfo driverCreateInfo{};

		{
			auto windowInfo = mainWindow0->window_->GetInfo(UIAL::Render::Vulkan);
			RAL::RenderSurface renderSurface;
			if (windowInfo.subsystem_ == UIAL::Subsystem::GLFW) {

				renderSurface.param1_ = windowInfo.param1_;
				renderSurface.param2_ = windowInfo.param2_;
				renderSurface.param3_ = windowInfo.param3_;
				renderSurface.size_ = windowInfo.size_;
				renderSurface.uiSubsystem_ = RAL::UISubsystem::GLFW;
			}

			auto renderSurfacePtr = std::make_shared<RAL::RenderSurface>(renderSurface);

			driverCreateInfo.surface_ = *renderSurfacePtr;
		}

		auto driver = api->CreateDriver(driverCreateInfo);

		const ECS2::Entity::Id driverEntityId = CreateEntity();
		CreateComponent<RenderDriver>(driverEntityId, driver);

		//Animation
		{
			Common::Size preallocatedEntitiesNumber = 65'536;

			//Create storage buffer for nodes LOCAL POSITIONS.
			RAL::Driver::ResourceSet::Id localPositionsSBResId = RAL::Driver::ResourceSet::Id::Invalid();
			RAL::Driver::StorageBuffer::Id localPositionsSBId = RAL::Driver::ResourceSet::Id::Invalid();
			{
				RAL::Driver::StorageBuffer::CreateInfo localPositionsSBCI{
					.size_ = preallocatedEntitiesNumber * sizeof(LocalPosition3D)
				};

				localPositionsSBId = driver->CreateStorageBuffer(localPositionsSBCI);

				CreateComponent<Animation::DriverLocalPosition3DComponents>(driverEntityId, localPositionsSBId);
				

				RAL::Driver::ResourceSet::Binding localPositionsStorageBinding
				{
					.stage_ = RAL::Driver::Shader::Stage::ComputeShader,
					.binding_ = 0,
					.sbid_ = localPositionsSBId
				};
				localPositionsSBResId = driver->CreateResource(localPositionsStorageBinding);
				CreateComponent<Animation::LocalPosition3DComponentsResource>(driverEntityId, localPositionsSBResId);
			}

			//Create storage buffer for nodes LOCAL ROTATIONS.
			RAL::Driver::ResourceSet::Id localRotationsSBResId = RAL::Driver::ResourceSet::Id::Invalid();
			RAL::Driver::StorageBuffer::Id localRotationsSBId = RAL::Driver::ResourceSet::Id::Invalid();
			{
				RAL::Driver::StorageBuffer::CreateInfo localRotationsSBCI{
					.size_ = preallocatedEntitiesNumber * sizeof(LocalRotation3D)
				};

				localRotationsSBId = driver->CreateStorageBuffer(localRotationsSBCI);
				CreateComponent<Animation::DriverLocalRotation3DComponents>(driverEntityId, localRotationsSBId);

				RAL::Driver::ResourceSet::Binding localRotationsStorageBinding
				{
					.stage_ = RAL::Driver::Shader::Stage::ComputeShader,
					.binding_ = 0,
					.sbid_ = localRotationsSBId
				};
				localRotationsSBResId = driver->CreateResource(localRotationsStorageBinding);

				CreateComponent<Animation::LocalRotation3DComponentsResource>(driverEntityId, localRotationsSBResId);
			}


			//Create storage buffer for node ANIMATION STATES.
			RAL::Driver::ResourceSet::Id nodeAnimationStatesSBResId = RAL::Driver::ResourceSet::Id::Invalid();
			{
				RAL::Driver::StorageBuffer::CreateInfo nodeAnimationStatesSBCI{
					.size_ = preallocatedEntitiesNumber * sizeof(Animation::Mdl::Node::RunningState)
				};

				const RAL::Driver::StorageBuffer::Id nodeAnimationStatesSBId = driver->CreateStorageBuffer(nodeAnimationStatesSBCI);
				CreateComponent<Animation::Mdl::Node::DriverRunningStates>(driverEntityId, nodeAnimationStatesSBId);

				RAL::Driver::ResourceSet::Binding nodeAnimationStatesStorageBinding
				{
					.stage_ = RAL::Driver::Shader::Stage::ComputeShader,
					.binding_ = 0,
					.sbid_ = nodeAnimationStatesSBId
				};
				nodeAnimationStatesSBResId = driver->CreateResource(nodeAnimationStatesStorageBinding);
				CreateComponent<Animation::Mdl::Node::RunningStatesResource>(driverEntityId, nodeAnimationStatesSBResId);
			}

			//Create storage buffer for node ANIMATION DATA.
			RAL::Driver::ResourceSet::Id nodeAnimationsSBResId = RAL::Driver::ResourceSet::Id::Invalid();
			{

				RAL::Driver::StorageBuffer::CreateInfo nodeAnimationsSBCI{
					.size_ = preallocatedEntitiesNumber * sizeof(Animation::Mdl::Node::Animations)
				};
				const RAL::Driver::StorageBuffer::Id nodeAnimationsSBId = driver->CreateStorageBuffer(nodeAnimationsSBCI);

				CreateComponent<Animation::Mdl::Node::DriverAnimationsComponents>(driverEntityId, nodeAnimationsSBId);

				RAL::Driver::ResourceSet::Binding nodeAnimationsStorageBinding
				{
					.stage_ = RAL::Driver::Shader::Stage::ComputeShader,
					.binding_ = 0,
					.sbid_ = nodeAnimationsSBId
				};
				nodeAnimationsSBResId = driver->CreateResource(nodeAnimationsStorageBinding);
				CreateComponent<Animation::Mdl::Node::AnimationsComponentsResource>(driverEntityId, nodeAnimationsSBResId);
			}
			//Create storage buffer for WORLD positions.
			{

				RAL::Driver::StorageBuffer::CreateInfo worldPositionsSBCI{
					.size_ = preallocatedEntitiesNumber * sizeof(WorldPosition3D)
				};
				const RAL::Driver::StorageBuffer::Id worldPositionsSBId = driver->CreateStorageBuffer(worldPositionsSBCI);

				RAL::Driver::ResourceSet::Binding worldPositionsStorageBinding
				{
					.stage_ = RAL::Driver::Shader::Stage::VertexShader,
					.binding_ = 0,
					.sbid_ = worldPositionsSBId
				};
				RAL::Driver::ResourceSet::Id worldPositionsSBResId = driver->CreateResource(worldPositionsStorageBinding);
				CreateComponent<GPGPUECS::StorageBuffer::WorldPositions3D>(driverEntityId, worldPositionsSBId, worldPositionsSBResId);
			}
			{

				RAL::Driver::StorageBuffer::CreateInfo worldPositionsSBCI{
					.size_ = preallocatedEntitiesNumber * sizeof(WorldRotation3D)
				};
				const RAL::Driver::StorageBuffer::Id worldPositionsSBId = driver->CreateStorageBuffer(worldPositionsSBCI);

				RAL::Driver::ResourceSet::Binding worldPositionsStorageBinding
				{
					.stage_ = RAL::Driver::Shader::Stage::VertexShader,
					.binding_ = 0,
					.sbid_ = worldPositionsSBId
				};
				RAL::Driver::ResourceSet::Id worldPositionsSBResId = driver->CreateResource(worldPositionsStorageBinding);
				CreateComponent<GPGPUECS::StorageBuffer::WorldRotations3D>(driverEntityId, worldPositionsSBId, worldPositionsSBResId);
			}
			{

				RAL::Driver::StorageBuffer::CreateInfo worldPositionsSBCI{
					.size_ = preallocatedEntitiesNumber * sizeof(WorldScale3D)
				};
				const RAL::Driver::StorageBuffer::Id worldPositionsSBId = driver->CreateStorageBuffer(worldPositionsSBCI);

				RAL::Driver::ResourceSet::Binding worldPositionsStorageBinding
				{
					.stage_ = RAL::Driver::Shader::Stage::VertexShader,
					.binding_ = 0,
					.sbid_ = worldPositionsSBId
				};
				RAL::Driver::ResourceSet::Id worldPositionsSBResId = driver->CreateResource(worldPositionsStorageBinding);
				CreateComponent<GPGPUECS::StorageBuffer::WorldScales3D>(driverEntityId, worldPositionsSBId, worldPositionsSBResId);
			}

			{

				RAL::Driver::StorageBuffer::CreateInfo worldPositionsSBCI{
					.size_ = preallocatedEntitiesNumber * sizeof(BoneInverseBindPoseMatrix)
				};
				const RAL::Driver::StorageBuffer::Id worldPositionsSBId = driver->CreateStorageBuffer(worldPositionsSBCI);

				RAL::Driver::ResourceSet::Binding worldPositionsStorageBinding
				{
					.stage_ = RAL::Driver::Shader::Stage::VertexShader,
					.binding_ = 0,
					.sbid_ = worldPositionsSBId
				};
				RAL::Driver::ResourceSet::Id worldPositionsSBResId = driver->CreateResource(worldPositionsStorageBinding);
				CreateComponent<GPGPUECS::StorageBuffer::BoneInverseBindPoseMatrices>(driverEntityId, worldPositionsSBId, worldPositionsSBResId);
			}


			{

				RAL::Driver::StorageBuffer::CreateInfo worldPositionsSBCI{
					.size_ = preallocatedEntitiesNumber * sizeof(Render::Mdl::ModelNodeEntityIds)
				};
				const RAL::Driver::StorageBuffer::Id worldPositionsSBId = driver->CreateStorageBuffer(worldPositionsSBCI);

				RAL::Driver::ResourceSet::Binding worldPositionsStorageBinding
				{
					.stage_ = RAL::Driver::Shader::Stage::VertexShader,
					.binding_ = 0,
					.sbid_ = worldPositionsSBId
				};
				RAL::Driver::ResourceSet::Id worldPositionsSBResId = driver->CreateResource(worldPositionsStorageBinding);
				CreateComponent<GPGPUECS::StorageBuffer::ModelNodeEntityIds>(driverEntityId, worldPositionsSBId, worldPositionsSBResId);
			}

			{

				RAL::Driver::StorageBuffer::CreateInfo worldPositionsSBCI{
					.size_ = preallocatedEntitiesNumber * sizeof(Render::Mdl::BoneNodeEntities)
				};
				const RAL::Driver::StorageBuffer::Id worldPositionsSBId = driver->CreateStorageBuffer(worldPositionsSBCI);

				RAL::Driver::ResourceSet::Binding worldPositionsStorageBinding
				{
					.stage_ = RAL::Driver::Shader::Stage::VertexShader,
					.binding_ = 0,
					.sbid_ = worldPositionsSBId
				};
				RAL::Driver::ResourceSet::Id worldPositionsSBResId = driver->CreateResource(worldPositionsStorageBinding);
				CreateComponent<GPGPUECS::StorageBuffer::BoneNodeEntities>(driverEntityId, worldPositionsSBId, worldPositionsSBResId);
			}

			{
				RAL::Driver::StorageBuffer::CreateInfo SBCI{
					.size_ = preallocatedEntitiesNumber * sizeof(ECS2::Entity::Id)
				};
				const RAL::Driver::StorageBuffer::Id SBId = driver->CreateStorageBuffer(SBCI);

				RAL::Driver::ResourceSet::Binding storageBinding
				{
					.stage_ = RAL::Driver::Shader::Stage::VertexShader,
					.binding_ = 0,
					.sbid_ = SBId
				};
				RAL::Driver::ResourceSet::Id SBResId = driver->CreateResource(storageBinding);

				CreateComponent<GPGPUECS::StorageBuffer::NodeDataEntityIdsToComponentIndices>(driverEntityId, SBId, SBResId);
			}
			{
				RAL::Driver::StorageBuffer::CreateInfo SBCI{
					.size_ = preallocatedEntitiesNumber * sizeof(Render::Mdl::ModelNodeDataEntityId)
				};
				const RAL::Driver::StorageBuffer::Id SBId = driver->CreateStorageBuffer(SBCI);

				RAL::Driver::ResourceSet::Binding storageBinding
				{
					.stage_ = RAL::Driver::Shader::Stage::VertexShader,
					.binding_ = 0,
					.sbid_ = SBId
				};
				RAL::Driver::ResourceSet::Id SBResId = driver->CreateResource(storageBinding);

				CreateComponent<GPGPUECS::StorageBuffer::ModelNodeDataEntityIds>(driverEntityId, SBId, SBResId);
			}

			{
				RAL::Driver::StorageBuffer::CreateInfo SBCI{
					.size_ = preallocatedEntitiesNumber * sizeof(ECS2::Entity::Id)
				};
				const RAL::Driver::StorageBuffer::Id SBId = driver->CreateStorageBuffer(SBCI);

				RAL::Driver::ResourceSet::Binding storageBinding
				{
					.stage_ = RAL::Driver::Shader::Stage::ComputeShader,
					.binding_ = 0,
					.sbid_ = SBId
				};
				RAL::Driver::ResourceSet::Id SBResId = driver->CreateResource(storageBinding);

				CreateComponent<Animation::NodeDataEntityIdsToComponentIndices>(driverEntityId, SBId, SBResId);
			}
			{
				RAL::Driver::StorageBuffer::CreateInfo SBCI{
					.size_ = preallocatedEntitiesNumber * sizeof(Render::Mdl::ModelNodeDataEntityId)
				};
				const RAL::Driver::StorageBuffer::Id SBId = driver->CreateStorageBuffer(SBCI);

				RAL::Driver::ResourceSet::Binding storageBinding
				{
					.stage_ = RAL::Driver::Shader::Stage::ComputeShader,
					.binding_ = 0,
					.sbid_ = SBId
				};
				RAL::Driver::ResourceSet::Id SBResId = driver->CreateResource(storageBinding);

				CreateComponent<Animation::ModelNodeDataEntityIds>(driverEntityId, SBId, SBResId);
			}

			{
				RAL::Driver::StorageBuffer::CreateInfo SBCI{
					.size_ = preallocatedEntitiesNumber * sizeof(ECS2::Entity::Id)
				};
				const RAL::Driver::StorageBuffer::Id SBId = driver->CreateStorageBuffer(SBCI);

				RAL::Driver::ResourceSet::Binding storageBinding
				{
					.stage_ = RAL::Driver::Shader::Stage::VertexShader,
					.binding_ = 0,
					.sbid_ = SBId
				};
				RAL::Driver::ResourceSet::Id SBResId = driver->CreateResource(storageBinding);

				CreateComponent<GPGPUECS::StorageBuffer::NodeEntityIdsToComponentIndices>(driverEntityId, SBId, SBResId);
			}

			{
				RAL::Driver::StorageBuffer::CreateInfo SBCI{
					.size_ = preallocatedEntitiesNumber * sizeof(ECS2::Entity::Id)
				};
				const RAL::Driver::StorageBuffer::Id SBId = driver->CreateStorageBuffer(SBCI);

				RAL::Driver::ResourceSet::Binding storageBinding
				{
					.stage_ = RAL::Driver::Shader::Stage::VertexShader,
					.binding_ = 0,
					.sbid_ = SBId
				};
				RAL::Driver::ResourceSet::Id SBResId = driver->CreateResource(storageBinding);

				CreateComponent<GPGPUECS::StorageBuffer::ModelEntityIdsToComponentIndices>(driverEntityId, SBId, SBResId);
			}

			{
				RAL::Driver::StorageBuffer::CreateInfo SBCI{
					.size_ = preallocatedEntitiesNumber * sizeof(Render::Mdl::ModelEntityIds)
				};
				const RAL::Driver::StorageBuffer::Id SBId = driver->CreateStorageBuffer(SBCI);

				RAL::Driver::ResourceSet::Binding storageBinding
				{
					.stage_ = RAL::Driver::Shader::Stage::VertexShader,
					.binding_ = 0,
					.sbid_ = SBId
				};
				RAL::Driver::ResourceSet::Id SBResId = driver->CreateResource(storageBinding);

				CreateComponent<GPGPUECS::StorageBuffer::ModelEntityIds>(driverEntityId, SBId, SBResId);
			}

			{
				RAL::Driver::StorageBuffer::CreateInfo SBCI{
					.size_ = preallocatedEntitiesNumber * sizeof(Render::Mdl::ModelNodeEntityIndices)
				};
				const RAL::Driver::StorageBuffer::Id SBId = driver->CreateStorageBuffer(SBCI);

				RAL::Driver::ResourceSet::Binding storageBinding
				{
					.stage_ = RAL::Driver::Shader::Stage::VertexShader,
					.binding_ = 0,
					.sbid_ = SBId
				};
				RAL::Driver::ResourceSet::Id SBResId = driver->CreateResource(storageBinding);

				CreateComponent<GPGPUECS::StorageBuffer::MeshNodeEntityIndices>(driverEntityId, SBId, SBResId);
			}

		}
	};


	void StartRender::Update(ECS2::Entity::Id entity0id, RenderDriver* renderDriver0) {

		renderDriver0->driver_->StartRender();

	};


	void EndRender::Update(ECS2::Entity::Id entity0id, RenderDriver* renderDriver0) {

		renderDriver0->driver_->EndRender();

	};

	void ResizeFrameBuffers::Update(
		ECS2::Entity::Id entity0id,
		const MainWindow* mainWindow0,
		const MainWindowResizeEvent* frameBufferResizeEvents0,

		ECS2::Entity::Id entity1id,
		const RenderDriver* renderDriver1) {

		if (!frameBufferResizeEvents0->events_.empty()) {
			auto event = frameBufferResizeEvents0->events_.back();
			renderDriver1->driver_->FrameBufferResize({ event.newWidth_, event.newHeight_ });
		}

	};

} // namespace OksEngine