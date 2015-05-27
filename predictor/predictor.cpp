#include <deque>
#include "../parser/blk_parser.h"
#include "./predictor.h"

namespace dpsim {
Predictor::Predictor(int prediction_interval)
: prediction_interval(prediction_interval) {}

Predictor::~Predictor() {
  while (!this->predictions.empty()) {
    this->predictions.pop_front();
  }
}

bool
Predictor::prediction_empty() {
  return this->predictions.empty();
}

struct prediction
Predictor::pop_prediction() {
  struct prediction popped_prediction = this->predictions.front();
  this->predictions.pop_front();

  return popped_prediction;
}
}  // namespace dpsim
