/* allowed functions
◦ getpid. (-)
◦ getuid. (-)
◦ getaddrinfo. (o)
◦ freeaddrinfo. (-)
◦ gettimeofday. (-)
◦ inet_ntop. (-)
◦ inet_pton. (o)
◦ exit. (-)
◦ signal. (-)
◦ alarm. (-)
◦ setsockopt. (o)
◦ recvmsg. (o)
◦ sendto. (o)
◦ socket. (o)
◦ printf and its family. (?)
◦ Your libft functions.
*/

#include "ft_ping.h"

#include <arpa/inet.h>
#include <linux/icmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include "libft.h"

struct global g_;

static void ping(int sig) {
  static long long count = 0;
  (void)sig;

  alarm(g_.interval);
  send_ping();
  recv_ping();
  count++;
  if (count >= (long long)(g_.options[(int)'c']))
    exit_program(0);
}

void open_socket() {
  int                error;
  int                on;
  struct icmp_filter filter;
  struct timeval     timeout;

  g_.sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  fatal_error_check(g_.sockfd < 0, "socket");

  on    = 1;
  error = setsockopt(g_.sockfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));
  fatal_error_check(error < 0, "setsockopt");

  filter.data =
      ~(1 << ICMP_ECHOREPLY | 1 << ICMP_DEST_UNREACH | 1 << ICMP_REDIRECT |
        1 << ICMP_TIME_EXCEEDED | 1 << ICMP_PARAMETERPROB);
  error = setsockopt(g_.sockfd, SOL_RAW, ICMP_FILTER, (char *)&filter,
                     sizeof(filter));
  fatal_error_check(error < 0, "setsockopt");

  timeout.tv_sec  = g_.interval;
  timeout.tv_usec = 0;
  error =
      setsockopt(g_.sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
  error =
      setsockopt(g_.sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
  fatal_error_check(error < 0, "setsockopt");
}

int main(int argc, char **argv) {
  char        ip[16] = {0};
  const char *domain_name;
  parse_arguments(argc, argv);

  g_.interval = (int)(long long)g_.options[(int)'i'];
  g_.min      = g_.interval * 1000;
  gettimeofday(&g_.start_time, NULL);

  open_socket();

  signal(SIGALRM, ping);
  signal(SIGINT, exit_program);
  signal(SIGQUIT, show_info);

  inet_ntop(AF_INET, &g_.dst_ip.sin_addr, ip, sizeof(ip));
  domain_name = g_.domain_name ? g_.domain_name : ip;
  printf("PING %s (%s) %x(%d) bytes of data.\n", domain_name, ip, BUFFER_SIZE,
         BUFFER_SIZE);
  ping(0);

  while (1)
    ;
}