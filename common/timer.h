
#pragma once

#include <chrono>

using namespace std::chrono; 

#define TIMER_NOW(tv) \
        tv = std::chrono::steady_clock::now();

#define TIMER_ELAPSE(start, end, elapse)                                                    \
        do {                                                                                \
            std::chrono::microseconds __elapse__                                            \
                = std::chrono::duration_cast<std::chrono::microseconds>(end-start);         \
            elapse = __elapse__.count()/1000.0;                                             \
        } while(0);

namespace utils {
class MarsWalking {
public:
    MarsWalking() {
        TIMER_NOW(timer_start_);
    }
    virtual ~MarsWalking() {}

    void Elapse(double elapse) { __elapse = elapse; }
    double Elapse() { 
        TIMER_NOW(timer_end_);
        TIMER_ELAPSE(timer_start_, timer_end_, elapse_);
        elapse_ += __elapse;
        return elapse_;
    }
protected:
    double elapse_{0};
    double __elapse{0};
    std::chrono::steady_clock::time_point timer_start_;
    std::chrono::steady_clock::time_point timer_end_;
};
}

#ifdef TIMER_TEST

#define TIMER(tv) \
    std::chrono::steady_clock::time_point tv = std::chrono::steady_clock::now();

#define TIMER_END(tv, elapse)                                                               \
        double elapse = 0;                                                                  \
        do {                                                                                \
            std::chrono::steady_clock::time_point _tv_end_ = std::chrono::steady_clock::now(); \
            std::chrono::microseconds /*milliseconds*/ __elapse__                                            \
                = std::chrono::duration_cast<std::chrono::microseconds /*milliseconds*/>(_tv_end_-tv);   \
            elapse = __elapse__.count()/1000.0;                                                    \
        } while(0);

#define CTIMER(tv) \
    struct timeval tv; gettimeofday(&tv, NULL);
#define CTIMER_END(tv, elapse)                                                               \
        double elapse = 0;                                                                  \
        do {                                                                                \
            TIMER(__timer_end);                                                             \
            elapse = ((__timer_end.tv_sec*1000000L+__timer_end.tv_usec)-(tv.tv_sec*1000000L+tv.tv_usec))/1000.0; \
        } while(0);

#else

#define TIMER(tv)               \

#define TIMER_END(tv, elapse)   \
    double elapse = 0;
#endif

#define TIME_SINCE_EPOCH(tv) \
    int64_t tv =  std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();


