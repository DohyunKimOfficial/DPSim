#ifndef PREDICTOR_PREDICTOR_H_
#define PREDICTOR_PREDICTOR_H_

#include <deque>
#include <string>
#include "../parser/blk_parser.h"

namespace dpsim {

//----------------------------------------------------------------------------
/// @brief container for predicted details
///
/// A light-weight container for each prediction made by various predictors
///
///
struct prediction {
  int start_time;    ///< the time when this prediction has been made
  int duration;      ///< the time duration of the prediction
  double data_size;  ///< predicted write data size in Kb
};

//----------------------------------------------------------------------------
/// @brief base class for all predictors
///
/// All predictors inherit this base Predictor class
///
class Predictor {
 protected:
  std::deque<struct prediction> predictions;
  int prediction_interval;
 public:
  /// @param prediction_interval time interval between predictions
  explicit Predictor(int prediction_interval);
  ~Predictor();

  /// @brief consumes one more parsed trace line
  /// @retval zero if no error occurred, otherwise if not
  /// @param parsed_trace trace struct for passing parsed details
  virtual int consume(struct trace* parsed_trace) = 0;

  /// @brief check whether any prediction has been made before
  /// @retval true if there is none, otherwise false
  ///
  /// safe to check with this function before calling pop_prediction()
  bool prediction_empty();

  /// @brief pop the least recent prediction made by the predictor
  /// @retval prediction struct containing the least recent prediction
  struct prediction pop_prediction();
};
}  // namespace dpsim

#endif  // PREDICTOR_PREDICTOR_H_
