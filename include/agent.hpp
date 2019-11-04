#ifndef AGENT_H
#define AGENT_H

#include <map>
#include <vector>

enum LogLevels { Debug, Info, Warn, Error };

class Agent
{
public:
	Agent(const std::map<std::string, std::string> &labels,
		  const int flush_interval,
		  const int max_buffer,
		  const LogLevels log_level);
	~Agent();

	bool Ready();
	std::vector<std::string> Metrics();

	void Log(std::string msg);

private:
	std::map<std::string, std::string> labels_;
	int flush_interval_;
	int max_buffer_;
	LogLevels log_level_;

};

#endif /* AGENT_H */
