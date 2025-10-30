#pragma once 

#include <optional>

#include <type_traits>
#include <memory>

#define MAGIC_ENUM_RANGE_MIN 0
#define MAGIC_ENUM_RANGE_MAX 1024
#include <magic_enum/magic_enum.hpp>

#define USE_PIX 1
#include <Windows.h>
#undef max
#undef min
#undef GetMessage
#include <pix3.h>

namespace Common {

	inline bool SetEnvVariable(const char* variableName, const char* value) {
#ifdef _WIN32
		return _putenv_s(variableName, value) == 0;
#else
		return setenv(variableName, value, 1) == 0;
#endif
	}

	inline std::string GetEnvVariable(const char* variableName) {
		char* value = nullptr;
		size_t sz = 0;

#ifdef _MSC_VER
		errno_t err = _dupenv_s(&value, &sz, variableName);
#else
		const char* tmp = getenv(varname);
		if (tmp) {
			value = strdup(tmp);
			sz = strlen(value) + 1;
		}
		else {
			err = ENOENT;
		}
#endif
		if (err != 0 || value == nullptr) {
			return std::string{};
		}
		std::string valueStr{ value };
		free(value);
		return valueStr;
	}
	inline bool UnsetEnvVariable(const char* variableName) {
#ifdef _WIN32
		return _putenv_s(variableName, "") == 0;
#else
		return unsetenv(variableName) == 0;
#endif
	}

	enum class Configuration {
		Debug,
		Release,
		ReleaseWithDebugInfo
	};

	inline constexpr Configuration currentConfiguration = Configuration::Debug;

	template<class Type>
	using Maybe = std::optional<Type>;

	template<class Type>
	concept Enum = std::is_enum<Type>::value && requires(Type value) {
		value = Type::Undefined;
	};

#if defined __MSC__ || defined _MSC_VER
#define NOINLINE __declspec(noinline)
#define FORCE_INLINE __forceinline
#elif __GNUC__
#define NOINLINE __attribute__((noinline))
#define FORCE_INLINE inline
#else
#error "Unsupported compiler!!"
#endif


	void BreakPointLine();

#if !defined(NDEBUG)
#define BRK do{ ::Common::BreakPointLine(); } while(0)
#define BRK_IF(expression)					\
	do {									\
		if (expression) {					\
			::Common::BreakPointLine();		\
		}									\
	} while(0)
#else 
#define BRK  ((void)0)
#define BRK_IF(expression) ((void)0)
#endif



#ifdef _DEBUG
	constexpr static inline Configuration configuration_ = Configuration::Debug;
#else
	constexpr static inline Configuration configuration_ = Configuration::Release;
#endif

	[[nodiscard]]
	constexpr bool IsDebug() {
		return (configuration_ == Configuration::Debug);
	}



	template<class Type>
	constexpr void DiscardUnusedParameter(Type&& value) { value; }


	template<class Callback, class Type>
	void ProcessType(Callback&& callback) {
		callback.template operator() < Type > ();
	}


	template<class Callback, class Type, class ...Types>
	void ProcessType(Callback&& callback) {
		callback.template operator() < Type > ();
		ProcessType<Callback, Types...>(std::forward<Callback>(callback));
	}


	template<class Callback>
	void ProcessType(Callback&&) { }


	template<class Callback, class ... Types>
	void ProcessTemplateList(Callback&& callback) {
		ProcessType<Callback, Types...>(std::forward<Callback>(callback));
	}

	template<typename E>
	struct EnableBitmaskOperators {
		static constexpr bool enable = false;
	};

	template<typename E>
	typename std::enable_if<EnableBitmaskOperators<E>::enable, E>::type
		operator|(E lhs, E rhs) {
		using underlying = typename std::underlying_type<E>::type;
		return static_cast<E>(
			static_cast<underlying>(lhs) |
			static_cast<underlying>(rhs)
			);
	}

	template<typename Type, typename... AllowedTypes>
	constexpr bool IsAnyOf() {
		return std::disjunction_v<std::is_same<Type, AllowedTypes>...>;
		// std::is_same<T, Allowed1>::value || 
		// std::is_same<T, Allowed2>::value || 
		// std::is_same<T, Allowed3>::value || ...
	}

	template <class To, class From>
	[[nodiscard]]
	inline std::shared_ptr<To> pointer_cast(const std::shared_ptr<From>& ptr) noexcept {
#if !defined(NDEBUG)
		return std::dynamic_pointer_cast<To>(ptr);
#else
		return std::static_pointer_cast<To>(ptr);
#endif
	}

	template <class To, class From>
	[[nodiscard]]
	inline std::shared_ptr<To> pointer_cast(std::shared_ptr<From>&& ptr) noexcept {
#if !defined(NDEBUG)
		return std::dynamic_pointer_cast<To>(ptr);
#else
		return std::static_pointer_cast<To>(ptr);
#endif
	}

//#define ENABLE_PROFILE

#if !defined(NDEBUG) || defined(ENABLE_PROFILE)

#define BEGIN_PROFILE(format, ...)\
	PIXBeginEvent(PIX_COLOR(255, 0, 0), format, __VA_ARGS__);

#define END_PROFILE()\
	PIXEndEvent();

#else

#define BEGIN_PROFILE(format, ...)  ((void)0)

#define END_PROFILE() ((void)0)

#endif
}