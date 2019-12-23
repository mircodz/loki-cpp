#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <curl/curl.h>
#include <string>

namespace loki
{

namespace detail
{

timespec now();
std::string to_string(const timespec &t);
void print(const std::string &line);

namespace http
{

enum class RequestMethod { Post, Get };
enum class ContentType   { Json, Raw };

struct Response
{
	std::string body;
	int code;
};

Response get(CURL *curl, const std::string &uri);
Response post(CURL *curl, const std::string &uri, const std::string &payload, ContentType content_type = ContentType::Raw);
Response request(CURL *curl, RequestMethod method, const std::string &uri, const std::string &payload, ContentType content_type);

size_t writer(char *ptr, size_t size, size_t nmemb, std::string *data);

} // namespace http

} // namespace detail

} // namespace loki

#endif /* UTILS_HPP_ */
