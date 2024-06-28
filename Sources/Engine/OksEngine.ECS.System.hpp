
#include <ECS.hpp>
#include <OksEngine.Context.hpp>

namespace OksEngine {

	class ECSSystem : public ECS::System {
	public:
		ECSSystem(Context& context) : 
			context_{ context } { }

		[[nodiscard]]
		Context& GetContext() noexcept {
			return context_;
		}

	private:
		Context& context_;
	};

}