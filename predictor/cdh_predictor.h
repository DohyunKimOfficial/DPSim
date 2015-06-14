#ifndef PREDICTOR_CDH_PREDICTOR_H_
#define PREDICTOR_CDH_PREDICTOR_H_

#include <cstdint>
#include <queue>
#include <map>
#include "../parser/blk_parser.h"

namespace dpsim {
class CDHPredictor: public Predictor {
 private:
  float probability;
  int32_t last_prediction_time;
  double interval_sum;
  int32_t n_wb;
  int32_t histogram_bin_width;
  int32_t histogram_size;
  int32_t total_quantity;
  std::map<int32_t, int32_t> histogram;
  std::queue<int32_t> history;

 public:
  explicit CDHPredictor(float probability,
                        int32_t interval,
                        int32_t n_wb,
                        int32_t histogram_bin_width,
                        int32_t histogram_size);
  int consume(struct trace* parsed_trace);
  int32_t predict();
};
}  // namespace dpsim

#endif  // PREDICTOR_CDH_PREDICTOR_H_
