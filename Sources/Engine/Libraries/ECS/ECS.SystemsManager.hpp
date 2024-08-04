#pragma once

#include <vector>
#include <memory>
#include <functional>

#include <ECS.System.hpp>

namespace ECS {

	class SystemsManager {
	public:

		template<class SystemType, class ... Args>
		std::shared_ptr<SystemType> RegisterSystem(Args&& ...args) noexcept {

			auto systemPointer = std::make_shared<SystemType>(std::forward<Args>(args)...);
			systems_.push_back(systemPointer);
			return systemPointer;
		}

		//Return value(bool) - Do continue loop?
		using ProcessSystem = std::function<bool(std::shared_ptr<System> system)>;

		void ForEachSystem(ProcessSystem&& processSystem) {
			for (std::shared_ptr<System> system : systems_) {
				const bool exit = !processSystem(system);
			}
		}

		template<class SystemType>
		std::shared_ptr<System> GetSystem() noexcept {
			std::shared_ptr<System> foundSystem = nullptr;
			Common::TypeId searchSystemTypeId = Common::TypeInfo<SystemType>().GetId();
			ForEachSystem([&foundSystem, searchSystemTypeId](std::shared_ptr<System> system) {
				if (system->GetTypeId() == searchSystemTypeId) {
					foundSystem = system;
					return false;
				}
				return true;
				});
			return foundSystem;
		}

		[[nodiscard]]
		SystemsManager& operator=(const SystemsManager& copySystemsManager) const noexcept {
			OS::NotImplemented();
		}

	private:
		std::vector<std::shared_ptr<System>> systems_;
	};

}