#include "agent.hpp"

#include "detail/utils.hpp"

#if defined(HAS_PROTOBUF)
#include <snappy.h>

#include "logproto.pb.h"
#endif

namespace loki {

void AgentJson::Flush() {
	std::lock_guard<std::recursive_mutex> lock{mutex_};

	fflush(stdout);

	if (logs_.begin() == cursor_) {
		return;
	}

	std::string payload = "";

	std::array<std::string, 4> streams{"[", "[", "[", "["};

	// Serialize queue of logs to 4 streams of logs, one for each log level.
	for (auto &&c = logs_.begin(); c != cursor_; c++) {
		auto i = static_cast<int>(c->level_);
		streams[i].reserve(c->line_.size());
		streams[i] += "[\"";
		streams[i] += std::to_string(c->time_.tv_sec * 1000 * 1000 * 1000 + c->time_.tv_nsec);
		streams[i] += "\",\"";
		detail::json_escape(streams[i], c->line_);
		streams[i] += "\"],";
	}
	cursor_ = logs_.begin();

	payload += "{\"streams\":[";

	for (int i = 0; i < static_cast<int>(Level::Error) + 1; i++) {
		if (streams[i].size() > 1) {
			// Remove last comma of each stream of logs.
			streams[i].pop_back();
			streams[i] += "]";

			payload += "{\"stream\":";
			payload += compiled_labels_[i];
			payload += ",\"values\":";
			payload += streams[i];
			payload += "},";
		}
	}

	// Remove last comma
	payload.pop_back();

	payload += "]}";

	detail::post(curl_, fmt::format("http://{}/loki/api/v1/push", remote_host_), payload, detail::ContentType::Json);
}

void AgentJson::BuildLabels() {
	std::string rest = "";
	for (auto &label : labels_) {
		rest += "\"";
		detail::json_escape(rest, label.first);
		rest += "\":\"";
		detail::json_escape(rest, label.second);
		rest += "\",";
	}

	for (int i = 0; i < static_cast<int>(Level::Error) + 1; i++) {
		std::string compiled = "{\"level\":\"";
		compiled += level_to_simple_label(static_cast<Level>(i));
		compiled += "\",";
		compiled += rest;
		compiled.pop_back();
		compiled += "}";
		compiled_labels_[i] = compiled;
	}
}

#if defined(HAS_PROTOBUF)

void AgentProto::Flush() {
	std::lock_guard<std::recursive_mutex> lock{mutex_};

	std::string payload;
	std::string compressed;
	logproto::PushRequest push_request;

	std::array<logproto::Stream *, 4> streams{};

	for (auto &&c = logs_.begin(); c != cursor_; c++) {
		auto *timestamp = new google::protobuf::Timestamp{};
		timestamp->set_seconds(c->time_.tv_sec);
		timestamp->set_nanos(c->time_.tv_nsec);

		auto i = static_cast<int>(c->level_);

		if (streams[i] == nullptr) {
			auto *stream = push_request.add_streams();
			stream->set_labels(compiled_labels_[0]);
			streams[i] = stream;
		}

		auto *entries = streams[i]->add_entries();
		entries->set_line(c->line_);
		entries->set_allocated_timestamp(timestamp);
	}

	cursor_ = logs_.begin();

	push_request.SerializeToString(&payload);
	snappy::Compress(payload.data(), payload.size(), &compressed);
	detail::post(curl_, fmt::format("http://{}/loki/api/v1/push", remote_host_), compressed);
}

void AgentProto::BuildLabels() {
	std::string rest = "";
	for (auto &label : labels_) {
		rest += "\"";
		detail::json_escape(rest, label.first);
		rest += "\"=\"";
		detail::json_escape(rest, label.second);
		rest += "\",";
	}

	for (int i = 0; i < static_cast<int>(Level::Error) + 1; i++) {
		std::string compiled = "{\"level\"=\"";
		compiled += level_to_simple_label(static_cast<Level>(i));
		compiled += "\",";
		compiled += rest;
		compiled.pop_back();
		compiled += "}";
		compiled_labels_[i] = compiled;
	}
}

#endif

}  // namespace loki
