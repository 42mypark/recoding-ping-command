#include "ft_ping.h"

extern struct global g_;

double ft_babylonian(double a, int cnt) {
  if (cnt == 0)
    return 1.0;
  double n_1 = ft_babylonian(a, cnt - 1);
  return (n_1 * n_1 + a) / (2 * n_1);
}

void calc_statistics(double time) {
  g_.max = g_.max > time ? g_.max : time;
  g_.min = g_.min < time ? g_.min : time;

  double total_time = g_.avg * (g_.total_count - 1) + time;
  double curr_avg   = total_time / g_.total_count;
  double prev_squre_sum =
      (g_.mdev * g_.mdev + g_.avg * g_.avg) * (g_.total_count - 1);
  double variance =
      (prev_squre_sum + time * time) / g_.total_count - curr_avg * curr_avg;

  g_.avg  = curr_avg;
  g_.mdev = ft_babylonian(variance, 5);

  double beta = 0.5;
  g_.ewma     = g_.ewma * beta + (1 - beta) * time;
}