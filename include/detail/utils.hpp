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

/// \brief Convert value (between 0 and 15) to a hexadecimal digit character
inline char hex_char(char c) {
	return c < 10 ? (c + '0') : (c - 10 + 'a');
}

/// \brief Append to `line` JSON escaped string `str`.
inline void json_escape(std::string &line, const std::string &str) {
	line.reserve(line.size() + str.size());
	for (auto c : str) {
		switch (c) {
		case '\\': line += "\\\\"; break;
		case '\"': line += "\\\""; break;
		case '\b': line += "\\b"; break;
		case '\f': line += "\\f"; break;
		case '\n': line += "\\n"; break;
		case '\r': line += "\\r"; break;
		case '\t': line += "\\t"; break;
		default:
			if (c >= ' ') {
				line += c;
				break;
			} else {
				line += "\\u00";
				line += hex_char(c >> 4);
				line += hex_char(c & 0xF);
				break;
			}
		}
	}
}

}  // namespace loki::detail
