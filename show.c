#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

#include "ft_ping.h"

extern struct global g_;

static void show_result() {
  int loss_percent = (g_.loss_count * 100 / g_.total_count);
  int time_ms = (g_.sent_time.tv_sec - g_.start_time.tv_sec) * 1000 +
                (g_.sent_time.tv_usec - g_.start_time.tv_usec) / 1000;

  printf("\n--- %s ping statistics ---\n", (char *)g_.options[0]);
  printf(
      "%d packets transmitted, %d received, %d%% packet loss, time "
      "%dms\n",
      g_.total_count, g_.received_count, loss_percent, time_ms);
  if (g_.received_count) {
    printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n", g_.min, g_.avg,
           g_.max, g_.mdev);
  }

  printf("%d\n", g_.loss_count);
}

void exit_program(int sig) {
  (void)sig;
  show_result();
  exit(1);
}

void show_info(int sig) {
  int loss_percent = (g_.loss_count * 100 / g_.total_count);

  (void)sig;
  printf(
      "\b\b%d/%d packets, %d%% loss, min/avg/ewma/max = %.3f/%.3f/%.3f/%.3f "
      "ms\n",
      g_.received_count, g_.total_count, loss_percent, g_.min, g_.avg, g_.ewma,
      g_.max);
}

void show_help() {
  printf("\nUsage\n");
  printf("  ping [options] <destination>\n");
  printf("\nOptions:\n");
  printf("  <destination>      dns name or ip address\n");
  printf("  -h                 print help and exit\n");
  printf("  -v                 verbose output\n");
  exit(1);
}
