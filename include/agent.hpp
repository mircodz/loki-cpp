#ifndef AGENT_HPP_
#define AGENT_HPP_

#include <atomic>
#include <chrono>
#include <map>
#include <mutex>
#include <queue>
#include <thread>

#include <curl/curl.h>

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

	void Log(std::string msg);
	void Log(std::chrono::system_clock::time_point ts, std::string msg);
	void BulkLog(std::string msg);

	/// \return true on failure
	bool QueueLog(std::string msg);

	void AsyncLog(std::string msg);

	/// flush queued logs
	void Flush();

private:
	std::map<std::string, std::string> labels_;
	int flush_interval_;
	int max_buffer_;
	LogLevels log_level_;
	std::string compiled_labels_;

	// the queue is unique for each agent
	std::queue<std::pair<std::chrono::system_clock::time_point, std::string>> logs_;
	std::mutex mutex_;

	CURL *curl_;
};

} // namespace loki

#endif /* AGENT_HPP_ */
