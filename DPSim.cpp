#include <iostream>
#include <fstream>
#include <string>

#include "./parser/blk_parser.h"
#include "./predictor/predictor.h"
#include "./predictor/oracle_predictor.h"
#include "./predictor/cdh_predictor.h"
#include "./predictor/history_predictor.h"
#include "./predictor/fb_history_predictor.h"
#include "./predictor/periodic_predictor.h"
#include "./predictor/neural_predictor.h"

#define PREDICTION_INTERVAL 5
#define CDH_T_EXPIRE 30
#define CDH_N_WB (CDH_T_EXPIRE/PREDICTION_INTERVAL)
#define MB 1024
#define GB 1024*1024

#define NUM_PREDICTOR 1

int main(int argc, char** argv) {
  dpsim::BlkParser *parser = new dpsim::BlkParser(argc, argv);

  dpsim::Predictor *oracle_predictor;
  dpsim::Predictor *predictors[NUM_PREDICTOR + 1];

  oracle_predictor =
    new dpsim::OraclePredictor(PREDICTION_INTERVAL);
  predictors[0] =
    new dpsim::NeuralPredictor(4, 5, 5, 6, dpsim::mode_arma);
//  predictors[0] =
//    new dpsim::HistoryPredictor(5, 6, dpsim::mode_avg);
/*  predictors[1] =
    new dpsim::CDHPredictor(0.8, CDH_T_EXPIRE, CDH_N_WB, 10*MB, 6);
  predictors[2] =
    new dpsim::CDHPredictor(0.8, PREDICTION_INTERVAL, 1, 1*MB, 30);
  predictors[3] =
    new dpsim::FBHistoryPredictor(5, 6, dpsim::mode_fb_exclusion, 0.05, 0.05);
  predictors[4] =
    new dpsim::PeriodicPredictor(PREDICTION_INTERVAL, 1, 30,
                                 dpsim::mode_singleperiod);
*/
  predictors[1] = oracle_predictor;

  int processed = 0;
  int filtered_processed = 0;

  std::string line;

  std::ifstream fin;
  std::ofstream fout;
  struct dpsim::trace parsed_trace;

  fin.open(parser->get_input_file_name());
  fout.open(parser->get_output_file_name());

  parser->print_options();

  for (int i = 0; i < NUM_PREDICTOR + 1; i++) {
    std::cout << "Predictor(" << i << ")" << std::endl;

    processed = 0;
    filtered_processed = 0;

    fin.clear();
    fin.seekg(0, std::ios::beg);

    while (std::getline(fin, line)) {
      processed++;

      if (parser->process_trace_line(&parsed_trace, line)) {
        predictors[i]->consume(&parsed_trace);
        filtered_processed++;
      }
    }

    predictors[i]->consume(NULL);

    std::cout << "processed : " << processed << std::endl;
    std::cout << "filtered : "  << filtered_processed << std::endl;
  }

  bool valid = true;

  while (!oracle_predictor->prediction_empty() && valid) {
    struct dpsim::prediction oracle_prediction =
      oracle_predictor->pop_prediction();

    valid = true;

    std::string line = std::to_string(oracle_prediction.start_time) + "," +
                       std::to_string(oracle_prediction.data_size);

    for (int i = 0; i < NUM_PREDICTOR; i++) {
      if (!predictors[i]->prediction_empty()) {
        struct dpsim::prediction pred = predictors[i]->pop_prediction();
        line += ",";
        line += std::to_string(pred.data_size);
      } else {
        line += ",";
        line += "n/a";
        break;
      }
    }

    if (valid) {
      fout << line << std::endl;
    }
  }

  for (int i = 0; i < NUM_PREDICTOR + 1; i++) {
    delete predictors[i];
  }

  fin.close();
  fout.close();

  delete parser;
}
