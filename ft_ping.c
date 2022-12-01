/* allowed functions
◦ getpid. (?)
◦ getuid. (?)
◦ getaddrinfo.
◦ freeaddrinfo.
◦ gettimeofday. (?)
◦ inet_ntop.
◦ inet_pton.
◦ exit.
◦ signal. (?)
◦ alarm. (?)
◦ setsockopt.
◦ recvmsg.
◦ sendto.
◦ socket.
◦ printf and its family.
◦ Your libft functions.
*/

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
// #include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 84  // 84 why?

// void test_getpid() { printf("pid: %d\n", getpid()); }

// void test_getuid() { printf("uid: %d\n", getuid()); }

//  struct addrinfo {
//          int ai_flags;           /* input flags */
//          int ai_family;          /* protocol family for socket */
//          int ai_socktype;        /* socket type */
//          int ai_protocol;        /* protocol for socket */
//          socklen_t ai_addrlen;   /* length of socket-address */
//          struct sockaddr *ai_addr; /* socket-address for socket */
//          char *ai_canonname;     /* canonical name for service location */
//          struct addrinfo *ai_next; /* pointer to next in list */
//  };

// void test_getaddrinfo() {
//   struct addrinfo* res;
//   struct addrinfo* rp;
//   int error = getaddrinfo("www.naver.com", "80", NULL, &res);

//   for (rp = res; rp; rp = rp->ai_next) {
//     printf("getaddrinfo:\n");
//     printf("\t ai_flags   : %d\n", rp->ai_flags);
//     printf("\t ai_family  : %d\n", rp->ai_family);
//     printf("\t ai_socktype: %d\n", rp->ai_socktype);
//     printf("\t ai_protocol: %d\n", rp->ai_protocol);
//     printf("\t ai_addrlen : %d\n", rp->ai_addrlen);
//     printf("\t ai_addr:\n");
//     printf("\t\t sa_family: %d\n", rp->ai_addr->sa_family);
//     printf("\t\t sa_data  : %s\n", rp->ai_addr->sa_data);

//     char ip_toshow[1000] = {0};
//     struct sockaddr_in* in = (struct sockaddr_in*)rp->ai_addr;
//     inet_ntop(AF_INET, &in->sin_addr, ip_toshow, 1000);
//     printf("\t\t IPv4     : %s\n", ip_toshow);

//     printf("\t ai_canonname:%s\n", rp->ai_canonname);
//     printf("\t ai_next:%p\n", rp->ai_next);
//   }
//   freeaddrinfo(res);
// }

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

void extract_addr(struct sockaddr* addr) {
  struct addrinfo* res;
  // struct addrinfo* rp;
  int error = getaddrinfo("www.google.com", "80", NULL, &res);
  if (error) {
    printf("Error getaddrinfo\n");
    exit(1);
  }
  *addr = *res->ai_addr;
  freeaddrinfo(res);
}

unsigned short checksum_2byte(void* buffer, unsigned int byte_size) {
  unsigned short* buf = buffer;
  unsigned int count = byte_size / 2;
  unsigned int sum = 0;

  for (unsigned int i = 0; i < count; ++i) {
    sum += buf[i];
  }
  sum += (sum & 0xffff0000) >> 16;

  return ~sum;
}

int main() {
  // test_getpid();
  // test_getuid();
  // test_getaddrinfo();

  struct sockaddr_in src;
  struct sockaddr_in* dst;
  struct sockaddr dst_addr;

  extract_addr(&dst_addr);

  src.sin_addr.s_addr = inet_addr("10.0.2.15");
  dst = (struct sockaddr_in*)&dst_addr;

  int on = 1;
  int fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  int err = setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));

  if (fd < 0 || err < 0) {
    printf("Error %d %d\n", fd, err);
    perror(NULL);
    exit(1);
  }

  unsigned char buffer[BUFFER_SIZE] = {0};

  struct iphdr* ip = (struct iphdr*)buffer;
  struct icmphdr* icmphdr = (struct icmphdr*)(ip + 1);
  unsigned char* icmpdata = (unsigned char*)(icmphdr + 1);

  ip->ihl = 5;
  ip->version = 4;
  ip->tos = 0;  // ?
  ip->tot_len = sizeof(buffer);
  ip->id = 0;  // unique per tuple (src, dst, protocol), if (IP_DF) ignorable
  ip->frag_off = 0;
  ip->frag_off |= htons(IP_DF);  // why is not working? -> endian
  ip->ttl = 1;                   // os default 64
  ip->protocol = IPPROTO_ICMP;
  ip->saddr = src.sin_addr.s_addr;
  ip->daddr = dst->sin_addr.s_addr;
  ip->check = checksum_2byte(buffer, sizeof(struct iphdr));

  printf("%04x\n", ip->check);

  icmphdr->type = 8;
  icmphdr->code = 0;
  icmphdr->un.echo.id = 0;             // ? not fixed...
                                       // the id of icmp echo reply is same.
  icmphdr->un.echo.sequence = 0x0100;  // (LE) 0x0100 increase
  strcpy((char*)icmpdata, "hello");    // not allowed func
  icmphdr->checksum =
      checksum_2byte(icmphdr, sizeof(buffer) - sizeof(struct iphdr));

  printf("%04x\n", icmphdr->checksum);

  sendto(fd, buffer, BUFFER_SIZE, 0, &dst_addr, sizeof(dst_addr));

  struct msghdr msg;
  struct iovec iov[1];
  struct cmsghdr cmsg;

  iov[0].iov_base = malloc(BUFFER_SIZE);
  memset(iov[0].iov_base, 0, BUFFER_SIZE);
  iov[0].iov_len = BUFFER_SIZE;

  msg.msg_name = NULL;  // source address
  msg.msg_namelen = 0;
  msg.msg_iov = iov;
  msg.msg_iovlen = 1;
  msg.msg_control = &cmsg;            // ?
  msg.msg_controllen = sizeof(cmsg);  // ?

  printf("%ld\n", cmsg.cmsg_len);
  printf("%d\n", cmsg.cmsg_level);
  printf("%d\n", cmsg.cmsg_type);

  recvmsg(fd, &msg, MSG_WAITALL);
  unsigned char* recved = msg.msg_iov->iov_base;
  int size = msg.msg_iov->iov_len;
  for (int l = 0; l < size / 16 + 1; ++l) {
    for (int i = 0; i < 16 && l * 16 + i < size; ++i)
      printf("%02x ", recved[l * 16 + i]);
    printf("\n");
  }
  free(iov[0].iov_base);
}
