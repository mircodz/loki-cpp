#ifndef COMMON_HPP_
#define COMMON_HPP_

#include <vector>
#include <string>

namespace http {

std::string post(std::string_view host, int port, std::string_view path, std::string_view payload);
std::string get(std::string_view host, int port, std::string_view path);
std::string full_get(std::string_view host, int port, std::string_view path);

namespace detail {

int get_code(const std::string &response);
std::string decode_chunked(int sock);

} // namespace detail

} // namespace http

#endif /* COMMON_HPP_ */

