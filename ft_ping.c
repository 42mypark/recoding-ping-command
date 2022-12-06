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
  int on;
  int error;

  on        = 1;
  g_.sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  error     = setsockopt(g_.sockfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));
  fatal_error_check(g_.sockfd < 0 || error < 0);
}

int main(int argc, char **argv) {
  parse_arguments(argc, argv);
  open_socket();

  g_.interval = ft_atoi(g_.options[(int)'i']);
  g_.min      = g_.interval * 1000;
  gettimeofday(&g_.start_time, NULL);

  signal(SIGALRM, ping_pong);  // error
  signal(SIGINT, exit_program);
  signal(SIGQUIT, show_info);
  ping_pong(0);

  while (1)
    ;
}