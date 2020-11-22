#ifndef LOKI_CPP_QUERY_HPP_
#define LOKI_CPP_QUERY_HPP_

#include <string>
#include <vector>
#include <unordered_map>

namespace loki {

class Result {
private:
	std::string status_;
	enum class ResultType {
		Streams, Matrix
	} result_type_;

	struct Stream {
		std::unordered_map<std::string, std::string> metric_;
		std::vector<std::pair<std::string, std::string>> values_;
	};

	struct Matrix {
		std::unordered_map<std::string, std::string> metric_;
		std::vector<std::pair<long, std::string>> values_;
	};

	std::vector<Stream> stream_result_;
	std::vector<Matrix> matrix_result_;

	std::unordered_map<std::string, float> summary_;
	std::unordered_map<std::string, float> store_;
	std::unordered_map<std::string, float> ingester_;

	friend class Cursor;
	friend class LineIterator;

public:
	Result()
		: status_{}
		, result_type_{}
		, stream_result_{}
		, matrix_result_{}
		, summary_{}
		, store_{}
		, ingester_{} {}
};

class Cursor {
private:
	std::string query_;

	long start_ = 0;
	int limit_ = 100;
	Result result_;

public:
	Cursor(std::string query)
		: query_{query} {}

	bool Execute();

	struct LineIterator {
		const Result &result_;

		decltype(result_.stream_result_.begin()) stream_cursor_;
		decltype(result_.matrix_result_.begin()) matrix_cursor_;

		decltype(stream_cursor_->values_.cbegin()) stream_value_cursor_;
		decltype(matrix_cursor_->values_.cbegin()) matrix_value_cursor_;

		LineIterator(const Result &result)
			: result_{result} {
			if (result_.result_type_ == Result::ResultType::Streams) {
				stream_cursor_ = result_.stream_result_.cbegin();
				if (stream_cursor_ != result_.stream_result_.cend()) {
					stream_value_cursor_ = stream_cursor_->values_.cbegin();
				}
			} else if (result_.result_type_ == Result::ResultType::Matrix) {
				matrix_cursor_ = result_.matrix_result_.cbegin();
				if (matrix_cursor_ != result_.matrix_result_.cend()) {
					matrix_value_cursor_ = matrix_cursor_->values_.cbegin();
				}
			}
		}

		LineIterator &begin() { return *this; }
		LineIterator &end() { return *this; }

		[[nodiscard]] bool operator!=(LineIterator&) const {
			if (result_.result_type_ == Result::ResultType::Streams) {
				return stream_cursor_ != result_.stream_result_.cend();
			} else if (result_.result_type_ == Result::ResultType::Matrix) {
				return matrix_cursor_ != result_.matrix_result_.cend();
			}
		}

		[[nodiscard]] const std::string &operator*() const {
			if (result_.result_type_ == Result::ResultType::Streams) {
				return stream_value_cursor_->second;
			} else if (result_.result_type_ == Result::ResultType::Matrix) {
				return matrix_value_cursor_->second;
			}
		}

		LineIterator &operator++() {
			if (result_.result_type_ == Result::ResultType::Streams) {
				stream_value_cursor_++;
				if (stream_value_cursor_ == stream_cursor_->values_.cend()) {
					stream_cursor_++;
					stream_value_cursor_ = stream_cursor_->values_.cbegin();
				}
			} else if (result_.result_type_ == Result::ResultType::Matrix) {
				matrix_value_cursor_++;
				if (matrix_value_cursor_ == matrix_cursor_->values_.cend()) {
					matrix_cursor_++;
					matrix_value_cursor_ = matrix_cursor_->values_.cbegin();
				}
			}
			return *this;
		}
	};

	[[nodiscard]] LineIterator Lines() const {
		return result_;
	}

	Cursor& operator++() {
		return *this;
	}

	[[nodiscard]] bool operator!=(Cursor&) {
		return Execute();
	}

	[[nodiscard]] bool operator==(Cursor&) {
		return !Execute();
	}

	[[nodiscard]] Cursor &begin() { return *this; }
	[[nodiscard]] Cursor &end() { return *this; }
	[[nodiscard]] Cursor &operator*() { return *this; }

};

// TODO parameter escaping (?) similar to SQls?
class Query {
private:
	std::string query_;

public:
	explicit Query(std::string query)
		: query_{query} {}

	Cursor Execute() {
		return Cursor{query_};
	}

};

} // namespace loki

#endif // ifndef LOKI_CPP_QUERY_HPP_
