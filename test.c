#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main() {
  char *dst = "8.8.8.8";
  struct sockaddr_in dst_net;
  inet_pton(AF_INET, dst, &dst_net.sin_addr);

  printf("%d\n", inet_addr(dst));
  printf("%d\n", dst_net.sin_addr);
}