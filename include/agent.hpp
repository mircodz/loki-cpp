#pragma once

#include <curl/curl.h>
#include <fmt/chrono.h>
#include <fmt/format.h>

#include <cstdio>
#include <ctime>
#include <mutex>
#include <string_view>
#include <unordered_map>
#include <vector>

#define USE_PROTOBUF

#if defined(__has_include) and defined(USE_PROTOBUF)
#if __has_include("snappy.h") and __has_include("google/protobuf/port_def.inc")
#define HAS_PROTOBUF
#endif
#endif

template <>
struct fmt::formatter<timespec> {
	template <typename ParseContext>
	constexpr auto parse(ParseContext &ctx) {
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(const timespec &s, FormatContext &ctx) {
		return format_to(ctx.out(), "{:%F %T}.{:09}", fmt::gmtime(s.tv_sec), s.tv_nsec);
	}
};

namespace loki {

/// \brief ASCII escape codes.
enum class Color : int {
	Black   = 30,
	Red     = 31,
	Green   = 32,
	Yellow  = 33,
	Blue    = 34,
	Magenta = 35,
	Cyan    = 36,
	White   = 37,
};

/// \brief Logging levels.
enum class Level : int {
	Debug = 0,
	Info  = 1,
	Warn  = 2,
	Error = 3,

	/// \brief Don't print anything. Default value.
	Disable = 4,
};

class Agent {
public:
	Agent(std::unordered_map<std::string, std::string> &labels,
	      std::size_t flush_interval,
	      std::size_t max_buffer,
	      Level log_level,
	      Level print_level,
	      const std::string &remote_host,
	      std::array<Color, 4> colors)
	    : labels_{labels},
	      flush_interval_{flush_interval},
	      max_buffer_{max_buffer},
	      log_level_{log_level},
	      print_level_{print_level},
	      remote_host_{remote_host},
	      push_url_{fmt::format("http://{}/loki/api/v1/push", remote_host_)},
	      colors_{colors} {
		logs_.resize(max_buffer_);
		cursor_ = logs_.begin();
	}

	Agent &operator=(const Agent &) = delete;

	Agent(const Agent &) = delete;

	virtual ~Agent() {
		if (curl_) {
			curl_easy_cleanup(curl_);
		}
		curl_ = nullptr;
	}

	/// \brief Return true if log contains any elements.
	bool Done() {
		return !logs_.empty();
	}

	/// \brief Flush all queued log lines.
	virtual void Flush() = 0;

	/// \brief Send message to log queue with `Debug` priority.
	template <typename... Args>
	void Debugf(std::string_view format, const Args &...args) {
		log(fmt::vformat(format, fmt::make_format_args(args...)), Level::Debug);
	}

	/// \brief Send message to log queue with `Info` priority.
	template <typename... Args>
	void Infof(std::string_view format, const Args &...args) {
		log(fmt::vformat(format, fmt::make_format_args(args...)), Level::Info);
	}

	/// \brief Send message to log queue with `Warning` priority.
	template <typename... Args>
	void Warnf(std::string_view format, const Args &...args) {
		log(fmt::vformat(format, fmt::make_format_args(args...)), Level::Warn);
	}

	/// \brief Send message to log queue with `Error` priority.
	template <typename... Args>
	void Errorf(std::string_view format, const Args &...args) {
		log(fmt::vformat(format, fmt::make_format_args(args...)), Level::Error);
	}

protected:
	const std::unordered_map<std::string, std::string> labels_;
	const std::size_t flush_interval_;
	const std::size_t max_buffer_;
	const Level log_level_;
	const Level print_level_;
	const std::string remote_host_;
	const std::string push_url_;

	/// \brief List of key-value pairs compiled to a string.
	std::array<std::string, 4> compiled_labels_;

	std::array<Color, 4> colors_;

	CURL *curl_ = curl_easy_init();

	struct Line {
		std::string line_{};
		std::timespec time_{};
		Level level_{};
	};

	std::vector<Line> logs_{};
	decltype(logs_.begin()) cursor_;

	std::recursive_mutex mutex_{};

	/// \brief Handle incoming logs.
	void log(std::string &&line, Level level) {
		std::lock_guard<std::recursive_mutex> lock{mutex_};

		std::timespec ts;
		std::timespec_get(&ts, TIME_UTC);

		if (print_level_ <= level) {
			print(line, level, ts);
		}

		if (log_level_ <= level) {
			cursor_->line_  = line;
			cursor_->time_  = ts;
			cursor_->level_ = level;
			cursor_++;
			if (cursor_ == logs_.end()) {
				Flush();
				cursor_ = logs_.begin();
			}
		}
	}

	/// \brief Printing function.
	void print(const std::string &line, Level level, std::timespec ts) const {
		fmt::print("\033[{}m{} {} {}\033[0m", static_cast<int>(colors_[static_cast<int>(level)]), ts,
		           level_to_fancy_label(level), line);
		putchar('\n');
	}

	[[nodiscard]] std::string_view level_to_simple_label(Level level) const noexcept {
		switch (level) {
		case Level::Debug: return "debug";
		case Level::Info: return "info";
		case Level::Warn: return "warn";
		case Level::Error: return "error";
		default: return "";
		}
	}

	[[nodiscard]] std::string_view level_to_fancy_label(Level level) const noexcept {
		switch (level) {
		case Level::Debug: return "[DEBUG]";
		case Level::Info: return "[ INFO]";
		case Level::Warn: return "[ WARN]";
		case Level::Error: return "[ERROR]";
		case Level::Disable:
		default: return "";
		}
	}
};

class AgentJson final : public Agent {
public:
	AgentJson(std::unordered_map<std::string, std::string> &labels,
	          std::size_t flush_interval,
	          std::size_t max_buffer,
	          Level log_level,
	          Level print_level,
	          const std::string &remote_host,
	          std::array<Color, 4> colors)
	    : Agent{labels, flush_interval, max_buffer, log_level, print_level, remote_host, colors} {
		BuildLabels();
	}

	void Flush();

protected:
	void BuildLabels();
};

#if defined(HAS_PROTOBUF)
class AgentProto final : public Agent {
public:
	AgentProto(std::unordered_map<std::string, std::string> &labels,
	           std::size_t flush_interval,
	           std::size_t max_buffer,
	           Level log_level,
	           Level print_level,
	           const std::string &remote_host,
	           std::array<Color, 4> colors)
	    : Agent{labels, flush_interval, max_buffer, log_level, print_level, remote_host, colors} {
		BuildLabels();
	}

	void Flush();

protected:
	void BuildLabels();
};
#endif

}  // namespace loki
