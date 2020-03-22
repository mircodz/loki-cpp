#ifndef PARSER_HPP_
#define PARSER_HPP_

#include <string>
#include <vector>

namespace loki
{

struct Metric
{
	std::string name{};
	long double value{};
	std::string help{};
	std::string type{};
	std::vector<std::pair<std::string, std::string>> labels{};
};

class Token
{
public:
	enum class Kind
		{ AttributeType		
		, AttributeKey
		, AttributeValue
		, MetricKey
		, MetricLabelStart
		, MetricLabelKey
		, MetricLabelValue
		, MetricLabelEnd
		, MetricValue
	};

	explicit Token(std::string value, Kind kind)
		: value_{value}
		, kind_{kind} {}

	std::string to_string() const
	{
		switch (kind_) {
			case Kind::AttributeType: return "attribute type: " + value_;
			case Kind::AttributeKey: return "attribute key: " + value_;
			case Kind::AttributeValue: return "attribute value:" + value_;
			case Kind::MetricKey: return "metric key:" + value_;
			case Kind::MetricLabelStart: return "{";
			case Kind::MetricLabelKey: return "label key: " + value_;
			case Kind::MetricLabelValue: return "label value: " + value_;
			case Kind::MetricLabelEnd: return "}";
			case Kind::MetricValue: return "value:" + value_;
		}
	}

	std::string value_;
	Kind kind_;
};

class Lexer
{
public:
	explicit Lexer(const std::string &source)
		: source_{source} { tokenize(); }

	std::vector<Token> tokens() { return tokens_; }

private:
	std::vector<Token> tokens_{};
	std::string source_;
	decltype(source_.cbegin()) cursor_{source_.cbegin()};

	void tokenize()
	{
		while (cursor_ != source_.end()) {
			switch(*cursor_) {
			case '#':
				cursor_ += 2;
				if (std::string v = scan_until(" "); v == "TYPE" || v == "HELP") {
					new_token(v, Token::Kind::AttributeType);
					new_token(scan_until(" "), Token::Kind::AttributeKey);
					new_token(scan_until("\n"), Token::Kind::AttributeValue);
				} else {
					scan_until(" ");
					scan_until("\n");
				}
				break;
			default:
				new_token(scan_until("{ "), Token::Kind::MetricKey);
				if (*(cursor_ - 1) == '{') {
					new_token("", Token::Kind::MetricLabelStart);
					while (*cursor_ != '}') {
						new_token(scan_until("="), Token::Kind::MetricLabelKey);
						std::string v = scan_until(",}");
						cursor_ -= 1;
						if (*cursor_ == ',') cursor_++;
						new_token(v.substr(1, v.size() - 2), Token::Kind::MetricLabelValue);
					}
					new_token("", Token::Kind::MetricLabelEnd);
				}
				cursor_ += 2;
				new_token(scan_until("\n"), Token::Kind::MetricValue);
				break;
			}
		}
	}

	void new_token(std::string value, Token::Kind kind)
	{
		tokens_.emplace_back(Token{value, kind});
	}

	std::string scan_until(const std::string &delim)
	{
		auto cur = cursor_ - source_.begin();
		auto end = source_.find_first_of(delim, cursor_ - source_.begin());
		if (end == std::string::npos) end = source_.size() - 1;
		cursor_ = source_.begin() + end + 1;
		return source_.substr(cur, end - cur);
	}

};

class Parser
{
public:
	explicit Parser(const std::vector<Token> &tokens)
		: tokens_{tokens.begin(), tokens.end()} { parse(); }

	std::vector<Metric> metrics() { return metrics_; }

private:
	std::vector<Metric> metrics_{};
	std::vector<Token> tokens_;
	decltype(tokens_.cbegin()) cursor_{tokens_.cbegin()};

	void parse()
	{
		Metric m{};
		while (cursor_ < tokens_.end()) {
			switch (cursor_->kind_) {
			case Token::Kind::AttributeType:
				if (cursor_->value_ == "TYPE") {
					m.type = (cursor_ + 2)->value_;
				} else if (cursor_->value_ == "HELP") {
					m.help = (cursor_ + 2)->value_;
				}
				cursor_ += 3;
				break;
			case Token::Kind::MetricKey:
				m.name = cursor_->value_;
				cursor_++;
				break;
			case Token::Kind::MetricLabelStart:
				cursor_++;
				while (cursor_->kind_ != Token::Kind::MetricLabelEnd) {
					m.labels.emplace_back(std::make_pair(cursor_->value_, (cursor_ + 1)->value_));
					cursor_ += 2;
				}
				cursor_++;
				break;
			case Token::Kind::MetricValue:
				m.value = std::stold(cursor_->value_);
				metrics_.emplace_back(m);
				m = Metric{};
				cursor_++;
				break;
			case Token::Kind::AttributeKey:
			case Token::Kind::AttributeValue:
			case Token::Kind::MetricLabelKey:
			case Token::Kind::MetricLabelValue:
			case Token::Kind::MetricLabelEnd:
				cursor_++;
				break;
			}
		}
	}

};

} // namespace loki

#endif /* PARSER_HPP_ */
