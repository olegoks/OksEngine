#pragma once 

#include <bitset>

#include <Common.hpp>

//#define DEBUG_ECS

#if !defined(NDEBUG)

#define ENABLE_ECS_DEBUG

#endif

namespace ECS2 {



	class ComponentsFilter {
	public:


		constexpr static Common::UInt16 maxComponentsNumber_ = 256;

		void SetAllBits() {
			filter_.set();
		}

		template<class ...ComponentType>
		void SetBits() {

			(filter_.set(ComponentType::GetTypeId()), ...);

#ifdef ENABLE_ECS_DEBUG
			if constexpr (Common::IsDebug()) {
				(componentNames_.push_back(std::string{ ComponentType::GetName() }), ...);
			}
#endif

		}

		template<class ...ComponentType>
		void RemoveBits() {

			(filter_.reset(ComponentType::GetTypeId()), ...);

#ifdef ENABLE_ECS_DEBUG
			if constexpr (Common::IsDebug()) {
				([&] {
					std::string name = ComponentType::GetName();
					componentNames_.erase(
						std::remove(componentNames_.begin(), componentNames_.end(), name),
						componentNames_.end()
					);
					}(), ...);
			}
#endif

		}

		void RemoveBit(ComponentTypeId componentTypeId) {
			filter_.reset(componentTypeId);
		}

		void SetBit(ComponentTypeId componentTypeId) {
			filter_.set(componentTypeId);
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


		template<class ...ComponentType>
		bool IsNotSet() const {
			return (!IsSet(ComponentType::GetTypeId()) && ...);
		}

		[[nodiscard]]
		bool IsNotSet(const ComponentsFilter& componentsFilter) const noexcept {
			return ((filter_ & componentsFilter.filter_).none());
		}

		using ProcessSetComponent = std::function<void(ComponentTypeId)>;

		void ForEachSetComponent(ProcessSetComponent&& processSetComponent) const {
			for (Common::Index i = 0; i < maxComponentsNumber_; i++) {
				if (IsSet(i)) {
					processSetComponent(i);
				}
			}
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

		[[nodiscard]]
		bool IsSetOneMoreComponent() const {
			return filter_.count() >= 1;
		}

	private:

#ifdef ENABLE_ECS_DEBUG
		std::vector<std::string> componentNames_;
#endif

		std::bitset<maxComponentsNumber_> filter_;
	};


}