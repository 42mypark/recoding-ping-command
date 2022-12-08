#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
// #include <time.h>

#include "ft_ping.h"
#include "libft.h"

extern struct global g_;

static int ttl() {
  // srand(time(NULL));
  // return rand() % 25 + 1;
  return (long long)(g_.options[(int)'t']) % 256;
}

unsigned short checksum_2byte(void* buffer, unsigned long byte_size) {
  unsigned short* buf   = buffer;
  unsigned int    count = byte_size / 2;
  unsigned int    sum   = 0;

  for (unsigned int i = 0; i < count; ++i) {
    sum += buf[i];
  }
  sum += (sum & 0xffff0000) >> 16;

  return ~sum;
}

static void set_ip_header(unsigned char* buffer) {
  struct iphdr*      ip = (struct iphdr*)buffer;
  struct sockaddr_in in;

  inet_pton(AF_INET, "0.0.0.0", &in.sin_addr);  // ?

  ip->ihl     = 5;
  ip->version = 4;
  ip->tos     = 0;
  ip->tot_len = BUFFER_SIZE;
  ip->id = 0;  // unique per tuple (src, dst, protocol), if (IP_DF) ignorable
  ip->frag_off = 0;
  ip->frag_off |= ft_htons(IP_DF);
  ip->ttl      = ttl();  // os default 64, FOR TEST random_ttl() FIXME
  ip->protocol = IPPROTO_ICMP;
  ip->saddr    = in.sin_addr.s_addr;
  ip->daddr    = g_.dst_ip.sin_addr.s_addr;
  ip->check    = checksum_2byte(ip, sizeof(struct iphdr));
}

static void set_icmp_header(unsigned char* buffer) {
  struct icmphdr* icmphdr  = (struct icmphdr*)(buffer + sizeof(struct iphdr));
  unsigned char*  icmpdata = (unsigned char*)(icmphdr + 1);

  icmphdr->type             = 8;
  icmphdr->code             = 0;
  icmphdr->un.echo.id       = getpid();  // the id of ipcmp echo reply is same.
  icmphdr->un.echo.sequence = ft_htons(g_.total_count);
  ft_strlcpy((char*)icmpdata, "hello",
             BUFFER_SIZE - sizeof(struct iphdr) - sizeof(struct icmphdr));
  icmphdr->checksum =
      checksum_2byte(icmphdr, BUFFER_SIZE - sizeof(struct iphdr));
}

void send_ping() {
  int           error;
  unsigned char buffer[BUFFER_SIZE] = {0};

  set_ip_header(buffer);
  set_icmp_header(buffer);
  error = sendto(g_.sockfd, buffer, BUFFER_SIZE, 0,
                 (struct sockaddr*)&g_.dst_ip, sizeof(g_.dst_ip));
  fatal_error_check(error < 0, "sendto");
  g_.total_count++;
  g_.loss_count++;
  gettimeofday(&g_.sent_time, NULL);
}