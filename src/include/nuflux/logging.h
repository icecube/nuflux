///Drop in replacement for IceTray's logging subsystem
#ifndef I3_LOGGER //key off of this so we shouldn't clash with any other implementation we meet

#include <map>
#include <string>
#include <iosfwd>
#include <sstream>

typedef enum {
  I3LOG_TRACE,
  I3LOG_DEBUG,
  I3LOG_INFO,
  I3LOG_NOTICE,
  I3LOG_WARN,
  I3LOG_ERROR,
  I3LOG_FATAL
} I3LogLevel;

#ifdef NDEBUG
const I3LogLevel I3DefaultLogLevel = I3LOG_NOTICE;
#else
const I3LogLevel I3DefaultLogLevel = I3LOG_INFO;
#endif

class SimpleLogger{
private:
  static I3LogLevel defaultLogLevel;
  static std::map<std::string,I3LogLevel> unitLevels;
public:
  static I3LogLevel LogLevelForUnit(const std::string& unit);
  static void SetLogLevelForUnit(const std::string &unit, I3LogLevel level);
  static void SetLogLevel(I3LogLevel level);

  static std::string I3LoggingStringF(const char* format, ...)
    __attribute__((__format__ (__printf__, 1, 2)));

  static void Log(I3LogLevel level, const std::string& unit,
                  const std::string& file, int line, const std::string& func,
                  const std::string& message);
};



#define SET_LOGGER(X) \
  static inline const char *__icetray_logger_id(void) { return X; }

SET_LOGGER("nuflux"); //project-wide logger

#define I3_LOGGER(level, id, file, line, func, format, ...)     \
  SimpleLogger::Log(level, id, file, line, func,                \
                    SimpleLogger::I3LoggingStringF(format, ##__VA_ARGS__))

#define I3_STREAM_LOGGER(level, id, file, line, func, msg, epilogue) \
  do { std::ostringstream _i3_str_logger_str; _i3_str_logger_str << msg; SimpleLogger::Log(level, \
  id, file, line, func, _i3_str_logger_str.str()); epilogue } while (0)

#ifndef NDEBUG
#define log_trace(format, ...) I3_LOGGER(I3LOG_TRACE, \
  __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
  ##__VA_ARGS__)
#define log_debug(format, ...) I3_LOGGER(I3LOG_DEBUG, \
  __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
  ##__VA_ARGS__)
#ifdef __cplusplus
#define log_trace_stream(msg) I3_STREAM_LOGGER(I3LOG_TRACE, \
  __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, msg, )
#define log_debug_stream(msg) I3_STREAM_LOGGER(I3LOG_DEBUG, \
  __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, msg, )
#endif
#else
#define log_trace(format, ...)
#define log_debug(format, ...)
#define log_trace_stream(msg)
#define log_debug_stream(msg)
#endif

#define log_info(format, ...) I3_LOGGER(I3LOG_INFO, \
  __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
  ##__VA_ARGS__)
#define log_notice(format, ...) I3_LOGGER(I3LOG_NOTICE, \
  __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
##__VA_ARGS__)
#define log_warn(format, ...) I3_LOGGER(I3LOG_WARN, \
  __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
##__VA_ARGS__)
#define log_error(format, ...) I3_LOGGER(I3LOG_ERROR, \
  __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
##__VA_ARGS__)

#ifdef __cplusplus
#define log_fatal(format, ...) I3_LOGGER(I3LOG_FATAL, \
  __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
  ##__VA_ARGS__), throw std::runtime_error(SimpleLogger::I3LoggingStringF( \
  format, ##__VA_ARGS__) + " (in " + __PRETTY_FUNCTION__ + ")")
#define log_info_stream(msg) I3_STREAM_LOGGER(I3LOG_INFO, \
  __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, msg, )
#define log_notice_stream(msg) I3_STREAM_LOGGER(I3LOG_NOTICE, \
  __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, msg, )
#define log_warn_stream(msg) I3_STREAM_LOGGER(I3LOG_WARN, \
  __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, msg, )
#define log_error_stream(msg) I3_STREAM_LOGGER(I3LOG_ERROR, \
  __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, msg, )
#define log_fatal_stream(msg) I3_STREAM_LOGGER(I3LOG_FATAL, \
  __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, msg, \
 throw std::runtime_error(_i3_str_logger_str.str() + " (in " + __PRETTY_FUNCTION__ + ")");)
#else
#define log_fatal(format, ...) I3_LOGGER(I3LOG_FATAL, \
  __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
  ##__VA_ARGS__), kill(getpid(), SIGABRT)
#endif

#endif //I3_LOGGER
