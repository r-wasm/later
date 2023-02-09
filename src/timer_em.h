#ifndef _TIMER_POSIX_H_
#define _TIMER_POSIX_H_

#ifdef __EMSCRIPTEN__

#include <functional>
#include "timestamp.h"
#include "threadutils.h"
#include "optional.h"

class Timer {
  std::function<void ()> callback;
  Mutex mutex;
  ConditionVariable cond;
  bool stopped;
  static int bg_main_func(void*);
  void bg_main();
public:
  Timer(const std::function<void ()>& callback);
  virtual ~Timer();
  void set(const Timestamp& timestamp);
  void do_callback();
};


#endif // __EMSCRIPTEN__

#endif // _TIMER_POSIX_H_
