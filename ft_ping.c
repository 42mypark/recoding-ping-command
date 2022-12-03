#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

static int interval;
static int received_count;
static int loss_count = 33;    // 0
static int total_count = 100;  // 0
static double max;
static double min;
static double ewma;  // exponential weight moving average
static double avg;
static double mdev;
static struct timeval start_time;
static struct timeval sent_time;
// static void* options[256];
// static struct sockaddr dst_network_ip;
static char dst_presentation_ip[16] = {'0', '.', '0', '.', '0', '.', '0'};

void print_result() {
  int loss_percent = (loss_count * 100 / total_count);
  int time_ms = (sent_time.tv_sec - start_time.tv_sec) * 1000 +
                (sent_time.tv_usec - start_time.tv_usec) / 1000;

  printf("\n--- %s ping statistics ---\n", dst_presentation_ip);
  printf(
      "%d packets transmitted, %d received, %d%% packet loss, time "
      "%dms\n",
      total_count, received_count, loss_percent, time_ms);
  if (received_count) {
    printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n", min, avg, max,
           mdev);
  }
}

void exit_program(int sig) {
  (void)sig;
  print_result();
  exit(1);
}

void show_info(int sig) {
  int loss_percent = (loss_count * 100 / total_count);

  (void)sig;
  printf(
      "\b\b%d/%d packets, %d%% loss, min/avg/ewma/max = %.3f/%.3f/%.3f/%.3f "
      "ms\n",
      received_count, total_count, loss_percent, min, avg, ewma, max);
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

void send_ping_recv_pong(int sig) {
  (void)sig;

  // recv_pong();
  // send_ping();
  // calc_statistics();

  gettimeofday(&sent_time, NULL);
  alarm(interval);
}

int main() {
  // parse_file();
  // parse_argument();
  interval = 1;
  gettimeofday(&start_time, NULL);

  signal(SIGALRM, send_ping_recv_pong);
  signal(SIGINT, exit_program);
  signal(SIGQUIT, show_info);

  alarm(interval);
  while (1)
    ;
}