#include "common.hpp"

#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

#include <fmt/format.h>

namespace http
{

std::string post(std::string_view host, int port, std::string_view path, std::string_view payload)
{
	constexpr std::string_view raw =
		"POST {} HTTP/1.1\r\n"
		"Host: {}:{}\r\n"
		"Accept: */*\r\n"
		"Content-Type: application/json\r\n"
		"Content-Length: {}"
		"\r\n\r\n{}";

	char buffer[1024] = { 0 };
	const std::string request = fmt::format(raw, path, port, host, payload.size(), payload);
	int sock = 0;
	struct sockaddr_in serv_addr{};

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return "socket creation error";

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	if(inet_pton(AF_INET, host.data(), &serv_addr.sin_addr) <= 0)
		return "invalid address";

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		return "connection failed";

	send(sock, request.c_str(), request.size(), 0);
	read(sock, buffer, sizeof buffer);

	return std::string(buffer);
}

std::string get(std::string_view host, int port, std::string_view path)
{
	constexpr std::string_view raw =
		"GET {} HTTP/1.1\r\n"
		"Host: {}:{}\r\n"
		"Accept: */*\r\n"
		"\r\n\r\n";

	char buffer[1024] = { 0 };
	const std::string request = fmt::format(raw, path, port, host);
	int sock = 0;
	struct sockaddr_in serv_addr{};

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return "socket creation error";

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	if(inet_pton(AF_INET, host.data(), &serv_addr.sin_addr) <= 0)
		return "invalid address";

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		return "connection failed";

	send(sock, request.c_str(), request.size(), 0);
	recv(sock, buffer, sizeof buffer, 0);

	return std::string(buffer);
}

std::string full_get(std::string_view host, int port, std::string_view path)
{
	constexpr std::string_view raw =
		"GET {} HTTP/1.1\r\n"
		"Host: {}:{}\r\n"
		"Accept: */*\r\n"
		"\r\n\r\n";

	const std::string request = fmt::format(raw, path, port, host);
	int sock = 0;
	struct sockaddr_in serv_addr{};

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return "socket creation error";

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	if(inet_pton(AF_INET, host.data(), &serv_addr.sin_addr) <= 0)
		return "invalid address";

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		return "connection failed";

	send(sock, request.c_str(), request.size(), 0);

	return detail::decode_chunked(sock);
}

namespace detail
{

int get_code(const std::string &response)
{
	int l = response.find(' ');
	int r = response.find(' ', l);
	int code = 0;
	code = std::stoi(response.substr(l, r));
	return code;
}

std::string decode_chunked(int sock)
{
	// this took me way more time than I expected...

	constexpr int MAX_CHUNK_SIZE = 8096;

	char *s;
	char buffer[MAX_CHUNK_SIZE + 1] = { 0 };
	char c[1]; // read character by character into this buffer
	int chunk_size = 0;
	int l = 0, r = 0; // start and end of initial chunk size
	std::string cs = ""; // hold the string representation of `chunk_size`
	std::string response;

	// discard headers
	read(sock, buffer, 512);

	s = strstr(buffer, "\r\n\r\n");
	l = s - buffer;
	s = strstr(s + 3, "\r\n");
	r = s - buffer;

	// extract the position of the first chunk size
	cs = std::string(buffer).substr(l, r - l);
	response += std::string(buffer).substr(r + 2);
	chunk_size = std::stoi(cs, nullptr, 16) - response.size();
	if (chunk_size > MAX_CHUNK_SIZE) return "";

	// read extracted initial chunk size
	memset(buffer, '\0', sizeof buffer);
	read(sock, buffer, chunk_size);
	response += buffer;

	while (chunk_size) {
		// read next chunk size
		cs = "";
		read(sock, c, 1);
		read(sock, c, 1);
		while (cs.find("\r\n") == std::string::npos) {
			read(sock, c, 1);
			cs += c[0];
		}
		chunk_size = std::stoi(cs, nullptr, 16);
		if (chunk_size > MAX_CHUNK_SIZE) return "";

		// read next chunk
		int r = 0;
		while (r < chunk_size) {
			memset(buffer, '\0', sizeof buffer);
			r += read(sock, buffer, chunk_size - r);
			response += buffer;
		}
	}

	response.pop_back();
	return response;
}

} // namespace detail

} // namespace http
