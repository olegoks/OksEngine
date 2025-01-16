#pragma once


namespace ECS2 {


	class IContainer {
	public:

		IContainer(ComponentTypeId typeId) noexcept : typeId_{ typeId } { }

		virtual std::size_t GetSize() const noexcept = 0;
		ComponentTypeId GetTypeId() const noexcept { return typeId_; }

		virtual void ProcessDelayedRequests() = 0;

	private:
		ComponentTypeId typeId_;
	};
	using ComponentIndex = std::size_t;

	template<class ComponentType>
	class Container final : public IContainer {
	public:

		Container() noexcept : IContainer{ ComponentType::GetTypeId() } { }

		[[nodiscard]]
		virtual Common::Size GetSize() const noexcept override {
			return components_.size();
		}

		[[nodiscard]]
		ComponentType* operator[](ComponentIndex index) noexcept {
			OS::AssertMessage(
				!IsComponentIndexFree(index),
				"Attempt to get component by index but component with this index was removed.");
			OS::AssertMessage(
				index < components_.size(),
				"Attempt to use incorrect component index.");
			return &components_[index];
		}

		[[nodiscard]]
		const ComponentType* operator[](ComponentIndex index) const noexcept {
			return const_cast<Container<ComponentType>*>(this)->operator[](index);
		}

		template<class ...Args>
		[[nodiscard]]
		ComponentIndex CreateComponent(Args&&... args) noexcept {
			delayedComponents_.emplace_back(std::forward<Args>(args)...);
			return components_.size() + delayedComponents_.size() - 1; // To get index that will be after adding delayed components.
		}

		[[nodiscard]]
		bool IsComponentExist(ComponentIndex index) const noexcept {
			return (index < components_.size() && !IsComponentIndexFree(index));
		}

		[[nodiscard]]
		bool IsComponentIndexValid(ComponentIndex index) const noexcept {
			return (index < components_.size() || IsComponentIndexFree(index));
		}

		void RemoveComponent(ComponentIndex index) noexcept {
			OS::AssertMessage(
				IsComponentIndexValid(index),
				"Attempt to remove components with incorrect index");
			OS::AssertMessage(
				std::ranges::find(delayedRemovedComponents_, index) == delayedRemovedComponents_.end(),
				"Attempt to remove the component twice.");
			delayedRemovedComponents_.push_back(index);
		}

		ComponentType* Get() {
			return components_.data();
		}

		virtual void ProcessDelayedRequests() override {
			components_.insert(
				components_.end(),
				std::make_move_iterator(delayedComponents_.begin()),
				std::make_move_iterator(delayedComponents_.end())
			);
			delayedComponents_.clear();

			for (ComponentIndex index : delayedRemovedComponents_) {
				ComponentType* component = this->operator[](index);
				component->~ComponentType();
				removedComponents_.push_back(index);
			}
			delayedRemovedComponents_.clear();
		}

	private:

		[[nodiscard]]
		bool IsComponentIndexFree(ComponentIndex index) const noexcept {
			return std::find(removedComponents_.cbegin(), removedComponents_.cend(), index) != removedComponents_.cend();
		}

	private:

		std::vector<ComponentIndex> delayedRemovedComponents_;
		std::vector<ComponentIndex> removedComponents_;

		std::vector<ComponentType>	delayedComponents_;
		std::vector<ComponentType>	components_;
	};

}