#pragma once 

#include <Common.Identifier.hpp>

namespace OksEngine {


	template<class Type>
	class [[nodiscard]] Storage {
	public:

		Storage() { }

		[[nodiscard]]
		Type::Id Add(Type&& object) {
			const Type::Id newId = idGenerator_.Generate();
			objects_.emplace(newId, std::move(object));
			return newId;
		}

		[[nodiscard]]
		bool IsExist(Type::Id objectId) {
			return objects_.find(objectId) != objects_.end();
		}

		[[nodiscard]]
		Type& Get(Type::Id objectId) {
			OS::AssertMessage(IsExist(objectId), "Object is not exist.");
			return objects_.find(objectId)->second;
		}

	private:
		Common::IdGenerator idGenerator_;
		std::map<Common::Id, Type> objects_;
	};

	template<class Type>
	class [[nodiscard]] TaggedStorage : public Storage<Type> {
	public:

		TaggedStorage() { }

		[[nodiscard]]
		Type::Id Add(const std::string& tag, Type&& object) {
			const Type::Id objectId = Storage<Type>::Add(std::move(object));
			tagId_[tag] = objectId;
			return objectId;
		}

		[[nodiscard]]
		Type& Get(Type::Id objectId) {
			OS::AssertMessage(Storage<Type>::IsExist(objectId), "Invalid object id.");
			return Storage<Type>::Get(objectId);
		}

		[[nodiscard]]
		Type& Get(const std::string& tag) {
			OS::AssertMessage(tagId_.find(tag) != tagId_.end(), "Invalid object id.");
			const Type::Id objectId = tagId_[tag];
			return Storage<Type>::Get(objectId);
		}

		[[nodiscard]]
		Common::Id GetIdByTag(const std::string& tag) {
			return tagId_[tag];
		}

		[[nodiscard]]
		bool IsExist(const std::string& tag) {
			return tagId_.find(tag) != tagId_.end();
		}

		std::map<std::string, Common::Id> tagId_;
	};


}