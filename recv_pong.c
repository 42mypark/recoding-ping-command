#include "ft_ping.h"

extern struct global g_;

void recv_pong() {
  g_.received_count++;
  g_.loss_count++;
}