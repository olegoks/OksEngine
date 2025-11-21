 #pragma once


namespace ECS2 {


	class IArchetypeContainer {
	public:

		IArchetypeContainer(ComponentTypeId typeId) noexcept : typeId_{ typeId } { }

		virtual void Resize(Common::Size newSize) noexcept = 0;
		[[nodiscard]]
		virtual Common::Size GetSize() const noexcept = 0;
		virtual Common::Size GetReservedSize() const noexcept = 0;
		ComponentTypeId GetTypeId() const noexcept { return typeId_; }

	private:
		ComponentTypeId typeId_;
	};

	using ComponentIndex = Common::Index;

	constexpr static inline ComponentIndex invalidComponentIndex_ = Common::Limits<ComponentIndex>::Max();

	template<class ComponentType>
	class ArchetypeContainer final : public IArchetypeContainer {
	public:

		ArchetypeContainer(Common::Size startSize) noexcept : IArchetypeContainer{ ComponentType::GetTypeId() } {
			
			components_.resize(startSize);
			
			//std::memset(components_.data(), 0, startSize * sizeof(ComponentType));
		}

		void Resize(Common::Size newSize) noexcept {
			const Common::Size oldSize = components_.size();
#pragma region Assert
			ASSERT_FMSG(newSize > oldSize,
				"New size must be more than previous.");
#pragma endregion

			components_.resize(newSize);

			//std::memset(components_.data() + oldSize, 0, (newSize - oldSize) * sizeof(ComponentType));
		}

		[[nodiscard]]
		virtual Common::Size GetSize() const noexcept override {
			NOT_IMPLEMENTED();
			return 0;
		}

		[[nodiscard]]
		virtual Common::Size GetReservedSize() const noexcept override {
			return components_.size();
		}

		[[nodiscard]]
		ComponentType* operator[](ComponentIndex index) noexcept {
#pragma region Assert
			//ASSERT_FMSG(
			//	!IsComponentIndexFree(index),
			//	"Attempt to get component by index but component with this index was removed.");
			//ASSERT_FMSG(
			//	index < components_.size(),
			//	"Attempt to use incorrect component index.");
#pragma endregion
			return &components_[index];
		}
		
		ComponentType* GetComponents() noexcept {
			return components_.data();
		}

		template<class ComponentType, class ...Args>
		void CreateComponent(ComponentIndex componentIndex, Args&& ...args) {

			components_[componentIndex].~ComponentType();
			new(components_.data() + componentIndex) ComponentType(std::forward<Args>(args)...);
		}
		


		[[nodiscard]]
		const ComponentType* operator[](ComponentIndex index) const noexcept {
			return const_cast<ArchetypeContainer<ComponentType>*>(this)->operator[](index);
		}


		[[maybe_unused]]
		[[nodiscard]]
		bool IsComponentIndexValid(ComponentIndex index) const noexcept {
			return index < components_.size();
		}

	private:
		std::vector<ComponentType>	components_;
	};



	class IContainer {
	public:

		IContainer(ComponentTypeId typeId) noexcept : typeId_{ typeId } { }

		virtual void Resize(Common::Size newSize) noexcept = 0;
		virtual Common::Size GetSize() const noexcept = 0;
		ComponentTypeId GetTypeId() const noexcept { return typeId_; }

		virtual void RemoveComponent(Entity::Id entityId) noexcept = 0;
		virtual bool IsComponentExist(ComponentTypeId componentTypeId) const noexcept = 0;

	private:
		ComponentTypeId typeId_;
	};


	template<class ComponentType>
	class Container final : public IContainer {
	public:

		Container() noexcept : IContainer{ ComponentType::GetTypeId() } {
			Resize(10);
			//std::memset(components_.data(), 0, components_.size() * sizeof(ComponentType));
		}

		void Resize(Common::Size newSize) noexcept {
#pragma region Assert
			ASSERT_FMSG(newSize > components_.size(),
				"New size must be more than previous.");
#pragma endregion
			const Common::Size oldSize = components_.size();
			components_.resize(newSize);
			//std::memset(components_.data() + oldSize, 0, (newSize - oldSize) * sizeof(ComponentType));

			componentIndexToEntityId_.resize(newSize, Entity::Id::invalid_);
			for (Common::Index i = oldSize; i < newSize; i++) {
				freeComponentIndices_.insert(i);
			}

		}

