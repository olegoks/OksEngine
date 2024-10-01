
#include <Behaviour/OksEngine.LuaContext.hpp>

namespace OksEngine {


	LuaContext::LuaContext(Lua::Context&& context) :
		ECSComponent{ nullptr }, context_{ std::move(context) } { }

}