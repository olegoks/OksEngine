#pragma once
#include <Render\auto_OksEngine.Show.hpp>

namespace OksEngine
{
	void ShowRenderAttachment::Update(ECS2::Entity::Id entity0id, RenderDriver* renderDriver0,
		const Render::MultisamplingAttachment* multisamplingAttachment0) {

		renderDriver0->driver_->Show(multisamplingAttachment0->textureId_);

	};

}