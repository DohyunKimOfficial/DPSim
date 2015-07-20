#ifndef PREDICTOR_CDH_PREDICTOR_H_
#define PREDICTOR_CDH_PREDICTOR_H_

#include <cstdint>
#include <queue>
#include <map>
#include "../parser/blk_parser.h"

namespace dpsim {
//----------------------------------------------------------------------------
/// @brief CDH predictor
///
/// CDH(Cumulative density histogram) predictor
/// utilizes CDH for its predictions.
/// n_wb multiplied by interval equals to t_expire.
///
class CDHPredictor: public Predictor {
 private:
  float probability;            ///< target probability
  int32_t last_prediction_time; ///< indicates when was the last
                                ///< prediction time
  double interval_sum;          ///< temporary variable for summing values
  int32_t n_wb;                 ///< number of prediction intervals per t_expire
  int32_t histogram_bin_width;  ///< bin width of histogram in Kb
  int32_t histogram_size;       ///< maximum size of stored histogram
  int32_t total_quantity;       ///< current size of histogram
  std::map<int32_t, int32_t> histogram; ///< map container for the history
  std::queue<int32_t> history;  ///< deque container for the history

 public:
  /// @param probability p-value for prediction
  /// @param interval time interval between adjacent predictions
  /// @param n_wb number of prediction intervals per t_expire.
  /// @param histogram_bin_width bin width of histogram in Kb
  /// @param histogram_size size of a histogram. zero if unlimited.
  explicit CDHPredictor(float probability,
                        int32_t interval,
                        int32_t n_wb,
                        int32_t histogram_bin_width,
                        int32_t histogram_size);

  /// @brief consumes one more parsed trace line
  /// @retval zero if no error occurred, otherwise if not
  /// @param parsed_trace trace struct for passing parsed details
  int consume(struct trace* parsed_trace);

  /// @brief makes a prediction
  /// @retval predicted amount of write data in Kb
  int32_t predict();
};
}  // namespace dpsim

#endif  // PREDICTOR_CDH_PREDICTOR_H_
