#pragma once

#include <Common/Async/auto_OksEngine.Async.hpp>

#define COMMON__ASYNC__CREATE_TASK(lamda)													\
	[this](std::function<std::any(void)>&& taskLambda){										\
		const ECS2::Entity::Id taskEntityId = CreateEntity();								\
		CreateComponent<Async::Manager::Task::Tag>(taskEntityId);							\
		std::future<std::any> future = std::async(std::launch::async, taskLambda);			\
		CreateComponent<Async::Manager::Task::Info>(taskEntityId, std::move(future));		\
		CreateComponent<Async::Manager::Task::State::Pending>(taskEntityId);				\
		return taskEntityId;																\
	}(lamda);