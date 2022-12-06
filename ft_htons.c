#include <stdint.h>

uint16_t ft_htons(uint16_t host) {
  uint16_t network;

#if __BYTE_ORDER == __LITTLE_ENDIAN
  network = ((host & 0xff00) >> 8) | ((host & 0x00ff) << 8);
#else
  network = host;
#endif
  return network;
}