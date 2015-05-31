#include <deque>
#include <cstdint>
#include "../parser/blk_parser.h"
#include "./history_predictor.h"

namespace dpsim {
HistoryPredictor::HistoryPredictor(uint32_t interval,
                                   uint32_t window_size) :
  Predictor(interval), window_size(window_size),
  interval_sum(0.0), last_prediction_time(0.0) {
}

int
HistoryPredictor::consume(struct trace* parsed_trace) {
  double interval_end_time = this->last_prediction_time
                             + Predictor::prediction_interval;
  if (parsed_trace == NULL) {
  } else if (interval_end_time < parsed_trace->timestamp) {  // every interval
    double window_sum = 0.0;
    this->history.push_back(this->interval_sum);

    // if limit of window size exists
    if (this->window_size != 0 && this->history.size() > this->window_size) {
      this->history.pop_front();
    }

    std::deque<double>::iterator it;

    for (it = this->history.begin(); it != this->history.end(); it++) {
      window_sum += *it;
    }

    double interval_num = (this->history.empty()) ? 1.0 : this->history.size();

    struct prediction pred;
    pred.start_time = this->last_prediction_time;
    pred.duration = Predictor::prediction_interval;
    pred.data_size = window_sum/interval_num;

    Predictor::predictions.push_back(pred);

    this->interval_sum = 0;
    this->last_prediction_time += Predictor::prediction_interval;
  } else if (parsed_trace->timestamp > this->last_prediction_time) {
    this->interval_sum += block_to_kb(parsed_trace->block_size);
  }

  return 0;
}
}  // namespace dpsim
