#include "common.hpp"

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

size_t writer(char *ptr, size_t size, size_t nmemb, std::string *data)
{
	if(data == NULL)
		return 0;
	data->append(ptr, size * nmemb);
	return size * nmemb;
}
