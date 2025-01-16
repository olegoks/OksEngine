#pragma once 

#include <bitset>

namespace ECS2 {


	class ComponentsFilter {
	public:

		template<class ...ComponentType>
		void SetBits() {

			auto callback = [&] <class ComponentType>() {
				filter_.set(ComponentType::GetTypeId());
			};

			(callback.template operator() < ComponentType > (), ...);

		}

		ComponentsFilter() { }

		template<class ComponentType>
		[[nodiscard]]
		bool IsSet() const {
			return IsSet(ComponentType::GetTypeId());
		}

		[[nodiscard]]
		bool IsSet(ComponentTypeId componentTypeId) const {
			return filter_.test(componentTypeId);
		}

		[[nodiscard]]
		constexpr static Common::UInt16 GetMaxComponentsNumber() noexcept {
			return maxComponentsNumber_;
		}

		struct LessComparator {
			bool operator()(const ComponentsFilter& first, const ComponentsFilter& second) const {
				for (Common::Index i = 0; i < ComponentsFilter::GetMaxComponentsNumber(); i++) {
					if (first.IsSet(i) && !second.IsSet(i)) {
						return false;
					}
					else if (!first.IsSet(i) && second.IsSet(i)) {
						return true;
					}
				}
			}
		};

	private:

		constexpr static Common::UInt16 maxComponentsNumber_ = 256;

		std::bitset<maxComponentsNumber_> filter_;
	};


}