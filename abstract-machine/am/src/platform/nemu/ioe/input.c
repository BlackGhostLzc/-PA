#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd)
{
  // bool keydown; int keycode
  // kbd->keydown = 0;
  // kbd->keycode = AM_KEY_NONE;
  uint32_t data = inl(KBD_ADDR);
  kbd->keydown = (bool)(data & KEYDOWN_MASK);
  kbd->keycode = (data & (~KEYDOWN_MASK));
}
