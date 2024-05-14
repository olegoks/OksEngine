#pragma once

#include <vector>
#include <memory>
#include <functional>

#include <ECS.System.hpp>

namespace ECS {

	class SystemsManager {
	public:

		template<class SystemType>
		void RegisterSystem() noexcept {

			auto systemPointer = std::make_shared<SystemType>();
			systems_.push_back(systemPointer);

		}

		using ProcessSystem = std::function<void(std::shared_ptr<System> system)>;
		void ForEachSystem(ProcessSystem&& processSystem) {
			for (std::shared_ptr<System> system : systems_) {
				processSystem(system);
			}
		}

	private:
		std::vector<std::shared_ptr<System>> systems_;
	};

}