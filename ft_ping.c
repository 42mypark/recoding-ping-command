/* allowed functions
◦ getpid. (-)
◦ getuid. (?)
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

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "libft.h"

struct global g_;

static void ping_pong(int sig) {
  static unsigned long long count = 0;
  (void)sig;

  alarm(g_.interval);
  send_ping();
  recv_pong();
  count++;
  if (count >= (unsigned long long)ft_atoi(g_.options[(int)'c']))
    exit_program(0);
}

void open_socket() {
  int            error;
  int            on;
  struct timeval timeout;

  g_.sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  fatal_error_check(g_.sockfd < 0, "socket");

  on    = 1;
  error = setsockopt(g_.sockfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));  // ?
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
  parse_arguments(argc, argv);
  if (g_.options[0] == NULL) {
    fprintf(stderr, "ping: usage error: Destination address required\n");
    exit(1);
  }

  g_.interval = ft_atoi(g_.options[(int)'i']);
  g_.min      = g_.interval * 1000;
  gettimeofday(&g_.start_time, NULL);

  open_socket();

  signal(SIGALRM, ping_pong);
  signal(SIGINT, exit_program);
  signal(SIGQUIT, show_info);
  ping_pong(0);

  while (1)
    ;
}