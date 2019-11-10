#ifndef AGENT_HPP_
#define AGENT_HPP_

#include <map>

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

	Stream Add(std::map<std::string, std::string> labels);

private:
	std::map<std::string, std::string> labels_;
	int flush_interval_;
	int max_buffer_;
	LogLevels log_level_;
	std::string compiled_labels_;

};

#endif /* AGENT_HPP_ */
