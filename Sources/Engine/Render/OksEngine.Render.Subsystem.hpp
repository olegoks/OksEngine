#pragma once 

#include <Common/OksEngine.Subsystem.hpp>
#include <RE.RenderEngine.hpp>
#include <Resources/OksEngine.Resource.Subsystem.hpp>
#include <Geometry.Model.hpp>
#include <UI/OksEngine.UI.Subsystem.hpp>
#include <Render/OksEngine.Render.System.hpp>
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
		Common::Index RenderModel(std::string objName, std::string mtlName, const std::vector<std::string>& textureName);
		Common::Index RenderModel(std::string objName, std::string mtlName);
		[[nodiscard]]
		Common::Index RenderModelTextures(std::string objName, std::string mtlName);
		[[nodiscard]]
		Common::Index RenderAnimationModel(const std::vector<std::string>& filesNames);
		void SetModelMatrix(Common::Index modelIndex, const glm::mat4& modelMatrix) {
			engine_->SetModelMatrix(models_[modelIndex], modelMatrix);
		}

		void SetCamera(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up);

		virtual void Update() noexcept override;

		void UpdateCamera(Camera* camera);
		void UpdateImmutableRenderGeometry(ImmutableRenderGeometry* immutableRenderGeometry);

	private:
		std::vector<RE::RenderEngine::Model> models_;
		std::shared_ptr<RE::RenderEngine> engine_ = nullptr;
	};

}