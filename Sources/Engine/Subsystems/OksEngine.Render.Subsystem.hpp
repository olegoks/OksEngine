#pragma once 

#include <OksEngine.Subsystem.hpp>
#include <RE.RenderEngine.hpp>
#include <OksEngine.Resource.Subsystem.hpp>
#include <Geometry.Model.hpp>
#include <OksEngine.UI.Subsystem.hpp>
#include <Systems/OksEngine.Render.System.hpp>
#include <RAL.Light.hpp>
#include <RAL.Camera.hpp>

namespace OksEngine {

	class RenderSubsystem : public Subsystem {
	public:

		struct CreateInfo {
			Context& context_;
		};

		RenderSubsystem(const CreateInfo& createInfo);

		[[nodiscard]]
		Common::Index RenderModel(std::string objName, std::string mtlName, std::string textureName);
		Common::Index RenderModel(std::string objName, std::string mtlName);

		void SetModelMatrix(Common::Index modelIndex, const glm::mat4& modelMatrix) {
			engine_->SetModelMatrix(models_[modelIndex], modelMatrix);
		}

		void SetCamera(const Math::Vector3f& position, const Math::Vector3f& direction);

		virtual void Update() noexcept override;

	private:
		std::vector<RE::RenderEngine::Model> models_;
		std::shared_ptr<RE::RenderEngine> engine_ = nullptr;
	};

}