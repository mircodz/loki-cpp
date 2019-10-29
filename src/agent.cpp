#include <chrono>
#include <iostream>

#include "agent.hpp"
#include "common.hpp"

Agent::Agent(const std::map<std::string, std::string> &labels,
			 const int flush_interval,
			 const int max_buffer,
			 const LogLevels log_level)
	: labels_(labels)
	, flush_interval_(flush_interval)
	, max_buffer_(max_buffer)
	, log_level_(log_level)
{
	curl_global_init(CURL_GLOBAL_ALL);
	handle = curl_easy_init();
}

Agent::~Agent()
{
	curl_easy_cleanup(handle);
	curl_global_cleanup();
}

size_t Agent::Ready()
{
	size_t code;
	curl_easy_setopt(handle, CURLOPT_URL, "http://localhost:3100/ready");
	auto res = curl_easy_perform(handle);
	curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &code);
	return code == 200 ? 1 : 0;
}

std::vector<std::string> Agent::Metrics()
{
	std::string body;

	curl_easy_setopt(handle, CURLOPT_URL, "http://localhost:3100/metrics");
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writer);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, &body);
	auto res = curl_easy_perform(handle);

	return split(body, "\n");
}

void Agent::Log(std::string msg)
{
	curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Content-Type: application/json");

	std::string payload = "";
	std::string body;

	curl_easy_setopt(handle, CURLOPT_URL, "http://localhost:3100/api/prom/push");

	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writer);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, &body);
	curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);

	payload += R"({"streams": [{ "stream": )";
	payload += R"({ "foo": "bar" })";
	payload += R"(, "values": [)";
	payload += R"( [ ")";
	payload += std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::system_clock::now().time_since_epoch()).count());
	payload += R"(", ")";
	payload += R"(fizzbuzz)";
	payload += R"(" ] )";
	payload += R"(] }]})";

	std::cout << payload << std::endl;

	curl_easy_setopt(handle, CURLOPT_POST, 1);
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, payload.c_str());

	auto res = curl_easy_perform(handle);

	curl_slist_free_all(headers);
	std::cout << body << std::flush;
}
