#ifndef LOKI_CPP_UTILS_HPP_
#define LOKI_CPP_UTILS_HPP_

#include <curl/curl.h>
#include <string>

namespace loki::detail {

enum class RequestMethod { Post, Get };
enum class ContentType { Json, Raw };

struct Response {
	std::string body;
	int code;
};

Response get(CURL *curl, const std::string &url);
Response post(CURL *curl, const std::string &url, const std::string &payload, ContentType content_type = ContentType::Raw);
Response request(CURL *curl, RequestMethod method, const std::string &url, const std::string &payload, ContentType content_type);

size_t writer(char *ptr, size_t size, size_t nmemb, std::string *data);

} // namespace loki::detail

#endif /* LOKI_CPP_UTILS_HPP_ */
