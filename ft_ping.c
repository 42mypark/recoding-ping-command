#include "ft_ping.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "libft.h"

struct global g_;

void ping_pong(int sig) {
  (void)sig;

  // recv_pong();
  // send_ping();
  // calc_statistics();

  gettimeofday(&g_.sent_time, NULL);  // error
  alarm(g_.interval);
}

int main(int argc, char **argv) {
  parse_arguments(argc, argv);

  // for test
  g_.interval = 1;
  g_.loss_count = 36;
  g_.received_count = 64;
  g_.total_count = 100;

  gettimeofday(&g_.start_time, NULL);  // error

  signal(SIGALRM, ping_pong);  // error
  signal(SIGINT, exit_program);
  signal(SIGQUIT, show_info);

  alarm(g_.interval);
  while (1)
    ;
}