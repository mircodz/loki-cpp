#ifndef COMMON_HPP_
#define COMMON_HPP_

#include <curl/curl.h>
#include <string>

namespace loki
{

timespec now();
std::string to_string(const timespec &t);
void print(const std::string &line);

namespace http
{

enum class RequestMethod { Post, Get };
enum class ContentType   { Json, Raw };

struct Response {
	std::string body;
	int code;
};

Response post(CURL *curl, const std::string &uri, const std::string &payload, ContentType content_type = ContentType::Raw);
Response get(CURL *curl, const std::string &uri);
Response request(CURL *curl, RequestMethod method, const std::string &uri, const std::string &payload, ContentType content_type);

namespace detail
{

size_t writer(char *ptr, size_t size, size_t nmemb, std::string *data);

} // namespace detail

} // namespace http

} // namespace loki

#endif /* COMMON_HPP_ */
