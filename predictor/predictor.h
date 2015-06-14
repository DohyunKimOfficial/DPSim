#ifndef PREDICTOR_PREDICTOR_H_
#define PREDICTOR_PREDICTOR_H_

#include <deque>
#include <string>
#include "../parser/blk_parser.h"

namespace dpsim {

struct prediction {
  int start_time;
  int duration;
  double data_size;
};

class Predictor {
 protected:
  std::deque<struct prediction> predictions;
  int prediction_interval;
 public:
  explicit Predictor(int prediction_interval);
  ~Predictor();

  virtual int consume(struct trace* parsed_trace) = 0;
  bool prediction_empty();
  struct prediction pop_prediction();
};
}  // namespace dpsim

#endif  // PREDICTOR_PREDICTOR_H_
