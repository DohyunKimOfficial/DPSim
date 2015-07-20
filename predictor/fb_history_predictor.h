#ifndef PREDICTOR_FB_HISTORY_PREDICTOR_H_
#define PREDICTOR_FB_HISTORY_PREDICTOR_H_

#include <cstdint>
#include <deque>
#include "../parser/blk_parser.h"
#include "./predictor.h"

namespace dpsim {
const int32_t mode_fb_exclusion = 1;  ///< exclusion-based feedback

//----------------------------------------------------------------------------
/// @brief feedbacked history predictor
///
/// Makes prediction based on the history, and uses accuracy of its recent
/// predictions as a feedback
///
class FBHistoryPredictor: public Predictor {
 private:
  int32_t last_prediction_time;  ///< indicates when was the last
                                 ///< prediction time
  int32_t window_size;           ///< window size of the history
  int32_t window_loc;            ///< window location on the history
                                 ///< (for exclusion algorithms)
  int32_t mode;                  ///< prediction mode
  double interval_sum;           ///< temporary variable for summing values
                                 ///< for next sample
  double upper_bound;            ///< over-prediction threshold (0.0 to 1.0)
  double lower_bound;            ///< under-prediction threshold (0.0 to 1.0)
  std::deque<double> history;    ///< deque container for the history

 public:
  /// @param interval time interval between adjacent predictions
  /// @param window_size number of data points to be used for prediction
  /// @param mode prediction mode
  /// @param upper_bound over-prediction threshold
  //                     for the evaluation of accuracy
  /// @param lower_bound under-prediction threshold
  //                     for the evaluation of accuracy
  explicit FBHistoryPredictor(int32_t interval,
                              int32_t window_size,
                              int32_t mode,
                              double upper_bound,
                              double lower_bound);

  /// @brief consumes one more parsed trace line
  /// @retval zero if no error occurred, otherwise if not
  /// @param parsed_trace trace struct for passing parsed details
  int consume(struct trace* parsed_trace);

  /// @brief makes a prediction
  /// @retval predicted amount of write data in Kb
  double predict();
};
}  // namespace dpsim

#endif  // PREDICTOR_FB_HISTORY_PREDICTOR_H_
