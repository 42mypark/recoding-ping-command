#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <sys/socket.h>

#define BUFFER_SIZE 100

// struct iphdr {
// #if __BYTE_ORDER == __LITTLE_ENDIAN
//   unsigned int ihl : 4;
//   unsigned int version : 4;
// #elif __BYTE_ORDER == __BIG_ENDIAN
//   unsigned int version : 4;
//   unsigned int ihl : 4;
// #else
// #error "Please fix <bits/endian.h>"
// #endif
//   uint8_t tos;
//   uint16_t tot_len;
//   uint16_t id;
//   uint16_t frag_off;
//   uint8_t ttl;
//   uint8_t protocol;
//   uint16_t check;
//   uint32_t saddr;
//   uint32_t daddr;
//   /*The options start here. */
// };

// struct icmphdr {
//   uint8_t type; /* message type */
//   uint8_t code; /* type sub-code */
//   uint16_t checksum;
//   union {
//     struct {
//       uint16_t id;
//       uint16_t sequence;
//     } echo;           /* echo datagram */
//     uint32_t gateway; /* gateway address */
//     struct {
//       uint16_t __glibc_reserved;
//       uint16_t mtu;
//     } frag; /* path mtu discovery */
//   } un;
// };

int main() {
  int fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

  unsigned char buffer[BUFFER_SIZE];

  struct iphdr* ip = (struct iphdr*)buffer;
  struct icmphdr* icmp = (struct icmphdr*)(ip + 1);

  ip->ihl = 5;
  ip->version = 4;
  ip->tos = 0;  // ? packet priority?
  ip->tot_len = sizeof(buffer);
  ip->id = 1;  // ?
  ip->frag_off = 0;
  ip->frag_off |= IP_DF;
  ip->ttl = 255;
  ip->protocol = IPPROTO_ICMP;
  ip->check = 0;

  icmp->type = 8;
  icmp->code = 0;
  icmp->checksum = 0;
  icmp->un.echo.id = 1;  // ?
  icmp->un.echo.sequence = 0;

  printf("iphdr size: %d\nicmphdr size: %d\nchecksum calc size: %d\n",
         sizeof(struct iphdr), sizeof(struct icmphdr),
         sizeof(buffer) - sizeof(struct iphdr));  // maybe?
}
