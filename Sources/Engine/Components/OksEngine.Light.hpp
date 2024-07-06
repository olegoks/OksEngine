
#include <Math.Common.hpp>
#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	template<class LightType>
	class Light : public ECSComponent<LightType> {
	public:

		Light(
			const glm::vec3& color,
			float intensity) : 
			ECSComponent<LightType>{ nullptr },
			color_{ color },
			intensity_{ intensity } { }

		[[nodiscard]]
		const glm::vec3& GetColor() const noexcept {
			return color_;
		}

		[[nodiscard]]
		float GetIntensity() const noexcept {
			return intensity_;
		}

	private:
		glm::vec3 color_{ 1.f, 1.f, 1.f };
		float intensity_{ 1.f }; //0.0f - 1.0f
	};

	class PointLight : public Light<PointLight> {
	public:

		PointLight(
			const glm::vec3& color,
			float intensity,
			float radius) :
			Light{ color, intensity },
			radius_{ radius } {  }

		[[nodiscard]]
		float GetRadius() const noexcept { 
			return radius_;
		}

	private:
		float radius_; // Border of light, on the border light intensity is 0.f.
	};

}