#ifndef PREDICTOR_NEURAL_PREDICTOR_H_
#define PREDICTOR_NEURAL_PREDICTOR_H_

#include <cstdint>
#include <deque>
#include <vector>

#include "../parser/blk_parser.h"
#include "./predictor.h"

namespace dpsim {
const int32_t mode_arma = 0;

class NeuralPredictor : public Predictor {
 private:
  int32_t hidden_number;
  int32_t last_sampled_time;
  int32_t sampling_interval;
  int32_t sample_size;
  int32_t mode;
  double interval_sum;

  std::deque<double> samples;
  std::vector<double> ih_weights;
  std::vector<double> ho_weights;

 public:
  explicit NeuralPredictor(int32_t hidden_number,
                           int32_t interval,
                           int32_t sampling_interval,
                           int32_t sample_size,
                           int32_t mode);
  int consume(struct trace* parsed_trace);
  void build_simple_network();
  void train(double expected_output);
  double forward_pass();
  double predict();
};
}  // namespace dpsim

#endif  // PREDICTOR_NEURAL_PREDICTOR_H_
