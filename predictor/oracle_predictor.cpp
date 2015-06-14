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
    double interval_end_time = this->last_prediction_time
                               + Predictor::prediction_interval;
    if (parsed_trace == NULL) {
      struct prediction pred;
      pred.start_time = this->last_prediction_time;
      pred.duration = Predictor::prediction_interval;
      pred.data_size = this->interval_sum;

      Predictor::predictions.push_back(pred);
    } else if (interval_end_time < parsed_trace->timestamp) {  // every t
      while (interval_end_time < parsed_trace->timestamp) {
        // initialize prediction structure
        struct prediction pred;
        pred.start_time = this->last_prediction_time;
        pred.duration = Predictor::prediction_interval;
        pred.data_size = this->interval_sum;

        // initialize interval sum
        this->interval_sum = 0.0;

        // push prediction to queue
        Predictor::predictions.push_back(pred);

        this->last_prediction_time += Predictor::prediction_interval;
        interval_end_time += Predictor::prediction_interval;
      }

      this->interval_sum = block_to_kb(parsed_trace->block_size);
    } else if (parsed_trace->timestamp >= this->last_prediction_time) {
      this->interval_sum += block_to_kb(parsed_trace->block_size);
    }
  }
}  // namespace dpsim