		[[nodiscard]]
		virtual Common::Size GetSize() const noexcept override {
			return components_.size();
		}


		[[nodiscard]]
		ComponentType* operator[](ComponentIndex index) noexcept {
#pragma region Assert
			ASSERT_FMSG(
				!freeComponentIndices_.contains(index),
				"Attempt to get component by index but component with this index was removed.");
			ASSERT_FMSG(
				index < components_.size(),
				"Attempt to use incorrect component index.");
#pragma endregion
			return &components_[index];
		}


		using ProcessComponent = std::function<void(ComponentType* component, Entity::Id entityId)>;

		void ForEachComponent(ProcessComponent&& processComponent) {
			for (Common::Index i = 0; i < components_.size(); ++i) {
				const Entity::Id entityId = componentIndexToEntityId_[i];
				if (entityId != Entity::Id::invalid_) {
					processComponent(components_.data() + i, entityId);
				}
			}
		}

		[[nodiscard]]
		const ComponentType* operator[](ComponentIndex index) const noexcept {
			return const_cast<Container<ComponentType>*>(this)->operator[](index);
		}

		template<class ...Args>
		[[nodiscard]]
		void CreateComponent(Entity::Id entityId, Args&&... args) noexcept {
			auto freeComponentIndexIt = freeComponentIndices_.begin();
			if (freeComponentIndexIt == freeComponentIndices_.end()) {
				Resize(GetSize() * 2);
				freeComponentIndexIt = freeComponentIndices_.begin();
			}
			const ComponentIndex freeComponentIndex = *freeComponentIndexIt;
			freeComponentIndices_.erase(freeComponentIndexIt);
			
			components_[freeComponentIndex].~ComponentType();
			new(components_.data() + freeComponentIndex) ComponentType(std::forward<Args>(args)...);
			componentIndexToEntityId_[freeComponentIndex] = entityId;
			entityIdComponentIndex_[entityId] = freeComponentIndex;
		}
		
		[[maybe_unused]]
		[[nodiscard]]
		virtual bool IsComponentExist(ComponentIndex index) const noexcept override {
#pragma region Assert
			ASSERT_FMSG(index < components_.size(),
				"Invalid component index.");
#pragma endregion
			return componentIndexToEntityId_[index] != Entity::Id::invalid_;
		}

		ComponentType* GetComponent(Entity::Id entityId) {
//#pragma region Assert
//			ASSERT_FMSG(entityIdComponentIndex_.contains(entityId), 
//				"Container doesn't contain component of this entity.");
//#pragma endregion
			auto componentIndexIt = entityIdComponentIndex_.find(entityId);
			if (componentIndexIt != entityIdComponentIndex_.end()) {
				return components_.data() + componentIndexIt->second;
			} else {
				return nullptr;
			}
			
		}
		
		virtual void RemoveComponent(Entity::Id entityId) noexcept override {
#pragma region Assert
			ASSERT_FMSG(
				entityIdComponentIndex_.contains(entityId),
				"Attempt to remove component, but this entity doesnt' have component of this type.");
#pragma endregion
			const ComponentIndex componentIndex = entityIdComponentIndex_[entityId];
#pragma region Assert
			ASSERT_FMSG(!freeComponentIndices_.contains(componentIndex), 
				"Attempt to remove component with index that was remove early.");
#pragma endregion
			freeComponentIndices_.insert(componentIndex);
			entityIdComponentIndex_.erase(entityId);
			componentIndexToEntityId_[componentIndex] = Entity::Id::invalid_;
		}

		//[[nodiscard]]
		//static ComponentTypeId GetComponentTypeId() const noexcept {
		//	return ComponentType::GetTypeId();
		//}
	
	private:

		std::set<ComponentIndex> freeComponentIndices_;
		std::map<Entity::Id, ComponentIndex> entityIdComponentIndex_;
		std::vector<Entity::Id> componentIndexToEntityId_; // Vector index is component index.
		std::vector<ComponentType>	components_;
	};

}