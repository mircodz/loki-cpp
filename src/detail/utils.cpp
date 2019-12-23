#include "detail/utils.hpp"

#include <chrono>

#include <fmt/format.h>

namespace loki
{

namespace detail
{

timespec now()
{
	auto t   = std::chrono::system_clock::now();
	auto sec = std::chrono::time_point_cast<std::chrono::seconds>(t);
	auto ns  = std::chrono::time_point_cast<
		std::chrono::nanoseconds>(t) - std::chrono::time_point_cast<std::chrono::nanoseconds>(sec);

	return timespec{sec.time_since_epoch().count(), ns.count()};
}

std::string to_string(const timespec &t)
{
	return std::to_string(t.tv_sec * 1000 * 1000 * 1000 + t.tv_nsec);
}

void print(const std::string &line)
{
	fmt::print(line + "\n");
}

namespace http
{

Response get(CURL *curl, const std::string &uri)
{
	return request(curl, RequestMethod::Get, uri, std::string{}, ContentType::Raw);
}

Response post(CURL *curl, const std::string &uri, const std::string &payload, ContentType content_type)
{
	return request(curl, RequestMethod::Post, uri, payload, content_type);
}

Response request(CURL *curl, RequestMethod method, const std::string &uri, const std::string &payload, ContentType content_type)
{
	Response r;

	curl_easy_setopt(curl, CURLOPT_URL, uri.data());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &r.body);

	if (method == RequestMethod::Post) {
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.data());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, payload.size());

		if (content_type == ContentType::Json) {
			struct curl_slist *headers = nullptr;
			headers = curl_slist_append(headers, "Content-Type: application/json");
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		}
	}

	curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &r.code);
	return r;
}

size_t writer(char *ptr, size_t size, size_t nmemb, std::string *data)
{
	if (data == nullptr)
		return 0;
	data->append(ptr, size * nmemb);
	return size * nmemb;
}

} // namespace http

} // namespace detail

} // namespace loki
