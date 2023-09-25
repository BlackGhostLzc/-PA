#include <am.h>
#include <nemu.h>

void __am_timer_init()
{
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime)
{
  // AM_DEVREG(6, TIMER_UPTIME, RD, uint64_t us) 会创建一个结构体 TIMER_UPTIME_T,它有一个成员 uint64_t us
  // 我们需要从设备寄存器中读取时间，然后把这个时间写会这个 TIMER_UPTIME_T 物理内存中的变量
  uint64_t low = inl(RTC_ADDR);
  uint64_t high = inl(RTC_ADDR + 4);
  uptime->us = low + (high << 32);
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc)
{
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour = 0;
  rtc->day = 0;
  rtc->month = 0;
  rtc->year = 1900;
}
