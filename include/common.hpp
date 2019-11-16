#ifndef COMMON_HPP_
#define COMMON_HPP_

#include <vector>
#include <string>

#include <curl/curl.h>

namespace http {

struct Response {
	std::string body;
	int code;
};

enum RequestMethod { POST, GET };

Response cpost(CURL *curl, std::string_view uri, std::string_view payload);
Response cget(CURL *curl, std::string_view uri);
Response request(CURL *curl, RequestMethod method, std::string_view uri, std::string_view payload);

[[deprecated]] std::string post(std::string_view host, int port, std::string_view path, std::string_view payload);
[[deprecated]] std::string get(std::string_view host, int port, std::string_view path);
[[deprecated]] std::string full_get(std::string_view host, int port, std::string_view path);

namespace detail {

size_t writer(char *ptr, size_t size, size_t nmemb, std::string *data);

int get_code(const std::string &response);
std::string decode_chunked(int sock);

} // namespace detail

} // namespace http

#endif /* COMMON_HPP_ */
