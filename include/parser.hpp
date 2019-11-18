#ifndef PARSER_HPP_
#define PARSER_HPP_

#include <string>
#include <vector>

// flase branch is not implemented yet
constexpr bool PARSE_LABELS = true;

namespace loki
{


struct Metric
{
	std::string_view metric;
	std::string_view value;
	std::string_view help;
	std::string_view type;
	std::vector<std::pair<std::string_view, std::string_view>> labels;
};

class Parser
{
public:
	Parser(std::string_view source)
		: source_(source)
		, cursor_(0)
	{
		tokenize();
		parse();
	}

	auto metrics() {
		return metrics_;
	}

	enum Token
	{ Newline
	, Attribute
	, AttributeType
	, AttributeKey
	, AttributeValue
	, MetricKey
	, MetricLabel
	, MetricLabelStart
	, MetricLabelKey
	, MetricLabelValue
	, MetricLabelEnd
	, MetricValue
	};

	/// lexer
	void tokenize();
	void scan_labels();
	std::string_view scan_untill(std::string_view delim);

	/// parser
	void parse();

	std::vector<std::pair<Token, std::string_view>> tokens;
	std::vector<Metric> metrics_;

private:
	std::string_view source_;
	int cursor_;

};

} // namespace loki

#endif /* PARSER_HPP_ */
