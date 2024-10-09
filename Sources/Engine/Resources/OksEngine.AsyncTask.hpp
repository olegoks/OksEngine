#pragma once 

#include <OksEngine.ECS.Component.hpp>

#include <Resources/OksEngine.Resource.Subsystem.hpp>

namespace OksEngine {

	class AsyncTask : public ECSComponent<AsyncTask> {
	public:

		AsyncTask(AsyncResourceSubsystem::Task::Id id) :
			ECSComponent{ nullptr },
			id_{ id } { }

		AsyncResourceSubsystem::Task::Id id_;
	};


	template<>
	inline void Edit<AsyncTask>(AsyncTask* asyncTask) {
		ImGui::TextDisabled("Async Task id: %d", asyncTask->id_);
	}

}