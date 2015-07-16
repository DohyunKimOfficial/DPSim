#include <cassert>
#include <cstdint>
#include <cmath>
#include <vector>

#include "../parser/blk_parser.h"
#include "./predictor.h"
#include "./neural_predictor.h"

namespace dpsim {
NeuralPredictor::NeuralPredictor(int32_t hidden_number,
                                 int32_t interval,
                                 int32_t sampling_interval,
                                 int32_t sample_size,
                                 int32_t mode)
: Predictor(interval), sampling_interval(sampling_interval),
  sample_size(sample_size), last_sampled_time(-sampling_interval),
  interval_sum(0.0), hidden_number(hidden_number) {
  switch (mode) {
    case mode_arma:
      this->mode = mode_arma;
      break;
  }

  this->build_simple_network();
}

void
NeuralPredictor::build_simple_network() {
  for (int32_t i = 0; i < this->hidden_number; i++) {
    for (int32_t j = 0; j < this->sample_size; j++) {
      ih_weights.push_back(0.0);
//    ih_weights.push_back(1.0/(this->hidden_number * this->sample_size));
    }
//  ho_weights.push_back(1.0/this->hidden_number);
    ho_weights.push_back(0.0);
  }
}

static double
normalize_value(double value) {
  return value / 100000;
}

static double
denormalize_value(double value) {
  return value * 100000;
}

double
NeuralPredictor::forward_pass() {
  assert(this->samples.size() == this->sample_size);

  std::vector<double> hiddens;
  for (int32_t i = 0; i < this->hidden_number; i++) {
    double value = 0.0;
    for (int32_t j = 0; j < this->sample_size; j++) {
      double input = this->samples[j];
      double weight = this->ih_weights[j*this->hidden_number + i];
      assert(j*this->hidden_number + i < this->ih_weights.size());
      value += input * weight;
    }
    hiddens.push_back(1.0 / (1 + exp(-value)));
  }

  double output = 0.0;
  for (int32_t i = 0; i < this->hidden_number; i++) {
    output += this->ho_weights[i] * hiddens[i];
  }
  output = 1.0 / (1 + exp(-output));

  return denormalize_value(output);
}

double
NeuralPredictor::predict() {
  if (this->samples.size() < this->sample_size) {
    return 0;
  } else {
    // forward pass
    return this->forward_pass();
  }
}

void
NeuralPredictor::train(double expected_output) {
  std::vector<double> hiddens;

  for (int32_t i = 0; i < this->hidden_number; i++) {
    double value = 0.0;
    for (int32_t j = 0; j < this->sample_size; j++) {
      double input = this->samples[j];
      double weight = this->ih_weights[j*this->hidden_number + i];
      value += input * weight;
    }
    hiddens.push_back(1.0 / (1 + exp(-value)));
  }

  double output = 0.0;
  for (int32_t i = 0; i < this->hidden_number; i++) {
    output += this->ho_weights[i] * hiddens[i];
  }
  output = 1.0 / (1 + exp(-output));

  double output_error = (expected_output - output) * \
                        output * (1 - output);

  for (int32_t i = 0; i < this->hidden_number; i++) {
    this->ho_weights[i] += output_error * hiddens[i];
    double hidden_error = this->ho_weights[i] * output_error * \
                          hiddens[i] * (1 - hiddens[i]);

    for (int32_t j = 0; j < this->sample_size; j++) {
      int32_t index = j*this->hidden_number + i;
      assert(index < this->ih_weights.size());
      this->ih_weights[index] += hidden_error * this->samples[j];
    }
  }
  std::cout << "output: " << output
            << " expected output: " << expected_output
            << " error: " << output_error << std::endl;
}

int
NeuralPredictor::consume(struct trace* parsed_trace) {
  int32_t sampling_interval_end_time = this->last_sampled_time
                                       + this->sampling_interval;

  if (parsed_trace == NULL) {
    return 0;
  } else if (sampling_interval_end_time < parsed_trace->timestamp) {
    // fast-forward time
    while (sampling_interval_end_time < parsed_trace->timestamp) {
      // train
      if (this->samples.size() == this->sample_size) {
        this->train(normalize_value(this->interval_sum));
      }

      // add sample
      if (this->last_sampled_time >= 0) {
        this->samples.push_back(normalize_value(this->interval_sum));
      }
      this->interval_sum = 0;

      if (this->sample_size != 0 && this->samples.size() > this->sample_size) {
        this->samples.pop_front();
      }

      // predict
      if (sampling_interval_end_time % Predictor::prediction_interval == 0) {
        double predicted_data_size = predict();

        struct prediction pred;
        pred.start_time = sampling_interval_end_time;
        pred.duration = Predictor::prediction_interval;
        pred.data_size = predicted_data_size;

        Predictor::predictions.push_back(pred);
      }

      this->last_sampled_time += this->sampling_interval;
      sampling_interval_end_time += this->sampling_interval;
    }

    this->interval_sum = block_to_kb(parsed_trace->block_size);
  } else if (parsed_trace->timestamp > this->last_sampled_time) {
    this->interval_sum += block_to_kb(parsed_trace->block_size);
  }

  return 0;
}

}  // namespace dpsim
