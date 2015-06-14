#ifndef PREDICTOR_HISTORY_PREDICTOR_H_
#define PREDICTOR_HISTORY_PREDICTOR_H_

#include <cstdint>
#include <deque>
#include "../parser/blk_parser.h"
#include "./predictor.h"

namespace dpsim {
const int32_t mode_max = 1;
const int32_t mode_avg = 2;

class HistoryPredictor: public Predictor {
 private:
  int32_t last_prediction_time;
  int32_t window_size;
  int32_t mode;
  double interval_sum;
  std::deque<double> history;

 public:
  explicit HistoryPredictor(int32_t interval,
                            int32_t window_size,
                            int32_t mode);
  int consume(struct trace* parsed_trace);
  double predict();
};
}  // namespace dpsim

#endif  // PREDICTOR_HISTORY_PREDICTOR_H_
