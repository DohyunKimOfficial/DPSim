#include <algorithm>
#include <deque>
#include <cstdint>
#include <vector>
#include "../parser/blk_parser.h"
#include "./fb_history_predictor.h"

#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))

namespace dpsim {
FBHistoryPredictor::FBHistoryPredictor(int32_t interval,
                                       int32_t window_size,
                                       int32_t mode,
                                       double upper_bound,
                                       double lower_bound) :
  Predictor(interval), window_size(window_size),
  upper_bound(upper_bound), lower_bound(lower_bound),
  interval_sum(0.0), last_prediction_time(-interval),
  window_loc(0) {
  switch (mode) {
    case mode_fb_exclusion:
      this->mode = mode_fb_exclusion;
      break;
    default:
      break;
  }
}

int
FBHistoryPredictor::consume(struct trace* parsed_trace) {
  int32_t interval_end_time = this->last_prediction_time
                               + Predictor::prediction_interval;
  if (parsed_trace == NULL) {
  } else if (interval_end_time < parsed_trace->timestamp) {  // every interval
    // fast-forward time
    while (interval_end_time < parsed_trace->timestamp) {
      // current time slot data size
      if (this->last_prediction_time >= 0) {
        this->history.push_back(this->interval_sum);
      }
      this->interval_sum = 0;

      // if limit of window size exists
      if (this->window_size != 0 && this->history.size() > this->window_size) {
        this->history.pop_front();
      }

      // add prediction of current interval
      // [last_prediction_time, interval_end_time]
      struct prediction pred;
      pred.start_time = interval_end_time;
      pred.duration = Predictor::prediction_interval;
      pred.data_size = predict();

      Predictor::predictions.push_back(pred);

      // proceed interval
      this->last_prediction_time += Predictor::prediction_interval;
      interval_end_time += Predictor::prediction_interval;
    }

    // start new cycle
    this->interval_sum = block_to_kb(parsed_trace->block_size);
  } else if (parsed_trace->timestamp > this->last_prediction_time) {
    this->interval_sum += block_to_kb(parsed_trace->block_size);
  }

  return 0;
}

double
FBHistoryPredictor::predict() {
  if (this->mode == mode_fb_exclusion) {
    double window_sum = 0;

    if (this->history.size() < this->window_size ||
        Predictor::predictions.size() < this->window_size) {
      this->window_loc = 0;

      std::deque<double>::iterator history_it;

      for (history_it = this->history.begin();
           history_it != this->history.end();
           history_it++) {
        window_sum += *history_it;
      }

      double interval_num = (this->history.empty()) ?
                            1.0 : this->history.size();

      return window_sum/interval_num;
    } else {
      // evaluation = -1: low prediction
      // evaluation = 0: accurate prediction
      // evaluation = 1: high prediction
      int32_t evaluation = 0;

      double pred = (*Predictor::predictions.rbegin()).data_size;
      double real = *this->history.rbegin();

      if (real == 0.0 && pred == 0.0) {
        evaluation = 0;
      } else if (real == 0.0) {
        evaluation = 1;
      } else {
        double error_rate = (pred - real) / real;
        if (error_rate <= -this->lower_bound) {
          evaluation = -1;
        } else if (error_rate >= this->upper_bound) {
          evaluation = 1;
        }
      }

      if (evaluation == -1) {  // calibrate under-prediction
        this->window_loc = MIN(this->window_size - 1, this->window_loc + 1);
      } else if (evaluation == 1) {  // calibrate over-prediction
        this->window_loc = MAX(-this->window_size + 1, this->window_loc - 1);
      }
    }

    std::deque<double>::iterator history_it;
    std::vector<double> history_vector;

    for (history_it = this->history.begin();
         history_it != this->history.end();
         history_it++) {
      history_vector.push_back(*history_it);
    }
    std::sort(history_vector.begin(), history_vector.end());

    int32_t start_idx = (this->window_loc >= 0) ?
                        this->window_loc : 0;
    int32_t end_idx = (this->window_loc <= 0) ?
                      this->window_size + this->window_loc : this->window_size;

    double interval_num = end_idx - start_idx;

    for (int32_t i = start_idx; i < end_idx; i++) {
      window_sum += history_vector.at(i);
    }

    return window_sum/interval_num;
  }
}
}  // namespace dpsim
