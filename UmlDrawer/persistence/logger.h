#ifndef LOGGER_H
#define LOGGER_H
#include <QString>

#include <fstream>
#include <mutex>
class Logger
{
private:
  const static QString logfileDirName;
  std::string logOsFileName = "log.log";
  std::string warnOsFileName = "warnings.log";
  std::string errOsFileName = "errors.log";
	std::ofstream logOs;
	std::ofstream warnOs;
	std::ofstream errOs;
  std::mutex logOsMutex;
  std::mutex warnOsMutex;
  std::mutex errOsMutex;
	void my_assert(bool expr, QString failMsgPart1="", QString failMsgPart2="", QString failMsgPart3="", QString failMsgPart4="");
public:
	Logger();
	~Logger();
	void printTimeStamp(std::ostream& os);
	void flush();
	bool log(std::string msg);
	bool warn(std::string msg);
	bool err(std::string msg);
};

#endif // LOGGER_H
