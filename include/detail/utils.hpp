#pragma once

#include <curl/curl.h>

#include <string>

namespace loki::detail {

enum class RequestMethod { Post, Get };
enum class ContentType { Json, Raw };

struct Response {
	std::string body;
	int code;
};

size_t writer(char *ptr, size_t size, size_t nmemb, std::string *data);
Response request(
    CURL *curl, RequestMethod method, const std::string &url, const std::string &payload, ContentType content_type);
Response get(CURL *curl, const std::string &url, const std::string &params = "");
Response post(CURL *curl,
              const std::string &url,
              const std::string &payload,
              ContentType content_type = ContentType::Raw);

}  // namespace loki::detail
