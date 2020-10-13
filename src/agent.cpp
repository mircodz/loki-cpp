#include "agent.hpp"

#include <cstdio>
#include <ctime>

#if defined(HAS_PROTOBUF)
#include <snappy.h>
#endif

#include "detail/utils.hpp"
#include "logproto.pb.h"

namespace loki {

void Agent::Log(std::string &&line, Level level) {
	std::lock_guard<std::recursive_mutex> lock{mutex_};

	if (logs_.size() <= max_buffer_) {
		Flush();
	}

	std::timespec ts;
	std::timespec_get(&ts, TIME_UTC);

	if (print_level_ <= level) {
		Print(line, level, ts);
	}

	if (log_level_ <= level) {
		logs_.emplace(std::make_pair(std::move(line), std::move(ts)));
	}
}

void Agent::Print(const std::string &line, Level level, timespec ts) const {
	const auto get_label = [](Level level) -> std::string_view {
		switch (level) {
			case Level::Debug:   return "[DEBUG]";
			case Level::Info:    return "[ INFO]";
			case Level::Warn:    return "[ WARN]";
			case Level::Error:   return "[ERROR]";
			case Level::Disable: return "";
		}
		return "";
	};

	char buf[128];
	std::strftime(buf, sizeof buf, "%F %T", std::gmtime(&ts.tv_sec));
	fmt::print("\033[{}m{}.{:09} {} {}\033[0m\n", static_cast<int>(colors_[static_cast<int>(level)]), std::string(buf), ts.tv_nsec, get_label(level), line);
}

void Agent::Escape(std::string &line, const std::string &str) const {
	line.reserve(str.size());
 	for (auto c : str) {
		switch (c) {
		case '\\':
			line += "\\\\";
			break;
		case '\"':
			line += "\\\"";
			break;
		default:
			line += c;
			break;
		}
	}
}

void AgentJson::BuildLabels() {
	compiled_labels_ = "{";
	for (auto label : labels_) {
		compiled_labels_ += "\"";
		Escape(compiled_labels_, label.first);
		compiled_labels_ += "\":\"";
		Escape(compiled_labels_, label.second);
		compiled_labels_ += "\",";
	}
	compiled_labels_.pop_back();
	compiled_labels_ += "}";
}

void AgentJson::Flush() {
	std::lock_guard<std::recursive_mutex> lock{mutex_};

	if (logs_.size() == 0) return;

	std::string payload = "";
	std::string line = "[";

	while (!logs_.empty()) {
		auto &&[s, t] = logs_.front();

		line.reserve(s.size());
		line += "[\"";
		line += std::to_string(t.tv_sec * 1000 * 1000 * 1000 + t.tv_nsec);
		line += "\",\"";
		Escape(line, s);
		line += "\"],";

		logs_.pop();
	}

	line.pop_back(); // remove last comma
	line += "]";

	payload.reserve(compiled_labels_.size() + line.size() + 36);
	payload += "{\"streams\":[{\"stream\":";
	payload += compiled_labels_;
	payload += ",\"values\":";
	payload += line;
	payload += "}]}";

	detail::post(curl_, "http://127.0.0.1:3100/loki/api/v1/push", payload);
}

#if defined(HAS_PROTOBUF)
void AgentProto::BuildLabels() {
	compiled_labels_ = "{";
	for (const auto &label : labels_) {
		Escape(compiled_labels_, label.first);
		compiled_labels_ += "=\"";
		Escape(compiled_labels_, label.second);
		compiled_labels_ += "\",";
	}
	compiled_labels_.pop_back();
	compiled_labels_ += "}";
}

void AgentProto::Flush() {
	std::lock_guard<std::recursive_mutex> lock{mutex_};

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
	detail::post(curl_, "http://127.0.0.1:3100/loki/api/v1/push", compressed);
}
#endif

} // namespace loki
