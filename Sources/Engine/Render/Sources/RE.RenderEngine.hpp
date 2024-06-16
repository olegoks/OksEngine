
#include <Geometry.Model.hpp>
#include <RAL.API.hpp>

namespace RE {

	class Model : protected Geometry::Model {
	public:
	private:
		
	};

	
	class [[nodiscard]] RenderEngine {
	public:

		struct CreateInfo {

		};

		RenderEngine() {

		}

	private:
		std::shared_ptr<RAL::API> api_ = nullptr;
		std::shared_ptr<RAL::Driver> driver_ = nullptr;
	};

}