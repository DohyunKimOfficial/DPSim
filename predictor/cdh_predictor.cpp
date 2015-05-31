#include <cstdint>
#include <deque>
#include <map>
#include <cmath>

#include "../parser/blk_parser.h"
#include "./predictor.h"
#include "./cdh_predictor.h"

namespace dpsim {
CDHPredictor::CDHPredictor(float probability,
                           uint32_t t_expire,
                           uint32_t n_wb,
                           uint32_t histogram_bin_width,
                           uint32_t histogram_size) :
  Predictor(t_expire/n_wb), probability(probability), t_expire(t_expire),
  histogram_bin_width(histogram_bin_width), histogram_size(histogram_size),
  n_wb(n_wb), last_prediction_time(0), interval_sum(0), total_quantity(0) {
}

int
CDHPredictor::consume(struct trace* parsed_trace) {
  double expiration_time = this->last_prediction_time + this->t_expire;

  if (parsed_trace == NULL) {
    return 0;
  } else if (Predictor::predictions.empty()) {
    for (uint32_t i = 0; i < this->n_wb; i++) {
      struct prediction pred;
      pred.start_time = this->last_prediction_time
                        + i * this->t_expire / this->n_wb;
      pred.duration = this->t_expire / this->n_wb;
      pred.data_size = 0.0;

      Predictor::predictions.push_back(pred);
    }
  } else if (expiration_time < parsed_trace->timestamp) {  // every t_expire
    // add histogram
    uint32_t interval = static_cast<uint32_t>(interval_sum /
                                              this->histogram_bin_width);

    this->histogram[interval]++;
    this->total_quantity++;

    // if limit of histogram size exists
    if (this->histogram_size != 0) {
      // register history
      this->history.push(interval);

      // if history overflow, delete frequency
      if (this->history.size() > this->histogram_size) {
        interval = this->history.front();
        this->history.pop();
        this->histogram[interval]--;
        this->total_quantity--;
      }
    }

    uint32_t predicted_data_size = predict();

    for (uint32_t i = 0; i < this->n_wb; i++) {
      // initialize prediction structure
      struct prediction pred;
      pred.start_time = expiration_time
                        + i * this->t_expire / this->n_wb;
      pred.duration = Predictor::prediction_interval;
      pred.data_size = static_cast<double>(this->predict())/this->n_wb;

      Predictor::predictions.push_back(pred);
    }

    this->last_prediction_time += this->t_expire;
    this->interval_sum = 0.0;
  } else if (parsed_trace->timestamp > this->last_prediction_time) {
      this->interval_sum += block_to_kb(parsed_trace->block_size);
  }
}

uint32_t CDHPredictor::predict() {
  uint32_t size = this->total_quantity;
  float boundary = ceil(size*this->probability);
  uint32_t boundary_quantity = static_cast<uint32_t>(boundary);
  std::map<uint32_t, uint32_t>::iterator it;

  uint32_t prediction = 0;
  uint32_t cumulated_quantity = 0;

  for (it = this->histogram.begin(); it != histogram.end(); it++) {
    // add frequency
    cumulated_quantity += it->second;

    if (cumulated_quantity >= boundary_quantity) {
      prediction = it->first;
      break;
    }
  }

  return prediction * histogram_bin_width;
}
}  // namespace dpsim
