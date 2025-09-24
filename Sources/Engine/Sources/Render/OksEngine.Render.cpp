#pragma once
#include <Render\auto_OksEngine.Render.hpp>

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
		{



		}

		const ECS2::Entity::Id driverEntityId = CreateEntity();
		CreateComponent<RenderDriver>(driverEntityId, driver);

		RAL::Driver::StorageBuffer::CreateInfo sbci{
			.size_ = 1024
		};

		const RAL::Driver::StorageBuffer::Id sbid = driver->CreateStorageBuffer(sbci);

		RAL::Driver::ResourceSet::Binding storageBinding
		{
			.stage_ = RAL::Driver::Shader::Stage::ComputeShader,
			.binding_ = 0,
			.sbid_ = sbid
		};
		const RAL::Driver::ResourceSet::Id sbrid = driver->CreateResource(storageBinding);

		CreateComponent<Render::StorageBufferResource>(driverEntityId, sbrid);
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