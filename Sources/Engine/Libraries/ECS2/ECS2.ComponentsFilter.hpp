#pragma once 

#include <bitset>

#include <Common.hpp>

namespace ECS2 {


	class ComponentsFilter {
	public:

		template<class ...ComponentType>
		void SetBits() {

			(filter_.set(ComponentType::GetTypeId()), ...);

			if constexpr (Common::IsDebug()) {
				(componentNames_.push_back(std::string{ ComponentType::GetName() }), ...);
			}

		}

		template<class ...ComponentType>
		void RemoveBits() {

			(filter_.reset(ComponentType::GetTypeId()), ...);

			if constexpr (Common::IsDebug()) {
				(std::remove(componentNames_.begin(), componentNames_.end(), (std::string{ ComponentType::GetName() })), ...);
			}

		}

		ComponentsFilter() { }


		template<class ...ComponentType>
		bool IsSet() const {
			return (IsSet(ComponentType::GetTypeId()) && ...);
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
				return false;
			}
		};

		[[nodiscard]]
		bool IsSubsetOf(const ComponentsFilter& componentsFilter) const noexcept {
			return (componentsFilter.filter_ & filter_) == filter_;
		}

		[[nodiscard]]
		bool operator==(const ComponentsFilter& componentsFilter) const noexcept {

			if (this == &componentsFilter) {
				return true;
			}

			return (filter_ == componentsFilter.filter_);

		}

		[[nodiscard]]
		bool IsSetOneComponent() const {
			return filter_.count() == 1;
		}

	private:

		constexpr static Common::UInt16 maxComponentsNumber_ = 256;

		std::vector<std::string> componentNames_;

		std::bitset<maxComponentsNumber_> filter_;
	};


}