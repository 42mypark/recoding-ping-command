#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>

#include "ft_ping.h"
#include "libft.h"

extern struct global g_;

// static void show_options() {  // FOR DEBUG
//   for (int i = 'A'; i <= 'z'; ++i) {
//     if (ft_isalpha(i) && g_.options[i]) {
//       if (i == 'i' || i == 'c')
//         printf("%c %d ", i, ft_atoi(g_.options[i]));
//       else
//         printf("%c %d ", i, (int)(long long)g_.options[i]);
//     }
//   }
//   printf("\n");
// }

static int arg_mux(char* arg, char* next) {
  int switch_option;
  int param_option;
  int last_option;

  if (arg[0] != '-') {
    g_.options[0] = arg;
    return 0;
  }
  for (int i = 1; arg[i]; ++i) {
    switch_option = ft_strchri("hv", arg[i]) != -1;
    param_option  = ft_strchri("cit", arg[i]) != -1;
    last_option   = arg[i + 1] == '\0';

    if (switch_option) {
      g_.options[(int)arg[i]] = (void*)1;
    } else if (param_option && !last_option) {
      g_.options[(int)arg[i]] = arg + i + 1;
      return 0;
    } else if (param_option && last_option) {
      g_.options[(int)arg[i]] = next;
      return 1;
    } else {
      g_.options[1] = (void*)(long long)arg[i];
      break;
    }
  }
  return 0;
}

static void check_options() {
  if (g_.options[1]) {
    fprintf(stderr, "ping: invalid option -- '%c'",
            (char)(long long)g_.options[1]);
    show_help();
  } else if (g_.options[(int)'h']) {
    show_help();
  }
  if (g_.options[(int)'i'] == NULL) {
    g_.options[(int)'i'] = "1";
  }
  if (g_.options[(int)'t'] == NULL) {
    g_.options[(int)'t'] = "64";
  }
  if (g_.options[(int)'c'] == NULL) {
    g_.options[(int)'c'] = "-1";
  }
}

static void get_dst_ip(const char* dst) {
  struct sockaddr_in* in;
  struct addrinfo*    res;
  struct addrinfo     hints;
  int                 error;
  int                 is_ip;

  is_ip = inet_pton(AF_INET, dst, &g_.dst_ip.sin_addr);

  error = EAI_AGAIN;
  if (!is_ip) {
    ft_memset(&hints, 0, sizeof(hints));
    hints.ai_family    = AF_UNSPEC; /* Allow IPv4 or IPv6 */
    hints.ai_socktype  = SOCK_RAW;  /* Datagram socket */
    hints.ai_protocol  = 0;         /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr      = NULL;
    hints.ai_next      = NULL;
    hints.ai_flags = AI_PASSIVE | AI_CANONNAME; /* For wildcard IP address */
    while (error == EAI_AGAIN)
      error = getaddrinfo(dst, NULL, &hints, &res);
    if (error == EAI_NONAME) {
      fprintf(stderr, "ping: %s: Name of service not know\n", dst);
      exit(1);
    }
    fatal_error_check(error, "getaddrinfo");
    in             = (struct sockaddr_in*)res->ai_addr;
    g_.dst_ip      = *in;
    g_.domain_name = dst;
    freeaddrinfo(res);
  }
}

void parse_arguments(int argc, char** argv) {
  for (int i = 1; i < argc; ++i)
    i += arg_mux(argv[i], argv[i + 1]);
  check_options();
  if (g_.options[0])
    get_dst_ip(g_.options[0]);
}