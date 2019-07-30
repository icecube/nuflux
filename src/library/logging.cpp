#include <NewNuFlux/logging.h>
#include <cstdarg>
#include <iostream>

I3LogLevel SimpleLogger::defaultLogLevel=I3DefaultLogLevel;
std::map<std::string,I3LogLevel> SimpleLogger::unitLevels;

I3LogLevel SimpleLogger::LogLevelForUnit(const std::string& unit){
	if(!unitLevels.count(unit))
		return(defaultLogLevel);
	return(unitLevels[unit]);
}
void SimpleLogger::SetLogLevelForUnit(const std::string& unit, I3LogLevel level){
	unitLevels[unit]=level;
}
void SimpleLogger::SetLogLevel(I3LogLevel level){
	defaultLogLevel=level;
}

std::string SimpleLogger::I3LoggingStringF(const char* format, ...){
	va_list args;
	va_start(args, format);
	
	size_t messagesize = vsnprintf(NULL, 0, format, args)+1;
	
	std::string s(messagesize,'\0');
	
	va_start(args, format);
	vsprintf(&*s.begin(), format, args);
	
	return s;
}

void SimpleLogger::Log(I3LogLevel level, const std::string& unit,
                       const std::string& file, int line, const std::string& func,
                       const std::string& message){
	const char *log_description;
	
	if (LogLevelForUnit(unit) > level)
		return;
	
	switch (level) {
		case I3LOG_TRACE:
			log_description = "TRACE";
			break;
		case I3LOG_DEBUG:
			log_description = "DEBUG";
			break;
		case I3LOG_INFO:
			log_description = "INFO";
			break;
		case I3LOG_NOTICE:
			log_description = "NOTICE";
			break;
		case I3LOG_WARN:
			log_description = "WARN";
			break;
		case I3LOG_ERROR:
			log_description = "ERROR";
			break;
		case I3LOG_FATAL:
			log_description = "FATAL";
			break;
		default:
			log_description = "UNKNOWN";
			break;
	}
	
	std::cerr << log_description << " (" << log_description << "): "
	<< message << " (" << file << ':' << line << " in " << func << std::endl;
}
