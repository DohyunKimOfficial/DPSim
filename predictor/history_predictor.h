#ifndef PREDICTOR_HISTORY_PREDICTOR_H_
#define PREDICTOR_HISTORY_PREDICTOR_H_

#include <cstdint>
#include <deque>
#include "../parser/blk_parser.h"
#include "./predictor.h"

namespace dpsim {
class HistoryPredictor: public Predictor {
 private:
  uint32_t last_prediction_time;
  uint32_t window_size;
  double interval_sum;
  std::deque<double> history;

 public:
  explicit HistoryPredictor(uint32_t interval,
                            uint32_t window_size);
  int consume(struct trace* parsed_trace);
};
}  // namespace dpsim

#endif  // PREDICTOR_HISTORY_PREDICTOR_H_
