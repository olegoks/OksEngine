
#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct API : public ECSComponent<API> {

		API(std::shared_ptr<API> api) : ECSComponent{ nullptr }, api_{ api } {}


		std::shared_ptr<API> api_ = nullptr;
 	};

}