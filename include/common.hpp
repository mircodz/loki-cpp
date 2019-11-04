#include <vector>
#include <string>

std::vector<std::string> split(const std::string &str, const std::string &delims);


namespace http {

std::string post(std::string_view host, int port, std::string_view path, std::string_view payload);
std::string get(std::string_view host, int port, std::string_view path);
std::string full_get(std::string_view host, int port, std::string_view path);

int get_code(const std::string &response);

}
