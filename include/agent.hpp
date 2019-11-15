#ifndef AGENT_HPP_
#define AGENT_HPP_

#include <atomic>
#include <chrono>
#include <map>
#include <mutex>
#include <queue>
#include <thread>

namespace loki
{

class Agent
{
public:
	enum LogLevels { Debug, Info, Warn, Error };

	Agent(const std::map<std::string, std::string> &labels,
		  int flush_interval,
		  int max_buffer,
		  LogLevels log_level);
	~Agent();

	void Spin();

	bool Ready();
	std::string Metrics();

	void Log(std::string msg);
	void Log(std::chrono::system_clock::time_point ts, std::string msg);
	void BulkLog(std::string msg);
	void QueueLog(std::string msg);
	void AsyncLog(std::string msg);

	// duplicate agent with extended labels
	Agent Extend(std::map<std::string, std::string> labels);

	// flush all logs
	void Flush();

private:
	std::map<std::string, std::string> labels_;
	int flush_interval_;
	int max_buffer_;
	LogLevels log_level_;
	std::string compiled_labels_;

	// the following attributes will be shared between agent instances

	// queue containing pairs of timestamps and logs to be flush
	static std::chrono::system_clock::time_point last_flush_;
	static std::queue<std::pair<std::chrono::system_clock::time_point, std::string>> logs_;
	static std::mutex lock_;

	// handle spinning thread
	static std::atomic<bool> close_request_;
	static std::thread thread_;

};

} // namespace loki

#endif /* AGENT_HPP_ */
