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
  uint32_t last_prediction_time;
  double interval_sum;
  uint32_t n_wb;
  uint32_t t_expire;
  uint32_t histogram_bin_width;
  uint32_t histogram_size;
  uint32_t total_quantity;
  std::map<uint32_t, uint32_t> histogram;
  std::queue<uint32_t> history;

 public:
  explicit CDHPredictor(float probability,
                        uint32_t t_expire,
                        uint32_t n_wb,
                        uint32_t histogram_bin_width,
                        uint32_t histogram_size);
  int consume(struct trace* parsed_trace);
  uint32_t predict();
};
}  // namespace dpsim

#endif  // PREDICTOR_CDH_PREDICTOR_H_
