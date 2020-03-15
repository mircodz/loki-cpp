#include "agent.hpp"

#include <cstdio>
#include <ctime>

#if defined(HAS_PROTOBUF)
#include <snappy.h>
#endif

#include "detail/utils.hpp"
#include "logproto.pb.h"

namespace loki
{

Agent::Agent(
		const std::map<std::string, std::string> &labels,
		int flush_interval,
		int max_buffer,
		Level log_level,
		Level print_level,
		std::array<Color, 4> colors)
	: labels_{labels}
	, flush_interval_{flush_interval}
	, max_buffer_{max_buffer}
	, log_level_{log_level}
	, print_level_{print_level}
	, colors_{colors}
	, logs_{}
	, mutex_{}
{
	curl_ = curl_easy_init();
}

Agent::~Agent()
{
	curl_easy_cleanup(curl_);
	curl_ = nullptr;
}

void Agent::Flush() {}

bool Agent::Done()
{
	std::lock_guard<std::mutex> lock{mutex_};
	return !logs_.empty();
}

std::string Agent::Format(fmt::string_view &&format, fmt::format_args &&args) const
{
	return std::move(fmt::vformat(format, args));
}

void Agent::Log(std::string &&line, Level level)
{
	std::lock_guard<std::mutex> lock{mutex_};

	if (logs_.size() <= max_buffer_) {
		mutex_.unlock();
		Flush();
	}

	timespec ts;
	timespec_get(&ts, TIME_UTC);

	if (print_level_ <= level) {
		Print(line, level, ts);
	}

	if (log_level_ <= level) {
		logs_.emplace(std::make_pair(std::move(line), std::move(ts)));
	}
}

void Agent::Print(const std::string &line, Level level, timespec ts) const
{
	const auto repr = [](Level level) -> std::string {
		switch (level) {
		case Level::Debug:   return "[DEBUG]";
		case Level::Info:    return "[ INFO]";
		case Level::Warn:    return "[ WARN]";
		case Level::Error:   return "[ERROR]";
		case Level::Disable: return "";
		}
	};

	char buf[128];
	std::strftime(buf, sizeof buf, "%F %T", std::gmtime(&ts.tv_sec));
	fmt::print(
		"\033[{}m{}.{:09} {} {}\033[0m\n",
		static_cast<int>(colors_[static_cast<int>(level)]),
		std::string(buf),
		ts.tv_nsec,
		repr(level),
		line);
}

std::string Agent::Escape(const std::string &str) const
{
	std::string s = "";
	for (const auto &c : str) {
		switch (c) {
		case '\\':
			s += "\\\\";
			break;
		case '\"':
			s += "\\\"";
			break;
		default:
			s += c;
			break;
		}
	}
	return s;
}

AgentJson::AgentJson(
			const std::map<std::string, std::string> &labels,
			int flush_interval,
			int max_buffer,
			Level log_level,
			Level print_level,
			std::array<Color, 4> colors)
	: Agent{labels, flush_interval, max_buffer, log_level, print_level, colors }
{
	compiled_labels_ = "{";
	for (auto label : labels_) {
		compiled_labels_ += "\"";
		compiled_labels_ += Escape(label.first);
		compiled_labels_ += "\":\"";
		compiled_labels_ += Escape(label.second);
		compiled_labels_ += "\",";
	}
	compiled_labels_.pop_back();
	compiled_labels_ += "}";
}

void AgentJson::Flush()
{
	std::lock_guard<std::mutex> lock{mutex_};

	if (logs_.size() == 0) return;

	std::string payload = "";
	std::string line = "[";

	while (!logs_.empty()) {
		auto &[s, t] = logs_.front();

		line.reserve(s.size());
		line += "[\"";
		line += detail::to_string(t);
		line += "\",\"";
		line += Escape(s);
		line += "\"],";

		logs_.pop();
	}

	line.pop_back();
	line += "]";

	payload += "{\"streams\":[{\"stream\":";
	payload += compiled_labels_;
	payload += ",\"values\":";
	payload += line;
	payload += "}]}";

	detail::http::post(curl_, "http://127.0.0.1:3100/loki/api/v1/push", payload);
}

#if defined(HAS_PROTOBUF)
AgentProto::AgentProto(
			const std::map<std::string, std::string> &labels,
			int flush_interval,
			int max_buffer,
			Level log_level,
			Level print_level,
			std::array<Color, 4> colors)
	: Agent{labels, flush_interval, max_buffer, log_level, print_level, colors }
{
	compiled_labels_ = "{";
	for (const auto &label : labels_) {
		compiled_labels_ += Escape(label.first);
		compiled_labels_ += "=\"";
		compiled_labels_ += Escape(label.second);
		compiled_labels_ += "\",";
	}
	compiled_labels_.pop_back();
	compiled_labels_ += "}";
}

void AgentProto::Flush()
{
	std::lock_guard<std::mutex> lock{mutex_};

	std::string payload;
	std::string compressed;
	logproto::PushRequest push_request;

	auto *stream = push_request.add_streams();
	stream->set_labels(compiled_labels_);

	while (!logs_.empty()) {
		auto &[s, t] = logs_.front();

		auto *timestamp = new google::protobuf::Timestamp{};
		timestamp->set_seconds(t.tv_sec);
		timestamp->set_nanos(t.tv_nsec);

		auto *entries = stream->add_entries();
		entries->set_line(s);
		entries->set_allocated_timestamp(timestamp);

		logs_.pop();
	}

	push_request.SerializeToString(&payload);
	snappy::Compress(payload.data(), payload.size(), &compressed);
	detail::http::post(curl_, "http://127.0.0.1:3100/loki/api/v1/push", compressed);
}
#endif

} // namespace loki
