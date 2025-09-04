#pragma once
#include <Render\auto_OksEngine.Show.hpp>

namespace OksEngine
{
	void ShowRenderAttachment::Update(ECS2::Entity::Id entity0id, RenderDriver* renderDriver0,
		const RenderAttachment* renderAttachment0) {

		renderDriver0->driver_->Show(renderAttachment0->textureId_);

	};

}