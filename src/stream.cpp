#include "stream.hpp"

#include <chrono>
#include <iostream>

#include "common.hpp"

Stream::Stream(const std::map<std::string, std::string> &labels)
	: labels_(labels)
{
	compiled_labels_ += "{";
	for (const auto &label : labels_) {
		compiled_labels_ += "\"" + label.first + "\":\"" + label.second + "\",";
	}
	compiled_labels_.pop_back();
	compiled_labels_ += "}";
}

/**
 * @todo see Agent::Log
 */
void Stream::Log(std::string msg)
{
	std::string payload;
	payload += R"({"streams":[{"stream":)";
	payload += compiled_labels_;
	payload += R"(,"values":[[")";
	payload += std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::system_clock::now().time_since_epoch()).count());
	payload += R"(",")";
	payload += msg;
	payload += R"("]]}]})";
	http::post("127.0.0.1", 3100, "/loki/api/v1/push", payload);
}
