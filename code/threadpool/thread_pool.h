#ifndef _thread_pool_HPP
#define _thread_pool_HPP

#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

// convenience macro to log with file and line information
#ifdef __SOLA_LOGGING_ENABLED
#define __SOLA_LOG(level, msg) sola::level(msg, __FILE__, __LINE__);
#else
#define __SOLA_LOG(level, msg)
#endif /* __SOLA_LOGGING_ENABLED */

// 命名空间为sola
namespace sola {

class logger_iface {
public:
    logger_iface() = default;
    virtual ~logger_iface() = default;

    // 拷贝构造函数
    logger_iface(const logger_iface&) = default;
    // 赋值运算符重载
    logger_iface& operator=(const logger_iface&) = default;

public:
    /**
      @param msg message to be logged
      @param file file from which the message is coming
      @param line line in the file of the message
    */
    virtual void debug(const std::string& msg, const std::string& file,
                       std::size_t line) = 0;

    virtual void info(const std::string& msg, const std::string& file,
                      std::size_t line) = 0;

    virtual void warn(const std::string& msg, const std::string& file,
                      std::size_t line) = 0;

    virtual void error(const std::string& msg, const std::string& file,
                       std::size_t line) = 0;
};

// default logger class provided by the library
class logger : public logger_iface {
public:
    enum class log_level { error = 0, warn = 1, info = 2, debug = 3 };

public:
    logger(log_level level = log_level::info);
    ~logger() = default;

    logger(const logger&) = default;
    logger& operator=(const logger&) = default;

public:
    /**
      @param msg message to be logged
      @param file file from which the message is coming
      @param line line in the file of the message
    */
    void debug(const std::string& msg, const std::string& file,
               std::size_t line);

    void info(const std::string& msg, const std::string& file,
              std::size_t line);

    void warn(const std::string& msg, const std::string& file,
              std::size_t line);

    void error(const std::string& msg, const std::string& file,
               std::size_t line);

private:
    // current log level in use
    log_level m_level;

    // mutex used to serialize logs in multithreaded environment
    std::mutex m_mutex;
};

// variable containing the current logger
// by default, not set (no logs)
extern std::unique_ptr<logger_iface> active_logger;

/**
  debug logging
  convenience function used internally to call the logger
  @param msg message to be logged
  @param file file from which the message is coming
  @param line line in the file of the message
*/
void debug(const std::string& msg, const std::string& file, std::size_t line);

/**
  info logging
  convenience function used internally to call the logger
  @param msg message to be logged
  @param file file from which the message is coming
  @param line line in the file of the message
*/
void info(const std::string& msg, const std::string& file, std::size_t line);

/**
  warn logging
  convenience function used internally to call the logger
  @param msg message to be logged
  @param file file from which the message is coming
  @param line line in the file of the message
*/
void warn(const std::string& msg, const std::string& file, std::size_t line);

/**
  error logging
  convenience function used internally to call the logger
  @param msg message to be logged
  @param file file from which the message is coming
  @param line line in the file of the message
*/
void error(const std::string& msg, const std::string& file, std::size_t line);

class thread_pool {
public:
    typedef std::function<void()> task_t;

    thread_pool(int init_size = 3)
        : m_init_threads_size(init_size), m_mutex(), m_cond(), m_is_started(false) {
        start();
    }

    ~thread_pool() {
        if (m_is_started) {
            stop();
        }
    }

    void stop();
    void add_task(const task_t&);  // thread safe;

private:
    thread_pool(const thread_pool&);  //禁止复制拷贝.
    const thread_pool& operator=(const thread_pool&);

    bool is_started() { return m_is_started; }
    void start();

    void thread_loop();
    task_t take();

    typedef std::vector<std::thread*> threads_t;
    typedef std::deque<task_t> tasks_t;

    int m_init_threads_size;

    threads_t m_threads;
    tasks_t m_tasks;

    std::mutex m_mutex;
    std::condition_variable m_cond;
    bool m_is_started;
};

}  // namespace sola
#endif
