#ifndef FT_PING_H
#define FT_PING_H
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>

struct global {
  int interval;
  int received_count;
  int loss_count;
  int total_count;
  double max;
  double min;
  double ewma;  // exponential weight moving average
  double avg;
  double mdev;
  struct timeval start_time;
  struct timeval sent_time;
  void* options[128];
  struct sockaddr_in dst_ip;
  char dst_ip_p[16];
};

void exit_program(int sig);
void show_info(int sig);
void show_help();
void parse_arguments(int argc, char** argv);

#endif