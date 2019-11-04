#include "common.hpp"

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include <fmt/format.h>
#include <iostream>

std::vector<std::string> split(const std::string &str, const std::string &delims)
{
	std::vector<std::string> output;
	size_t first = 0;
	while (first < str.size()) {
		const auto second = str.find_first_of(delims, first);
		if (first != second)
			output.emplace_back(str.substr(first, second - first));
		if (second == std::string::npos)
			break;
		first = second + 1;
	}
	return output;
}

namespace http {

std::string post(std::string_view host, int port, std::string_view path, std::string_view payload)
{

	constexpr std::string_view raw =
		"POST {} HTTP/1.1\r\n"
		"Host: {}:{}\r\n"
		"Accept: */*\r\n"
		"Content-Type: application/json\r\n"
		"Content-Length: {}"
		"\r\n\r\n{}";

	int valread = 0;
	int sock    = 0;

	struct sockaddr_in serv_addr;

	const std::string request = fmt::format(raw, path, port, host, payload.size(), payload);

	char buffer[1024] = { 0 };

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return "socket creation error";

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	if(inet_pton(AF_INET, host.data(), &serv_addr.sin_addr) <= 0)
		return "invalid address";

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		return "connection failed";

	send(sock, request.c_str(), request.size(), 0);
	valread = read(sock, buffer, sizeof buffer);

	return std::string(buffer);

}

std::string get(std::string_view host, int port, std::string_view path)
{
	constexpr std::string_view raw =
		"GET {} HTTP/1.1\r\n"
		"Host: {}:{}\r\n"
		"Accept: */*\r\n"
		"\r\n\r\n";

	int valread = 0;
	int sock    = 0;

	struct sockaddr_in serv_addr;

	const std::string request = fmt::format(raw, path, port, host);

	char buffer[1024] = { 0 };

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return "socket creation error";

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	if(inet_pton(AF_INET, host.data(), &serv_addr.sin_addr) <= 0)
		return "invalid address";

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		return "connection failed";

	send(sock, request.c_str(), request.size(), 0);
	valread = recv(sock, buffer, sizeof buffer, 0);

	return std::string(buffer);
}

std::string full_get(std::string_view host, int port, std::string_view path)
{
	constexpr std::string_view raw =
		"GET {} HTTP/1.1\r\n"
		"Host: {}:{}\r\n"
		"Accept: */*\r\n"
		"\r\n\r\n";

	int valread = 0;
	int sock    = 0;

	struct sockaddr_in serv_addr;

	std::string response;
	char buffer[16535] = { 0 };

	const std::string request = fmt::format(raw, path, port, host);

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return "socket creation error";

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	if(inet_pton(AF_INET, host.data(), &serv_addr.sin_addr) <= 0)
		return "invalid address";

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		return "connection failed";

	send(sock, request.c_str(), request.size(), 0);

	std::string b = "";
	char c[0];
	int chunk = 0;

	/* for now we can ignore the headers */
	while (b.find("\r\n\r\n") == std::string::npos) {
		read(sock, c, 1);
		b += c[0];
	}

	do {

		b = "";
		while (b.find("\r\n") == std::string::npos) {
			read(sock, c, 1);
			b += c[0];
		}
		read(sock, c, 1);
		read(sock, c, 1);

		try {
			chunk = std::stoi(b, nullptr, 16);
		} catch (...) { break; }

		read(sock, buffer, chunk);

		buffer[chunk] = '\0';
		response += buffer;

	} while(chunk);

	return response;
}

int get_code(const std::string &response)
{
	int l = response.find(" ");
	int r = response.find(" ", l);
	int code = 0;
	try {
		code = std::stoi(response.substr(l, r));
	} catch (...) { }
	return code;
}

}
