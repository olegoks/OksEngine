#pragma once

#include <string_view>
#include <source_location>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <optional>
#include <filesystem>

#include <Common.StringView.hpp>
#include <Common.Format.hpp>
#include <Common.Types.hpp>

#include <OS.Assert.hpp>

namespace OS {

	class Logger {
	public:

		class Severity {
		public:
			enum Type {
				Error,
				Warning,
				Info
			};

			Severity(Severity::Type severity) : type_{ severity } { }

			operator const char* () const noexcept {
				switch(type_) {
				case Error: { return "Error"; }
				case Warning: {  return "Warning"; }
				case Info: {  return "Info"; }
				default: { return "Unknown severity"; }
				};
			}

			operator std::string () const noexcept {
				return static_cast<const char*>(*this);
			}

			operator std::string_view() const noexcept {
				return static_cast<const char*>(*this);
			}

			operator Type() const noexcept {
				return GetType();
			}

			[[nodiscard]]
			Type GetType() const noexcept { return type_; }

		private:
			Type type_;
		};

		void Log(Severity severity, const std::string_view path, const Common::Format& format, const std::source_location& location = std::source_location::current()) {
			//std::lock_guard lock{ mutex_ };
			Entry entry{ severity, path, format, location };
			std::cout << entry;
			std::cout.flush();
		}

	private:
		std::mutex mutex_;
		class Entry {
		public:

			class Column {
			public:
				Column(Common::Size width, const std::string_view& text) noexcept :
					width_{ width } {
					textLines_ = SeparateToLines(text);
					for (auto& line : textLines_) {
						textLinesViews_.emplace_back(Common::StringView{ line }).SetView(0, width);
					}
				}

				static std::vector<std::string> SeparateToLines(const std::string_view& text) noexcept {
					std::vector<std::string> lines;
					const Common::Index textLastSymbolIndex = text.size() - 1;
					const Common::Size textSize = text.size();
					for (
						Common::Index lineStart = 0,
						symbolIndex = 0; 
						symbolIndex < textSize;
						symbolIndex++) {
						const char symbol = text[symbolIndex];
						if (symbol == '\n') {
							const std::string line{ text.substr(lineStart, symbolIndex - lineStart) };
							lines.push_back(line);
							lineStart = symbolIndex + 1;
						}
						else if (symbolIndex == textLastSymbolIndex) {
							const std::string line{ text.substr(lineStart, symbolIndex - lineStart + 1).data() };
							lines.push_back(line);
						}
					}
					return lines;
				}
				
				void NextColumnLine() noexcept {
					Common::StringView& lineView = textLinesViews_[currentTextLine_];
					lineView.OffsetViewRight(width_);
					if (lineView.GetViewLength() == 0) {
						const bool isLastTextLine = (currentTextLine_ == textLinesViews_.size() - 1);
						if (!isLastTextLine) {
							++currentTextLine_;
						}
					}
				}

				[[nodiscard]]
				Common::Size GetWidth() const noexcept {
					return width_;
				}

				[[nodiscard]]
				std::optional<std::string> GetColumnLine() const noexcept {
					const Common::StringView& textLineView = textLinesViews_[currentTextLine_];
					if (textLineView.GetViewLength() != 0) {
						return textLineView.GetView();
					}
					return {};
				}

			private:
				Common::Index currentTextLine_ = 0;
				Common::Size width_ = 20;
				std::vector<std::string> textLines_;
				std::vector<Common::StringView> textLinesViews_;
			};

			Entry(Severity severity, const std::string_view path, const Common::Format& format, const std::source_location& location) {

				OS::AssertMessage(!path.empty(), "Attempt to use empty log path.");

				columns_.emplace_back(Column{ 10, severity });
				columns_.emplace_back(Column{ 30, path.data() });
				columns_.emplace_back(Column{ 30, format });
				columns_.emplace_back(Column{ 20, location.function_name() });
				std::filesystem::path pathToSource{ location.file_name() };
				columns_.emplace_back(Column{ 30, pathToSource.filename().string() });
				columns_.emplace_back(Column{  5, std::to_string(location.line()) });

				bool isLastLine = true;
				do {
					isLastLine = true;
					std::stringstream line;
					for (Column& column : columns_) {
						std::optional<std::string> maybeColumnLine = column.GetColumnLine();
						line << std::setw(column.GetWidth());
						line << std::left;
						if (maybeColumnLine.has_value()) {
							line << maybeColumnLine.value();
							column.NextColumnLine();
							if (column.GetColumnLine().has_value()) {
								isLastLine = false;
							}
						}
						line << "";
						line << delimiter_;
					}
					text_ << line.str();
					text_ << std::endl;
				} while (!isLastLine);

			}

			friend std::ostream& operator<<(std::ostream& os, Entry& entry) {
				return os << entry.text_.str();
			}

		private:
			std::vector<Common::Size> columnsWidth_{ 10, 30, 90, 20, 30, 5 };
			char delimiter_ = '|';
			std::stringstream text_;
			std::vector<Column> columns_;

		};
	};

	inline Logger logger;

	void LogError(
		const std::string_view path, 
		const Common::Format format,
		std::source_location location = std::source_location::current());

	void LogWarning(
		const std::string_view path,
		const Common::Format format,
		std::source_location location = std::source_location::current());

	void LogInfo(
		const std::string_view path,
		const Common::Format format,
		std::source_location location = std::source_location::current());

}
