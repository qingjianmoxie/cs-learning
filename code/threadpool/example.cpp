#include <iostream>
#include <chrono>
#include <condition_variable>
#include "thread_pool.h"

std::mutex g_mutex;

void testFunc()
{
  // loop to print character after a random period of time
  for (int i = 1; i < 4; ++i)
  {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      std::lock_guard<std::mutex> lock(g_mutex);
      std::cout << "testFunc() [" << i << "] at thread [ " << std::this_thread::get_id() << "] output" << std::endl;
  }
}


int main()
{
  sola::active_logger = std::unique_ptr<sola::logger>(new sola::logger(sola::logger::log_level::debug));

  sola::thread_pool thread_pool;

  for(int i = 0; i < 5 ; i++)
    thread_pool.add_task(testFunc);

  getchar();
  return 0;
}