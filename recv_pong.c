#include <arpa/inet.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>

#include "ft_ping.h"
#include "libft.h"

extern struct global g_;

static void set_socket_timeout() {
  int            error;
  struct timeval timeout;

  timeout.tv_sec  = g_.interval;
  timeout.tv_usec = 0;
  error           = setsockopt(g_.sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                               sizeof(timeout));  // ?
  fatal_error_check(error < 0);
}

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

static void print_message(unsigned char* buffer) {
  unsigned char type     = buffer[sizeof(struct iphdr)];
  unsigned char code     = buffer[sizeof(struct iphdr) + 1];
  unsigned char icmp_seq = buffer[sizeof(struct iphdr) + 7];
  unsigned char ttl      = buffer[8];
  int           bytes    = BUFFER_SIZE - sizeof(struct iphdr);
  char          ip[16]   = {0};
  double        time     = latency();

  inet_ntop(AF_INET, buffer + 12, ip, 16);
  if (type == 0) {
    printf("%d bytes from %s: imcp_seq=%d ttl=%d time=%.1f ms\n", bytes, ip,
           (int)icmp_seq, (int)ttl, time);
    calc_statistics(time);
    g_.received_count++;
    return;
  } else if (g_.options[(int)'v']) {
    printf("%d bytes from %s: type=%d code=%d\n", bytes, ip, (int)type,
           (int)code);
  }
  g_.loss_count++;
}

void recv_pong() {
  int           error;
  struct msghdr msg = {0};
  struct iovec  iov[1];
  unsigned char buffer[BUFFER_SIZE];

  init_msg_iov(&msg, 1, iov, buffer);
  set_socket_timeout();
  error = recvmsg(g_.sockfd, &msg, MSG_WAITALL);
  if (error < 0) {
    g_.loss_count++;
    return;
  }
  print_message(buffer);
}