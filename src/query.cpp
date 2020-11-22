#include "query.hpp"

#include "detail/utils.hpp"

#include <exception>
#include <fmt/format.h>
#include <rapidjson/document.h>

namespace loki {

bool Cursor::Execute() {
	CURL *curl = curl_easy_init();
	std::string params = "";

	result_ = Result{};

	if (!curl) return false;

	char *output = curl_easy_escape(curl, query_.c_str(), query_.length());

	if (!output) return false;

	params += "?query=";
	params += output;
	params += "&start=0";
	if (start_ != 0) {
		params += "&end=";
		params += std::to_string(start_);
	}
	params += "&limit=";
	params += std::to_string(limit_);

	auto response = detail::get(curl, "http://localhost:3100/loki/api/v1/query_range", params);

	rapidjson::Document d{};
	d.Parse(response.body.c_str(), response.body.size());
	result_.status_ = d["status"].GetString();

	//fmt::print("{}\n", d["data"]["result"].GetArray().Size());
	//fmt::print("{}\n", std::stol(std::prev(std::prev(d["data"]["result"].End())->GetObject()["values"].End())->GetArray()[0].GetString()) + 1);

	if (d["data"]["result"].GetArray().Size()) {
		result_.result_type_ = d["data"]["resultType"] == "streams"
			? Result::ResultType::Streams
			: Result::ResultType::Matrix;

		if (result_.result_type_ == Result::ResultType::Streams) {
			auto i = std::stol(std::prev(std::prev(d["data"]["result"].End())->GetObject()["values"].End())->GetArray()[0].GetString()) - 1;
			if (i != start_) {
				start_ = i;
			}

			for (const auto & s : d["data"]["result"].GetArray()) {
				std::unordered_map<std::string, std::string> metrics;
				std::vector<std::pair<std::string, std::string>> values;

				for (auto itr = s["stream"].MemberBegin(); itr != s["stream"].MemberEnd(); ++itr) {
					metrics[itr->name.GetString()] = itr->value.GetString();
				}

				for (const auto & e: s["values"].GetArray()) {
					values.emplace_back(std::make_pair(
						e[0].GetString(),
						e[1].GetString()
					));
				}

				result_.stream_result_.emplace_back(Result::Stream{
					metrics, values
				});
			}

		} else if (result_.result_type_ == Result::ResultType::Matrix) {
			auto i = std::prev(std::prev(d["data"]["result"].End())->GetObject()["values"].End())[0].GetInt64() - 1;
			if (i != start_) {
				start_ = i;
			}

			// TODO ...
		}

	} else {
		return false;
	}

	curl_free(output);
	curl_easy_cleanup(curl);

	return true;
}

} // namespace loki
