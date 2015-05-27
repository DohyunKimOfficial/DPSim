#include <deque>
#include <stack>
#include "../parser/blk_parser.h"
#include "./predictor.h"
#include "./oracle_predictor.h"

namespace dpsim {
  OraclePredictor::OraclePredictor(int prediction_interval)
  : Predictor(prediction_interval), last_prediction_time(0), interval_sum(0) {
  }

  int
  OraclePredictor::consume(struct trace* parsed_trace) {
    if (parsed_trace == NULL) {
      struct prediction pred;
      pred.start_time = this->last_prediction_time;
      pred.duration = Predictor::prediction_interval;
      pred.data_size = block_to_kb(this->interval_sum);

      Predictor::predictions.push_back(pred);
    } else if (this->last_prediction_time > parsed_trace->timestamp) {
      std::stack<struct prediction> temp_storage;
      struct prediction temp_pred;

      while (!Predictor::prediction_empty()) {
        temp_pred = Predictor::predictions.back();
        Predictor::predictions.pop_back();

        double timestamp = parsed_trace->timestamp;

        if (temp_pred.start_time <= timestamp &&
            temp_pred.start_time + temp_pred.duration >= timestamp) {
          temp_pred.data_size += block_to_kb(parsed_trace->block_size);
          temp_storage.push(temp_pred);
          break;
        } else {
          temp_storage.push(temp_pred);
        }
      }

      while (!temp_storage.empty()) {
        Predictor::predictions.push_back(temp_storage.top());
        temp_storage.pop();
      }
    } else {
      double interval_end_time = this->last_prediction_time
                                 + Predictor::prediction_interval;
      if (interval_end_time < parsed_trace->timestamp) {
        // initialize prediction structure
        struct prediction pred;
        pred.start_time = this->last_prediction_time;
        pred.duration = Predictor::prediction_interval;
        pred.data_size = block_to_kb(this->interval_sum);

        // push prediction to queue
        Predictor::predictions.push_back(pred);

        // start new cycle
        this->last_prediction_time += Predictor::prediction_interval;
        this->interval_sum = 0.0;
      }

      this->interval_sum += parsed_trace->block_size;
    }

    return 0;
  }
}  // namespace dpsim
