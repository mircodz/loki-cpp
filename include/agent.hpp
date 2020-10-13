#ifndef LOKI_CPP_AGENT_HPP_
#define LOKI_CPP_AGENT_HPP_

#include <map>
#include <mutex>
#include <queue>

#include <curl/curl.h>
#include <fmt/format.h>

#define USE_PROTOBUF

#if defined(__has_include) and defined(USE_PROTOBUF)
#  if __has_include("snappy.h") and __has_include("google/protobuf/port_def.inc")
#    define HAS_PROTOBUF
#  endif
#endif

namespace loki {

/// \brief ASCII escape codes.
enum class Color : int {
	Black   =  30,
	Red     =  31,
	Green   =  32,
	Yellow  =  33,
	Blue    =  34,
	Magenta =  35,
	Cyan    =  36,
	White   =  37,
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
	Agent(
		std::map<std::string, std::string> &&labels,
		std::size_t flush_interval,
		std::size_t max_buffer,
		Level log_level,
		Level print_level,
		const std::string &remote_host,
		std::array<Color, 4> colors)
	: labels_{labels}
	, flush_interval_{flush_interval}
	, max_buffer_{max_buffer}
	, log_level_{log_level}
	, print_level_{print_level}
	, remote_host_{remote_host}
	, push_url_{fmt::format("http://{}/loki/api/v1/push", remote_host_)}
	, colors_{colors} {
		curl_ = curl_easy_init();
	}

	Agent& operator=(const Agent&) = delete;
	Agent(const Agent&) = delete;

	~Agent() {
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
	void Flush() {}

	/// \brief Send message to log queue with `Debug` priority.
	template <typename... Args>
	void Debugf(std::string_view format, const Args&... args) {
		Log(fmt::vformat(format, fmt::make_format_args(args...)), Level::Debug);
	}

	/// \brief Send message to log queue with `Info` priority.
	template <typename... Args>
	void Infof(std::string_view format, const Args&... args) {
		Log(fmt::vformat(format, fmt::make_format_args(args...)), Level::Info);
	}

	/// \brief Send message to log queue with `Warning` priority.
	template <typename... Args>
	void Warnf(std::string_view format, const Args&... args) {
		Log(fmt::vformat(format, fmt::make_format_args(args...)), Level::Warn);
	}

	/// \brief Send message to log queue with `Error` priority.
	template <typename... Args>
	void Errorf(std::string_view format, const Args&... args) {
		Log(fmt::vformat(format, fmt::make_format_args(args...)), Level::Error);
	}

protected:
	std::map<std::string, std::string> labels_;
	std::size_t flush_interval_;
	std::size_t max_buffer_;
	Level log_level_;
	Level print_level_;
	std::string remote_host_;
	std::string push_url_;
	std::string compiled_labels_;
	std::array<Color, 4> colors_;

	std::queue<std::pair<std::string, timespec>> logs_{};
	
	CURL *curl_;

	std::recursive_mutex mutex_{};

	/// \brief Append to `line` escaped string `str`.
	void Escape(std::string &line, const std::string &str) const;

	/// \brief Handle incoming logs.
	void Log(std::string &&line, Level level);

	/// \brief Printing function.
	void Print(const std::string &line, Level level, timespec ts) const;

	void BuildLabels();
};

class AgentJson final : public Agent {
public:
	AgentJson(
		std::map<std::string, std::string> &&labels,
		std::size_t flush_interval,
		std::size_t max_buffer,
		Level log_level,
		Level print_level,
		const std::string &remote_host,
		std::array<Color, 4> colors)
	: Agent{std::move(labels), flush_interval, max_buffer, log_level, print_level, remote_host, colors} {
		BuildLabels();
	}
	
	void Flush();

protected:
	void BuildLabels();
};


#if defined(HAS_PROTOBUF)
class AgentProto final : public Agent {
public:
	AgentProto(
		std::map<std::string, std::string> &&labels,
		std::size_t flush_interval,
		std::size_t max_buffer,
		Level log_level,
		Level print_level,
		const std::string &remote_host,
		std::array<Color, 4> colors)
	: Agent{std::move(labels), flush_interval, max_buffer, log_level, print_level, remote_host, colors} {
		BuildLabels();
	}

	void Flush();

protected:
	void BuildLabels();
};
#endif

} // namespace loki

#endif /* LOKI_CPP_AGENT_HPP_ */
