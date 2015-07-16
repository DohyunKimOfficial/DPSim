#ifndef PREDICTOR_NEURAL_PREDICTOR_H_
#define PREDICTOR_NEURAL_PREDICTOR_H_

#include <cstdint>
#include <deque>
#include <vector>

#include "../parser/blk_parser.h"
#include "./predictor.h"

namespace dpsim {
const int32_t mode_arma = 0;  ///< arma model for neural network

//----------------------------------------------------------------------------
/// @brief ANN-based predictor
///
/// Simple ANN(artificial neural network)-based predictor.
/// Utilizes one-layered neural network
///
class NeuralPredictor : public Predictor {
 private:
  int32_t hidden_number;      ///< number of hidden nodes in the network
  int32_t last_sampled_time;  ///< indicates when was the last sampling
  int32_t sampling_interval;  ///< defines the time interval between
                              ///< adjacent samples
  int32_t sample_size;        ///< defines how many samples would be used
                              ///< for predictions (equals to number of
                              ///< the input nodes
  int32_t mode;               ///< which model would be used
  double interval_sum;        ///< temporary variable for summing values
                              ///< before next sampling event

  std::deque<double> samples;      ///< deque container for the last samples
  std::vector<double> ih_weights;  ///< vector container for weighted
                                   ///< edges between hidden and input nodes
  std::vector<double> ho_weights;  ///< vector container for weighted
                                   ///< edges between hidden and output nodes

 public:
  /// @param hidden_number number of hidden nodes in the network
  /// @param interval time interval between adjacent predictions
  /// @param sampling_interval time interval between adjacent samples
  /// @param sample_size number of samples used for prediction
  /// @param mode defines which model would be used
  explicit NeuralPredictor(int32_t hidden_number,
                           int32_t interval,
                           int32_t sampling_interval,
                           int32_t sample_size,
                           int32_t mode);

  /// @brief consumes one more parsed trace line
  /// @retval zero if no error occurred, otherwise if not
  /// @param parsed_trace trace struct for passing parsed details
  int consume(struct trace* parsed_trace);

  /// @brief builds simple network with given params
  ///
  /// single-layer neural network is constructed with sample_size input nodes,
  /// hidden_number hidden nodes, and one output node
  void build_simple_network();

  /// @brief train the network with the real output
  /// @param normalized real output
  void train(double expected_output);

  /// @brief forward pass the network with given inputs
  /// @retval normalized output of the network
  double forward_pass();

  /// @brief predicts the future demand with the network
  /// @retval prediction for the next prediction interval
  double predict();
};
}  // namespace dpsim

#endif  // PREDICTOR_NEURAL_PREDICTOR_H_
