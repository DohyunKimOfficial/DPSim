#ifndef PREDICTOR_PERIODIC_PREDICTOR_H_
#define PREDICTOR_PERIODIC_PREDICTOR_H_

#include <cstdint>
#include <deque>
#include <vector>
#include "../parser/blk_parser.h"
#include "./predictor.h"

namespace dpsim {
const int32_t mode_singleperiod = 0;
const int32_t mode_singleperiod_cdh = 1;

class PeriodicPredictor: public Predictor {
 private:
  int32_t last_sampled_time;
  int32_t sample_size;
  int32_t sampling_interval;
  int32_t mode;
  double interval_sum;
  std::deque<double> samples;

 public:
  explicit PeriodicPredictor(int32_t interval,
                             int32_t sampling_interval,
                             int32_t sample_size,
                             int32_t mode);
  int consume(struct trace* parsed_trace);
  double predict();
  void calculate_autocorrelation(std::vector<double> *container);
};
}  // namespace dpsim

#endif  // PREDICTOR_PERIODIC_PREDICTOR_H_
