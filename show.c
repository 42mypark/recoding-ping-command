#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

#include "ft_ping.h"

extern struct global g_;

static void show_result() {
  int loss_percent = (g_.loss_count * 100 / g_.total_count);
  int time_ms      = (g_.sent_time.tv_sec - g_.start_time.tv_sec) * 1000 +
                (g_.sent_time.tv_usec - g_.start_time.tv_usec) / 1000;
  int error;

  error = printf("\n--- %s ping statistics ---\n", (char *)g_.options[0]);
  fatal_error_check(error < 0, "printf");
  error = printf("%d packets transmitted, %d received, ", g_.total_count,
                 g_.received_count);
  fatal_error_check(error < 0, "printf");
  if (g_.error_count) {
    error = printf("%+d errors, ", g_.error_count);
    fatal_error_check(error < 0, "printf");
  }
  error = printf("%d%% packet loss, time %dms\n", loss_percent, time_ms);
  fatal_error_check(error < 0, "printf");
  if (g_.received_count) {
    error = printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n", g_.min,
                   g_.avg, g_.max, g_.mdev);
    fatal_error_check(error < 0, "printf");
  }
}

void exit_program(int sig) {
  (void)sig;
  show_result();
  exit(1);
}

void show_info(int sig) {
  int loss_percent = (g_.loss_count * 100 / g_.total_count);
  int error;

  (void)sig;
  error = printf("\b\b%d/%d packets, %d%% loss", g_.received_count,
                 g_.total_count, loss_percent);
  fatal_error_check(error < 0, "printf");
  if (g_.received_count)
    error = printf(", min/avg/ewma/max = %.3lf/%.3lf/%.3lf/%.3lf ms\n", g_.min,
                   g_.avg, g_.ewma, g_.max);
  else
    error = printf("\n");
  fatal_error_check(error < 0, "printf");
}

void show_help() {
  fprintf(stderr, "\nUsage\n");
  fprintf(stderr, "  ping [options] <destination>\n");
  fprintf(stderr, "\nOptions:\n");
  fprintf(stderr, "  <destination>      dns name or ip address\n");
  fprintf(stderr, "  -c <count>         stop after <count> replies\n");
  fprintf(stderr, "  -h                 print help and exit\n");
  fprintf(stderr, "  -i <interval>      seconds between sending each packet\n");
  fprintf(stderr, "  -t <ttl>           define time to live\n");
  fprintf(stderr, "  -v                 verbose output\n");
  exit(1);
}
