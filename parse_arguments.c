#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>

#include "ft_ping.h"
#include "libft.h"

// TODO check id!

extern struct global g_;

// static void show_options() {  // FOR DEBUG
//   for (int i = 'A'; i <= 'z'; ++i) {
//     if (ft_isalpha(i) && g_.options[i]) {
//       if (i == 'i' || i == 'c')
//         printf("%c %x ", i, ft_atoi(g_.options[i]));
//       else
//         printf("%c %x ", i, (int)(long long)g_.options[i]);
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
      g_.options[(int)arg[i]] = next ? next : (void*)-1;
      return 1;
    } else {
      g_.options[1] = (void*)(long long)arg[i];
      break;
    }
  }
  return 0;
}

static long long number_conversion(char* s, int* error) {
  unsigned long long size;
  unsigned long long prev;
  long long          sign = 1;

  if (*s == '-') {
    sign = -1;
    s++;
  }

  size = 0;
  while (*s) {
    if (!ft_isdigit(*s)) {
      *error = -1;
      return 0;
    }
    prev = size;
    size = size * 10 + *s - '0';
    if (prev > (unsigned long long)(__LONG_MAX__ - (*s - '0')) / 10) {
      *error = -2;
      return 0;
    }
    s++;
  }
  return (long long)size * sign;
}

static void number_validation(char* arg, int error) {
  if (error == -1) {
    fprintf(stderr, "ping: invalid argument: '%s'\n", arg);
    exit(1);
  } else if (error == -2) {
    fprintf(stderr,
            "ping: invalid argument: '%s': Numerical result out of range\n",
            arg);
    exit(1);
  }
}

static void check_invalid_option() {
  if (g_.options[1]) {
    fprintf(stderr, "ping: invalid option -- '%c'",
            (char)(long long)g_.options[1]);
    show_help();
  } else if (g_.options[(int)'h']) {
    show_help();
  }
}

static void check_option_argument_c(long long number) {
  if (number < 1) {
    fprintf(stderr,
            "ping: invalid argument: '%s': out of range: 1 <= value <= "
            "9223372036854775807\n",
            (char*)g_.options[(int)'c']);
    exit(1);
  }
}

static void check_option_argument_i(long long number) {
  if (number < 1 && getuid() != 0) {
    fprintf(stderr,
            "ping: cannot flood; minimal interval allowed for user is 200ms\n");
    exit(1);
  }
}

static void check_option_argument_t(long long number) {
  if (number < 0) {
    fprintf(stderr,
            "ping: invalid argument: '%s': out of range: 0 <= value <= 255\n",
            (char*)g_.options[(int)'t']);
    exit(1);
  } else if (number == 0) {
    fprintf(stderr,
            "ping: cannot set unicast time-to-live: Invalid argument\n");
    exit(1);
  }
}

static void check_option_arguments() {
  int       error;
  long long number;
  char      s[]                        = "cit";
  void (*check_argument[3])(long long) = {
      check_option_argument_c,
      check_option_argument_i,
      check_option_argument_t,
  };

  for (int i = 0; s[i]; ++i) {
    if (g_.options[(int)s[i]] == (void*)-1) {
      fprintf(stderr, "ping: option requires an argument -- '%c'", s[i]);
      show_help();
    }
    number = number_conversion(g_.options[(int)s[i]], &error);
    check_argument[i](number);
    number_validation(g_.options[(int)s[i]], error);
    g_.options[(int)s[i]] = (void*)number;
  }
}

static void check_options() {
  // show_options();
  check_invalid_option();
  check_option_arguments();
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
  g_.options[(int)'i'] = "1";
  g_.options[(int)'t'] = "64";
  g_.options[(int)'c'] = "9223372036854775807";

  for (int i = 1; i < argc; ++i)
    i += arg_mux(argv[i], argv[i + 1]);

  check_options();

  if (g_.options[0])
    get_dst_ip(g_.options[0]);
  else {
    fprintf(stderr, "ping: usage error: Destination address required\n");
    exit(1);
  }
}