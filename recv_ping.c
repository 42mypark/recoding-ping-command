#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>

#include "ft_ping.h"
#include "libft.h"

extern struct global g_;

static void init_msg_iov(struct msghdr* msg, size_t msg_iovlen,
                         struct iovec* iov, unsigned char* buffer) {
  ft_memset(buffer, 0, BUFFER_SIZE);
  iov->iov_base   = buffer;
  iov->iov_len    = BUFFER_SIZE;
  msg->msg_iov    = iov;
  msg->msg_iovlen = msg_iovlen;
}

static double latency() {
  double         time;
  struct timeval curr_time;

  gettimeofday(&curr_time, NULL);
  time = (double)(curr_time.tv_sec - g_.sent_time.tv_sec) * 1000 +
         (double)(curr_time.tv_usec - g_.sent_time.tv_usec) / 1000;
  return time;
}

static unsigned short get_id(unsigned char* buffer) {
  const int      iphdr   = sizeof(struct iphdr);
  const int      icmphdr = sizeof(struct icmphdr);
  unsigned char  type    = buffer[iphdr];
  unsigned short id;

  if (type == 0)
    id = *(unsigned short*)(buffer + iphdr + 4);
  else
    id = *(unsigned short*)(buffer + iphdr * 2 + icmphdr + 4);
  return id;
}

static void echo_reply(const unsigned char* buffer, const char* ip) {
  const int     iphdr = sizeof(struct iphdr);
  double        time  = latency();
  unsigned char seq   = buffer[iphdr + 7];
  unsigned char ttl   = buffer[8];
  int           bytes = BUFFER_SIZE - iphdr;
  int           error;

  if (g_.domain_name)
    error = printf("%d bytes from %s (%s): imcp_seq=%d ttl=%d time=%.1f ms\n",
                   bytes, g_.domain_name, ip, (int)seq, (int)ttl, time);
  else
    error = printf("%d bytes from %s: imcp_seq=%d ttl=%d time=%.1f ms\n", bytes,
                   ip, (int)seq, (int)ttl, time);

  fatal_error_check(error < 0, "printf");
  calc_statistics(time);
}

static void packet_error(const unsigned char* buffer, const char* ip) {
  const int     iphdr = sizeof(struct iphdr);
  unsigned char type  = buffer[iphdr];
  unsigned char code  = buffer[iphdr + 1];
  int           bytes = BUFFER_SIZE - iphdr;
  int           error;

  error = printf("%d bytes from %s: ", bytes, ip);
  switch (type) {
    case DESTINATION_UNREACHABLE:
      error = printf("Destination Unreachable");
      break;
    case TIME_EXCEEDED:
      error = printf("Time exceeded");
      break;
    default:
      break;
  }
  if (g_.options[(int)'v'])
    error = printf(" type=%d code=%d\n", (int)type, (int)code);
  else
    error = printf("\n");
  fatal_error_check(error < 0, "printf");
}

static void packet_return(unsigned char* buffer) {
  unsigned short id     = get_id(buffer);
  unsigned char  type   = buffer[sizeof(struct iphdr)];
  char           ip[16] = {0};

  if (id != getpid())
    return;

  inet_ntop(AF_INET, buffer + 12, ip, 16);
  if (type == 0) {
    echo_reply(buffer, ip);
    g_.received_count++;
    g_.loss_count--;
  } else {
    packet_error(buffer, ip);
    g_.error_count++;
  }
}

void recv_ping() {
  int           error;
  struct msghdr msg = {0};
  struct iovec  iov[1];
  unsigned char buffer[BUFFER_SIZE];

  init_msg_iov(&msg, 1, iov, buffer);
  error = recvmsg(g_.sockfd, &msg, MSG_WAITALL);
  if (error < 0)
    return;
  packet_return(buffer);
}