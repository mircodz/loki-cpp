#include "detail/utils.hpp"

namespace loki::detail {

size_t writer(char *ptr, size_t size, size_t nmemb, std::string *data) {
	if (data == nullptr) {
		return 0;
	}
	data->append(ptr, size * nmemb);
	return size * nmemb;
}

Response request(
    CURL *curl, RequestMethod method, const std::string &url, const std::string &payload, ContentType content_type) {
	Response r{};
	struct curl_slist *headers = nullptr;

	curl_easy_setopt(curl, CURLOPT_URL, url.data());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &r.body);

	if (method == RequestMethod::Post) {
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.data());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, payload.size());

		if (content_type == ContentType::Json) {
			headers = curl_slist_append(headers, "Content-Type: application/json");
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		}
	}

	curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &r.code);

	return r;
}

Response get(CURL *curl, const std::string &url, const std::string &params) {
	return request(curl, RequestMethod::Get, url + params, "", ContentType::Raw);
}

Response post(CURL *curl, const std::string &url, const std::string &payload, ContentType content_type) {
	return request(curl, RequestMethod::Post, url, payload, content_type);
}

}  // namespace loki::detail
