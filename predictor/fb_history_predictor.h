#ifndef PREDICTOR_FB_HISTORY_PREDICTOR_H_
#define PREDICTOR_FB_HISTORY_PREDICTOR_H_

#include <cstdint>
#include <deque>
#include "../parser/blk_parser.h"
#include "./predictor.h"

namespace dpsim {
const int32_t mode_fb_exclusion = 1;
class FBHistoryPredictor: public Predictor {
 private:
  int32_t last_prediction_time;
  int32_t window_size;
  int32_t window_loc;
  int32_t mode;
  double interval_sum;
  double upper_bound;
  double lower_bound;
  std::deque<double> history;

 public:
  explicit FBHistoryPredictor(int32_t interval,
                              int32_t window_size,
                              int32_t mode,
                              double upper_bound,
                              double lower_bound);
  int consume(struct trace* parsed_trace);
  double predict();
};
}  // namespace dpsim

#endif  // PREDICTOR_FB_HISTORY_PREDICTOR_H_
