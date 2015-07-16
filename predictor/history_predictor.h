#ifndef PREDICTOR_HISTORY_PREDICTOR_H_
#define PREDICTOR_HISTORY_PREDICTOR_H_

#include <cstdint>
#include <deque>
#include "../parser/blk_parser.h"
#include "./predictor.h"

namespace dpsim {
const int32_t mode_max = 1;  ///< getting max from the history
const int32_t mode_avg = 2;  ///< getting average from the history

//----------------------------------------------------------------------------
/// @brief history predictor
///
/// Makes prediction based on the history
///
class HistoryPredictor: public Predictor {
 private:
  int32_t last_prediction_time;  ///< indicates when was the last
                                 ///< prediction time
  int32_t window_size;           ///< window size of the history
  int32_t mode;                  ///< prediction mode
  double interval_sum;           ///< temporary variable for summing values
                                 ///< for next sample
  std::deque<double> history;    ///< deque container for the history

 public:
  /// @param interval time interval between adjacent predictions
  /// @param window_size number of data points to be used for prediction
  /// @param mode prediction mode
  explicit HistoryPredictor(int32_t interval,
                            int32_t window_size,
                            int32_t mode);

  /// @brief consumes one more parsed trace line
  /// @retval zeo if no error occurred, otherwise if not
  /// @param parsed_trace trace struct for passing parsed details
  int consume(struct trace* parsed_trace);
  double predict();
};
}  // namespace dpsim

#endif  // PREDICTOR_HISTORY_PREDICTOR_H_
