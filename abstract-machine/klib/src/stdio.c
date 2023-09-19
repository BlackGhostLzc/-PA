#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...)
{
  char buf[1024];
  va_list ap;
  va_start(ap, fmt);

  int ret = vsprintf(buf, fmt, ap);
  va_end(ap);

  if (ret > 0)
    putstr(buf);

  return ret;
}

int vsprintf(char *out, const char *fmt, va_list ap)
{
  int d;
  // char c;
  char buf[1024];
  size_t out_idx = 0;
  size_t fmt_idx = 0;
  int format_state = 0;

  for (; fmt[fmt_idx] != '\0'; fmt_idx++)
  {
    switch (format_state)
    {
    case 0:
      if (fmt[fmt_idx] == '%')
        format_state = 1;
      else
        out[out_idx++] = fmt[fmt_idx];
      break;

    case 1:
      switch (fmt[fmt_idx])
      {
      case 'd':
        d = va_arg(ap, int);
        if (d == 0)
        {
          out[out_idx++] = '0';
          break;
        }
        int d_tmp = d;
        int buf_idx = 0;
        while (d_tmp)
        {
          buf[buf_idx++] = (char)(d_tmp % 10 + '0');
          d_tmp /= 10;
        }
        if (d < 0)
          buf[buf_idx] = '-';
        while (buf_idx >= 0)
        {
          out[out_idx++] = buf[buf_idx--];
        }
        break;

      case 's':
        char *tmp = va_arg(ap, char *);
        while (*tmp != '\0')
        {
          out[out_idx++] = *tmp++;
        }
        break;

      default:
        assert(0);
      }
    }
  }
  va_end(ap);
  out[out_idx] = '\0';
  return out_idx;
}

int sprintf(char *out, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  int ret = vsprintf(out, fmt, ap);

  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...)
{
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap)
{
  panic("Not implemented");
}

#endif
