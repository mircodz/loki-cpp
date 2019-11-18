#include "parser.hpp"

#include <fmt/format.h>

namespace loki
{

void Parser::tokenize()
{
	// Thanks to Andy Balaam:
	// https://www.youtube.com/watch?v=TG0qRDrUPpA
	std::string_view t, k, v, l; // type, key, value, labels
	while (cursor_ < source_.size()) {
		switch (source_[cursor_]) {
		case '\n':
			tokens.emplace_back(std::make_pair(Newline, ""));
			cursor_ += 1;
			break;
		case '#':
			cursor_ += 2;
			t = scan_untill(" "); cursor_ += 1;
			k = scan_untill(" "); cursor_ += 1;
			v = scan_untill("\n");
			tokens.emplace_back(std::make_pair(Attribute, ""));
			tokens.emplace_back(std::make_pair(AttributeType, t));
			tokens.emplace_back(std::make_pair(AttributeKey, k));
			tokens.emplace_back(std::make_pair(AttributeValue, v));
			break;
		default:
			k = scan_untill("{ "); cursor_ += 1;
			tokens.emplace_back(std::make_pair(MetricKey, k));

			if (k[k.size()] == '{') {
				if constexpr(PARSE_LABELS) {
					scan_labels();
				} else {
					l = scan_untill("}"); cursor_ += 2;
					tokens.emplace_back(std::make_pair(MetricLabel, l));
				}
			}

			v = scan_untill("\n"); cursor_ += 1;
			tokens.emplace_back(std::make_pair(MetricValue, v));
		}
	}
}

void Parser::scan_labels()
{
	std::string_view k, v;
	tokens.emplace_back(std::make_pair(MetricLabelStart, ""));
	while (source_[cursor_] != '}') {
		k = scan_untill("="); cursor_ += 1;
		v = scan_untill(",}");
		cursor_ += source_[cursor_] == ',';
		tokens.emplace_back(std::make_pair(MetricLabelKey, k));
		tokens.emplace_back(std::make_pair(MetricLabelValue, v.substr(1, v.size() - 2)));
	}
	tokens.emplace_back(std::make_pair(MetricLabelEnd, ""));
}

std::string_view Parser::scan_untill(std::string_view delim)
{
	int cur = cursor_;
	int end = source_.find_first_of(delim, cursor_);
	if (end == std::string::npos) end = source_.size() - 1;
	cursor_ = end;
	return source_.substr(cur, end - cur);
}

void Parser::parse() {
	Metric cur{};
	cursor_ = 0;
	while (cursor_ < tokens.size()) {
		const auto &[token, value] = tokens[cursor_];
		switch (token) {
		case Attribute:
			if (tokens[cursor_ + 1].second == "HELP")
				cur.help = tokens[cursor_ + 3].second;
			else if (tokens[cursor_ + 1].second == "TYPE")
				cur.type = tokens[cursor_ + 3].second;
			cursor_ += 3;
			break;
		case MetricLabelStart:
			cursor_ += 1;
			while (tokens[cursor_].first != MetricLabelEnd) {
				cur.labels.emplace_back(std::make_pair(
					tokens[cursor_ + 0].second,
					tokens[cursor_ + 1].second));
				cursor_ += 2;
			}
			break;
		case MetricKey:
			cur.metric = value;
			break;
		case MetricValue:
			cur.value = value;
			metrics_.emplace_back(cur);
			cur = Metric{};
			break;
		default:
			break;
		}
		cursor_ += 1;
	}
}

} // namespace loki
