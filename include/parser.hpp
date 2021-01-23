#pragma once

#include <map>
#include <string>
#include <vector>

namespace loki {

struct Metric {
	std::string name;
	long double value;
	std::string help;
	std::string type;
	std::map<std::string, std::string> labels;
};

namespace detail {

struct Token {
	enum class Kind {
		AttributeType,
		AttributeKey,
		AttributeValue,
		MetricKey,
		MetricLabelStart,
		MetricLabelKey,
		MetricLabelValue,
		MetricLabelEnd,
		MetricValue
	} kind_;

	std::string value_;

	explicit Token(std::string value, Kind kind) : kind_{kind}, value_{value} {
	}
};

}  // namespace detail

class Parser {
private:
	using metrics_t = std::map<std::string, Metric>;

	metrics_t metrics_{};

	std::string source_;
	decltype(source_.cbegin()) cursor_{source_.cbegin()};

	bool has_next() const {
		return cursor_ != source_.end();
	}

	std::optional<Metric> Consume() {
		if (has_next()) {
			return std::nullopt;
		}

		Metric m{};

		while (true) {
			if (*cursor_ == '#') {
				cursor_ += 2;
				auto key = scan_until(" ");
				if (key == "HELP") {
					m.help = scan_until("\n");
				} else if (key == "TYPE") {
					m.type = scan_until("\n");
				} else {
					scan_until(" ");
					scan_until("\n");
				}
			} else {
				m.name = scan_until("{ ");
				if (*(cursor_ - 1) == '{') {
					while (*cursor_ != '}') {
						auto key      = scan_until("=");
						std::string v = scan_until(",}");
						cursor_ -= 1;
						if (*cursor_ == ',')
							cursor_++;
						auto value    = v.substr(1, v.size() - 2);
						m.labels[key] = value;
					}
				}
				cursor_ += 2;
				m.value          = std::stold(scan_until("\n"));
				metrics_[m.name] = m;
				return m;
			}
		}
	}

	std::string scan_until(const std::string &delim) {
		auto cur = cursor_ - source_.begin();
		auto end = source_.find_first_of(delim, cursor_ - source_.begin());
		if (end == std::string::npos) {
			end = source_.size() - 1;
		}
		cursor_ = source_.begin() + end + 1;
		return source_.substr(cur, end - cur);
	}

public:
	explicit Parser(const std::string &source) : source_{source} {
	}

	metrics_t Metrics() {
		while (Consume())
			;
		return metrics_;
	}

	class MetricsIterator {
	public:
		Parser &parser_;
		Metric metric_;
		Metric &operator*() {
			return metric_;
		}
		MetricsIterator &operator++() {
			metric_ = parser_.Consume().value();
			return *this;
		}
		MetricsIterator(Parser &parser) : parser_{parser} {
		}
	};

	MetricsIterator begin() {
		return *this;
	}
	MetricsIterator end() {
		return *this;
	}
};

}  // namespace loki
