#ifndef AGENT_HPP_
#define AGENT_HPP_

#include <map>
#include <queue>
#include <chrono>

#include "stream.hpp"

enum LogLevels { Debug, Info, Warn, Error };

class Agent
{
public:
	Agent(const std::map<std::string, std::string> &labels,
		  int flush_interval,
		  int max_buffer,
		  LogLevels log_level);
	~Agent();

	bool Ready();
	std::string Metrics();

	void Log(std::string msg);
	void Log(std::chrono::system_clock::time_point ts, std::string msg);
	void QueueLog(std::string msg);
	void AsyncLog(std::string msg);

	Stream Add(std::map<std::string, std::string> labels);

	// forcefully flush all logs
	void Flush();

private:
	std::map<std::string, std::string> labels_;
	int flush_interval_;
	int max_buffer_;
	LogLevels log_level_;
	std::string compiled_labels_;

	// queue containing pairs of timestamps and logs to be flush
	std::queue<std::pair<std::chrono::system_clock::time_point, std::string>> logs_;

	std::chrono::system_clock::time_point last_flush_;

};

#endif /* AGENT_HPP_ */
