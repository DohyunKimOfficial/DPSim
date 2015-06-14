#include <deque>
#include <cstdint>
#include "../parser/blk_parser.h"
#include "./history_predictor.h"

namespace dpsim {
HistoryPredictor::HistoryPredictor(int32_t interval,
                                   int32_t window_size,
                                   int32_t mode) :
  Predictor(interval), window_size(window_size),
  interval_sum(0.0), last_prediction_time(-interval) {
  switch (mode) {
    case mode_max:
      this->mode = mode_max;
      break;
    case mode_avg:
      this->mode = mode_avg;
      break;
    default:
      break;
  }
}

int
HistoryPredictor::consume(struct trace* parsed_trace) {
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
HistoryPredictor::predict() {
  if (this->mode == mode_avg) {
    double window_sum = 0;

    std::deque<double>::iterator it;

    for (it = this->history.begin(); it != this->history.end(); it++) {
      window_sum += *it;
    }

    double interval_num = (this->history.empty()) ? 1.0 : this->history.size();

    return window_sum/interval_num;
  } else if (this->mode == mode_max) {
    double result = 0;

    std::deque<double>::iterator it;

    for (it = this->history.begin(); it != this->history.end(); it++) {
      result = (*it > result) ? *it : result;
    }

    return result;
  }
}
}  // namespace dpsim
