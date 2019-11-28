#include "parser.hpp"

#include <fmt/format.h>

namespace loki
{

void Parser::tokenize()
{
	while (cursor_ < source_.size()) {
		switch (source_[cursor_]) {
		case '#':
			new_token(Attribute, ""); step(2);
			new_token(AttributeType,  scan_untill(" "));
			new_token(AttributeKey,   scan_untill(" "));
			new_token(AttributeValue, scan_untill("\n"));
			break;
		default:
			new_token(MetricKey,   scan_untill("{ "));
			if (source_[cursor_ - 1] == '{')
				scan_labels();
			new_token(MetricValue, scan_untill("\n"));
		}
	}
}

void Parser::scan_labels()
{
	std::string_view v;
	new_token(MetricLabelStart, "");
	while (source_[cursor_] != '}') {
		new_token(MetricLabelKey, scan_untill("="));
		v = scan_untill(",}"); step(-1);
		step(source_[cursor_] == ',');
		new_token(MetricLabelValue, v.substr(1, v.size() - 2));
	}
	new_token(MetricLabelEnd, "");
}

std::string_view Parser::scan_untill(std::string_view delim)
{
	int cur = cursor_;
	int end = source_.find_first_of(delim, cursor_);
	if (end == std::string::npos) end = source_.size() - 1;
	cursor_ = end + 1;
	return source_.substr(cur, end - cur);
}

void Parser::new_token(Token token, std::string_view value)
{
	tokens_.emplace_back(std::make_pair(token, value));
}

void Parser::parse() {
	Metric cur{};
	cursor_ = 0;
	while (cursor_ < tokens_.size()) {
		switch (tokens_[cursor_].first) {
		case Attribute:
			if (tokens_[cursor_ + 1].second == "HELP")
				cur.help = tokens_[cursor_ + 3].second;
			else if (tokens_[cursor_ + 1].second == "TYPE")
				cur.type = tokens_[cursor_ + 3].second;
			step(3);
			break;
		case MetricLabelStart:
			step(1);
			while (tokens_[cursor_].first != MetricLabelEnd) {
				cur.labels.emplace_back(std::make_pair(
					tokens_[cursor_ + 0].second,
					tokens_[cursor_ + 1].second));
				step(2);
			}
			break;
		case MetricKey:
			cur.name = tokens_[cursor_].second;
			break;
		case MetricValue:
			cur.value = tokens_[cursor_].second;
			metrics_.emplace_back(cur);
			cur = Metric{};
			break;
		default:
			break;
		}
		step(1);
	}
}

} // namespace loki
