#ifndef STREAM_HPP_
#define STREAM_HPP_

#include <map>

class Stream
{
	const std::map<std::string, std::string> labels_;
	std::string compiled_labels_;

public:
	Stream(const std::map<std::string, std::string> &labels);

	void Log(std::string msg);
	void AsyncLog(std::string msg);

};

#endif /* STREAM_HPP_ */
