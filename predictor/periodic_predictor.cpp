#include <cassert>
#include <cmath>
#include <cstdint>
#include <deque>
#include <vector>
#include <map>

#include "../parser/blk_parser.h"
#include "./predictor.h"
#include "./periodic_predictor.h"

namespace dpsim {
PeriodicPredictor::PeriodicPredictor(int32_t interval,
                                     int32_t sampling_interval,
                                     int32_t sample_size,
                                     int32_t mode) :
  Predictor(interval), sampling_interval(sampling_interval),
  sample_size(sample_size), last_sampled_time(-sampling_interval),
  interval_sum(0.0) {
    switch (mode) {
      case mode_singleperiod:
        this->mode = mode_singleperiod;
        break;
      case mode_singleperiod_cdh:
        this->mode = mode_singleperiod_cdh;
        break;
    }
  }

int
PeriodicPredictor::consume(struct trace* parsed_trace) {
  int32_t sampling_interval_end_time = this->last_sampled_time
                                       + this->sampling_interval;

  if (parsed_trace == NULL) {
    return 0;
  } else if (sampling_interval_end_time < parsed_trace->timestamp) {
    // fast-forward time
    while (sampling_interval_end_time < parsed_trace->timestamp) {
      // add sample
      if (this->last_sampled_time >= 0) {
        this->samples.push_back(this->interval_sum);
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
}

double
PeriodicPredictor::predict() {
  if (this->samples.size() >= this->sample_size) {
    std::vector<double> autocorrelation;
    this->calculate_autocorrelation(&autocorrelation);

    if (this->mode == mode_singleperiod) {
      double max_corr = -1.0;
      int32_t max_period = -1;
      int32_t period = 0;

      for (int32_t period = 1; period < autocorrelation.size(); period++) {
        double corr_sum = 0, corr_avg;
        int32_t corr_cnt = 0;
        for (int32_t n_period = period;
             n_period < autocorrelation.size();
             n_period += period) {
          corr_sum += autocorrelation.at(n_period);
          corr_cnt++;
        }

        corr_avg = corr_sum/corr_cnt;
        if (corr_avg > max_corr) {
          max_corr = corr_avg;
          max_period = period;
        }
      }

      std::vector<double> interval_sum;
      std::vector<int32_t> interval_cnt;

      for (int32_t i = 0; i < max_period; i++) {
        interval_sum.push_back(0.0);
        interval_cnt.push_back(0);
      }

      double prediction = 0;

      for (int32_t interval = 0;
           interval < this->samples.size()
                      + Predictor::prediction_interval/this->sampling_interval;
           interval++) {
        if (interval < this->samples.size()) {
          interval_sum[interval%max_period] += this->samples.at(interval);
          interval_cnt[interval%max_period]++;
        } else {
          double sum = interval_sum[interval%max_period];
          double cnt = interval_cnt[interval%max_period];
          prediction += sum/cnt;
        }
      }

      return prediction;
    } else if (this->mode == mode_singleperiod_cdh) {
      double max_corr = -1.0;
      int32_t max_period = -1;
      int32_t period = 0;

      for (int32_t period = 1; period < autocorrelation.size(); period++) {
        double corr_sum = 0, corr_avg;
        int32_t corr_cnt = 0;
        for (int32_t n_period = period;
             n_period < autocorrelation.size();
             n_period += period) {
          corr_sum += autocorrelation.at(n_period);
          corr_cnt++;
        }

        corr_avg = corr_sum/corr_cnt;
        if (corr_avg > max_corr) {
          max_corr = corr_avg;
          max_period = period;
        }
      }

      std::cout << "max_period" << max_period << std::endl;

      std::map<int32_t, int32_t> interval_histogram;
      std::map<int32_t, double> interval_prediction;

      // for every interval [0:max_period-1]
      for (int32_t interval = 0; interval < max_period; interval++) {
        int32_t interval_cnt = 0;
        int32_t cumulated_cnt = 0;
        int32_t prediction = 0;

        interval_histogram.clear();

        for (int32_t i = interval;
             i < this->samples.size();
             i += max_period) {
          int32_t bin = static_cast<int32_t>(this->samples[i]/256);
          interval_histogram[bin]++;
          interval_cnt++;
        }

        float boundary = ceil(interval_cnt*0.8);
        int32_t boundary_cnt = static_cast<int32_t>(boundary);

        std::map<int32_t, int32_t>::iterator it;

        for (it = interval_histogram.begin();
             it != interval_histogram.end();
             it++) {
          // add frequency
          cumulated_cnt += it->second;

          if (cumulated_cnt >= boundary_cnt) {
            prediction = it->first;
            break;
          }
        }
        interval_prediction[interval] = (prediction+1) * 256;
      }

      double prediction = 0;

      for (int32_t interval = this->samples.size();
           interval < this->samples.size()
                      + Predictor::prediction_interval/this->sampling_interval;
           interval++) {
        prediction += interval_prediction[interval%max_period];
      }

      return prediction;
    }
  }
  return 0;
}

static double
calculate_variance(std::vector<double> container) {
  double sum = 0, mu = 0, variance = 0;

  std::vector<double>::iterator it;
  for (it = container.begin(); it != container.end(); it++) {
    sum += *it;
  }
  mu = sum / container.size();

  for (it = container.begin(); it != container.end(); it++) {
    variance += (*it - mu) * (*it - mu);
  }

  return sqrt(variance);
}

static double
calculate_covariance(std::vector<double> container1,
                     std::vector<double> container2) {
  assert(container1.size() == container2.size());
  double sum = 0, mu1 = 0, mu2 = 0, covariance = 0;

  std::vector<double>::iterator it1, it2;
  for (it1 = container1.begin(); it1 != container1.end(); it1++) {
    sum += *it1;
  }
  mu1 = sum / container1.size();

  sum = 0;
  for (it2 = container2.begin(); it2 != container2.end(); it2++) {
    sum += *it2;
  }
  mu2 = sum / container2.size();

  for (it1 = container1.begin(), it2 = container2.begin();
       it1 != container1.end() && it2 != container2.end();
       it1++, it2++) {
    covariance += (*it1 - mu1) * (*it2 - mu2);
  }

  return covariance;
}

void
PeriodicPredictor::calculate_autocorrelation(std::vector<double> *container) {
  std::vector<double> samples_copy;
  std::deque<double>::iterator it;

  for (it = this->samples.begin(); it != this->samples.end(); it++) {
    samples_copy.push_back(*it);
  }

  for (int32_t offset = 0; offset <= this->sample_size/2; offset++) {
    std::vector<double> container1(samples_copy.begin(),
                                   samples_copy.end() - offset);
    std::vector<double> container2(samples_copy.begin() + offset,
                                   samples_copy.end());

    double variance1 = calculate_variance(container1);
    double variance2 = calculate_variance(container2);
    double covariance = calculate_covariance(container1, container2);
    double correlation = (variance1 == 0 || variance2 == 0) ?
                         1.0 : covariance/(variance1*variance2);
    container->push_back(correlation);
  }
}
}  // namespace dpsim
