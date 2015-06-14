#include <cstdint>
#include <deque>
#include <map>
#include <cmath>

#include "../parser/blk_parser.h"
#include "./predictor.h"
#include "./cdh_predictor.h"

namespace dpsim {
CDHPredictor::CDHPredictor(float probability,
                           int32_t interval,
                           int32_t n_wb,
                           int32_t histogram_bin_width,
                           int32_t histogram_size) :
  Predictor(interval), probability(probability),
  histogram_bin_width(histogram_bin_width), histogram_size(histogram_size),
  n_wb(n_wb), last_prediction_time(-interval), interval_sum(0),
  total_quantity(0) {
}

int
CDHPredictor::consume(struct trace* parsed_trace) {
  double interval_end_time = this->last_prediction_time
                             + Predictor::prediction_interval;

  if (parsed_trace == NULL) {
    return 0;
  } else if (interval_end_time < parsed_trace->timestamp) {  // every t_expire
    // fast-forward time
    while (interval_end_time < parsed_trace->timestamp) {
      // add histogram
      int32_t interval = static_cast<int32_t>(interval_sum /
                                              this->histogram_bin_width);
      this->interval_sum = 0;

      if (this->last_prediction_time >= 0) {
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
      }

      int32_t predicted_data_size = predict();

      for (int32_t i = 0; i < this->n_wb; i++) {
        // initialize prediction structure
        struct prediction pred;
        pred.start_time = interval_end_time
                          + i * Predictor::prediction_interval / this->n_wb;
        pred.duration = Predictor::prediction_interval;
        pred.data_size = static_cast<double>(predicted_data_size)/this->n_wb;

        Predictor::predictions.push_back(pred);
      }

      // proceed interval
      this->last_prediction_time += Predictor::prediction_interval;
      interval_end_time += Predictor::prediction_interval;
    }

    // start new cycle
    this->interval_sum = block_to_kb(parsed_trace->block_size);
  } else if (parsed_trace->timestamp > this->last_prediction_time) {
      this->interval_sum += block_to_kb(parsed_trace->block_size);
  }
}

int32_t
CDHPredictor::predict() {
  if (this->histogram.empty()) {
    return 0;
  } else {
    int32_t size = this->total_quantity;
    float boundary = ceil(size*this->probability);
    int32_t boundary_quantity = static_cast<int32_t>(boundary);
    std::map<int32_t, int32_t>::iterator it;

    int32_t prediction = 0;
    int32_t cumulated_quantity = 0;

    for (it = this->histogram.begin(); it != histogram.end(); it++) {
      // add frequency
      cumulated_quantity += it->second;

      if (cumulated_quantity >= boundary_quantity) {
        prediction = it->first;
        break;
      }
    }

    return (prediction + 1) * histogram_bin_width;
  }
}
}  // namespace dpsim
