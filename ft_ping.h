#ifndef FT_PING_H
#define FT_PING_H
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#define BUFFER_SIZE 84  // ?

struct global {
  int                sockfd;
  int                interval;
  int                received_count;
  int                loss_count;
  int                total_count;
  int                error_count;
  double             max;
  double             min;
  double             ewma;  // exponential weight moving average
  double             avg;
  double             mdev;
  struct timeval     start_time;
  struct timeval     sent_time;
  void*              options[128];
  const char*        domain_name;
  struct sockaddr_in dst_ip;
};

uint16_t ft_htons(uint16_t host);
void     recv_ping();
void     send_ping();
void     exit_program(int sig);
void     show_info(int sig);
void     show_help();
void     parse_arguments(int argc, char** argv);
void     calc_statistics(double time);
void     fatal_error_check(int condition, char* msg);

enum icmp_error_type {
  DESTINATION_UNREACHABLE = 3,
  REDIRECT                = 5,
  TIME_EXCEEDED           = 11,
  PARAMETER_PROBLEM       = 12,
};

#endif