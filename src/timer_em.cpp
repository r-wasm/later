#ifdef __EMSCRIPTEN__

#include <errno.h>
#include <sys/time.h>
#include <stdio.h>

#include "timer_em.h"
#include "later.h"

#include <emscripten.h>

void do_timer_callback(void* data) {
  reinterpret_cast<Timer*>(data)->do_callback();
}

int Timer::bg_main_func(void* data) {
  reinterpret_cast<Timer*>(data)->bg_main();
  return 0;
}

void Timer::bg_main() {
  Guard guard(&this->mutex);
}

Timer::Timer(const std::function<void ()>& callback) :
  callback(callback), mutex(tct_mtx_recursive), cond(mutex), stopped(false) {
}

Timer::~Timer() {
}

void Timer::do_callback() {
  // Since webR blocks in the worker thread, skip the unix input handler
  // mechanism for now and just invoke the top-level callbacks directly
  execCallbacksForTopLevel();

  Optional<Timestamp> nextEvent = getGlobalRegistry()->nextTimestamp();
  if (nextEvent.has_value()) {
    this->set(*nextEvent);
  }
}

void Timer::set(const Timestamp& timestamp) {
  Guard guard(&this->mutex);

  Timestamp now;
  double msec_diff = 1000 * timestamp.diff_secs(now);

  EM_ASM({
    globalThis.Module.webr.setTimeoutWasm($0, $1, $2);
  }, do_timer_callback, msec_diff, this);
}

#endif // __EMSCRIPTEN__
