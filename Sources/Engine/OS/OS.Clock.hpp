#pragma once 

#include <chrono>
 
#include <Common.Types.hpp>
#include <OS.Assert.hpp>

namespace OS {

	using namespace std::chrono;

	class [[nodiscard]] Clock final {
	public:

		class Duration final {
		public:

			enum class Type {
				Nanoseconds,
				Milliseconds,
				Seconds
			};

			constexpr Duration(Common::Size value) noexcept : 
				duration_{ value } {

			}

			Duration(nanoseconds duration = nanoseconds::zero()) noexcept : duration_{ duration } { }

			[[nanoseconds]]
			static Duration Zero() noexcept {
				return Duration{ nanoseconds::zero() };
			}

			[[nodiscard]]
			bool operator>(const Duration& duration) const noexcept {
				return duration_ > duration.duration_;
			}

			[[nodiscard]]
			bool operator==(const Duration& duration) const noexcept {
				return duration_ == duration.duration_;
			}

			[[nodiscard]]
			bool operator>=(const Duration& duration) const noexcept {
				return (*this > duration) || (*this == duration);
			}


			[[nodiscard]]
			Common::Size GetValue(/*Type type = Type::Nanoseconds*/) const noexcept {
				return duration_.count();
				/*switch (type) {
				case Type::Milliseconds: {
					return duration_cast<milliseconds>(duration_).count();
					break;
				}
				case Type::Nanoseconds: {
					return duration_cast<nanoseconds>(duration_).count();
					break;
				}
				case Type::Seconds: {
					return duration_cast<seconds>(duration_).count();
					break;
				}

				default: {
					OS::AssertFailMessage("Attempt to get duration value and cast it to unsupported type.");
					return 0;
				}
				};*/

			}

		private:

			nanoseconds duration_;
		};

		class [[nodiscard]] TimePoint final {
		public:
			TimePoint() noexcept = default;

			TimePoint(time_point<high_resolution_clock>&& timePoint) noexcept :
				timePoint_{ timePoint } { }

			Duration operator-(const TimePoint& timePoint) const noexcept {
				return Duration{ timePoint_ - timePoint.timePoint_ };
			}

			TimePoint operator+(const Duration& duration) noexcept {
				return TimePoint{ timePoint_ + nanoseconds{ duration.GetValue() } };
			}

		private:
			time_point<high_resolution_clock> timePoint_;
		};

		static TimePoint Now() noexcept {
			return TimePoint{ high_resolution_clock::now() };
		}

		Clock() noexcept { }

	private:

	};


	constexpr Clock::Duration operator""_msec(const Common::Size milliseconds) noexcept {
		return Clock::Duration{ milliseconds * 1000000 };
	}

	constexpr Clock::Duration operator""_sec(const Common::Size seconds) noexcept {
		return Clock::Duration{ seconds * 1000000000 };
	}
}