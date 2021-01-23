#pragma once

#include "registry.hpp"

namespace loki {

template <typename T>
class Builder {
public:
	Builder<T>& Labels(const std::unordered_map<std::string, std::string>& labels) {
		labels_ = labels;
		return *this;
	}

	Builder<T>& FlushInterval(std::size_t flush_interval) {
		flush_interval_ = flush_interval;
		return *this;
	}

	Builder<T>& MaxBuffer(std::size_t max_buffer) {
		max_buffer_ = max_buffer;
		return *this;
	}

	Builder<T>& LogLevel(Level log_level) {
		log_level_ = log_level;
		return *this;
	}

	Builder<T>& PrintLevel(Level print_level) {
		print_level_ = print_level;
		return *this;
	}

	Builder<T>& Remote(const std::string& remote_host) {
		remote_host_ = remote_host;
		return *this;
	}

	Builder<T>& Colorize(Level level, Color color) {
		colors_[static_cast<int>(level)] = color;
		return *this;
	}

	Registry<T> Build() {
		return {std::move(labels_), flush_interval_,         max_buffer_, log_level_,
		        print_level_,       std::move(remote_host_), colors_};
	}

private:
	std::unordered_map<std::string, std::string> labels_{};
	std::size_t flush_interval_{5000};
	std::size_t max_buffer_{10000};
	Level log_level_{Level::Info};
	Level print_level_{Level::Disable};
	std::string remote_host_{"127.0.0.1:3100"};
	std::array<Color, 4> colors_{{Color::White, Color::White, Color::White, Color::White}};
};

}  // namespace loki
