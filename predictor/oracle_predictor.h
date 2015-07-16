#ifndef PREDICTOR_ORACLE_PREDICTOR_H_
#define PREDICTOR_ORACLE_PREDICTOR_H_

#include <queue>
#include "../parser/blk_parser.h"

namespace dpsim {

//----------------------------------------------------------------------------
/// @brief oracle predictor
///
/// Oracle predictor is a imaginary predictor which has no error
/// on its predictions.
///
class OraclePredictor: public Predictor {
 private:
  int last_prediction_time;  ///< indicates when was the last prediction time
  double interval_sum;       ///< temporary variable for summing values
                             ///< for next sample
 public:
  /// @param interval time interval between adjacent predictions
  explicit OraclePredictor(int interval);

  /// @brief consumes one more parsed trace line
  /// @retval zero if no error occurred, otherwise if not
  /// @param parsed_trace trace struct for passing parsed details
  int consume(struct trace* parsed_trace);
};
}  // namespace dpsim

#endif  // PREDICTOR_ORACLE_PREDICTOR_H_
