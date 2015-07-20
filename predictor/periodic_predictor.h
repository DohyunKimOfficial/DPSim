#ifndef PREDICTOR_PERIODIC_PREDICTOR_H_
#define PREDICTOR_PERIODIC_PREDICTOR_H_

#include <cstdint>
#include <deque>
#include <vector>
#include "../parser/blk_parser.h"
#include "./predictor.h"

namespace dpsim {
const int32_t mode_singleperiod = 0;     ///< identifies single period in the history
const int32_t mode_singleperiod_cdh = 1; ///< same as mode_singleperiod,
                                         ///< but uses cdh for prediction

//----------------------------------------------------------------------------
/// @brief periodicity based predictor
///
/// Makes prediction based on the periodicity recognizable from the history
///
class PeriodicPredictor: public Predictor {
 private:
  int32_t last_sampled_time;   ///< indicates when was the last sampling time
  int32_t sample_size;         ///< maximum sample size of the history
  int32_t sampling_interval;   ///< time interval between adjacent samples
  int32_t mode;                ///< prediction mode
  double interval_sum;         ///< temporary variable for summing values
  std::deque<double> samples;  ///< deque container for the history

 public:
  /// @param interval time interval between adjacent predictions
  /// @param window_size number of data points to be used for prediction
  /// @param mode prediction mode
  explicit PeriodicPredictor(int32_t interval,
                             int32_t sampling_interval,
                             int32_t sample_size,
                             int32_t mode);

  /// @brief consumes one more parsed trace line
  /// @retval zero if no error occurred, otherwise if not
  /// @param parsed_trace trace struct for passing parsed details
  int consume(struct trace* parsed_trace);

  /// @brief makes a prediction
  /// @retval predicted amoung of write data in Kb
  double predict();

  /// @brief calculates autocorrelation from the samples
  /// @param container vector container of the samples, filled with
  ///                  corresponding autocorrelation values when return
  void calculate_autocorrelation(std::vector<double> *container);
};
}  // namespace dpsim

#endif  // PREDICTOR_PERIODIC_PREDICTOR_H_
