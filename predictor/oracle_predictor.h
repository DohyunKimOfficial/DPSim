#ifndef PREDICTOR_ORACLE_PREDICTOR_H_
#define PREDICTOR_ORACLE_PREDICTOR_H_

#include <queue>
#include "../parser/blk_parser.h"

namespace dpsim {
class OraclePredictor: public Predictor {
 private:
  int last_prediction_time;
  double interval_sum;
 public:
  explicit OraclePredictor(int interval);
  int consume(struct trace* parsed_trace);
};
}

#endif  // PREDICTOR_ORACLE_PREDICTOR_H_
