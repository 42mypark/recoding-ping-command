#include "ft_ping.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "libft.h"

struct global g_;

static void ping_pong(int sig) {
  (void)sig;

  recv_pong();
  send_ping();

  // calc_statistics();

  gettimeofday(&g_.sent_time, NULL);  // error
  alarm(g_.interval);
}

void open_socket() {
  int on;
  int err;

  on = 1;
  g_.sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  err = setsockopt(g_.sockfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));

  if (g_.sockfd < 0 || err < 0) {
    fprintf(stderr, "Fatal Error\n");
    exit(1);
  }
}

int main(int argc, char **argv) {
  parse_arguments(argc, argv);
  open_socket();

  // for test
  g_.interval = 1;
  gettimeofday(&g_.start_time, NULL);  // error

  signal(SIGALRM, ping_pong);  // error
  signal(SIGINT, exit_program);
  signal(SIGQUIT, show_info);
  alarm(g_.interval);
  ping_pong(0);
  while (1)
    ;
}