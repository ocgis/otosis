#include "div.h"

unsigned char
pop_char (char ** stack_ptr) {
  return *((*stack_ptr)++);
}
