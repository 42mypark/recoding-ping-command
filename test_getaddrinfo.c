#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
// #include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>

void test_getaddrinfo() {
  struct addrinfo* res;
  struct addrinfo* rp;
  int error = getaddrinfo("127.0.0.1", "80", NULL, &res);

  for (rp = res; rp; rp = rp->ai_next) {
    printf("getaddrinfo:\n");
    printf("\t ai_flags   : %d\n", rp->ai_flags);
    printf("\t ai_family  : %d\n", rp->ai_family);
    printf("\t ai_socktype: %d\n", rp->ai_socktype);
    printf("\t ai_protocol: %d\n", rp->ai_protocol);
    printf("\t ai_addrlen : %d\n", rp->ai_addrlen);
    printf("\t ai_addr:\n");
    printf("\t\t sa_family: %d\n", rp->ai_addr->sa_family);
    printf("\t\t sa_data  : %s\n", rp->ai_addr->sa_data);

    char ip_toshow[1000] = {0};
    struct sockaddr_in* in = (struct sockaddr_in*)rp->ai_addr;
    inet_ntop(AF_INET, &in->sin_addr, ip_toshow, 1000);
    printf("\t\t IPv4     : %s\n", ip_toshow);

    printf("\t ai_canonname:%s\n", rp->ai_canonname);
    printf("\t ai_next:%p\n", rp->ai_next);
  }
  freeaddrinfo(res);
}

int main() { test_getaddrinfo(); }