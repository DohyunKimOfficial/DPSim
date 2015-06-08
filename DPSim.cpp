#include <iostream>
#include <fstream>
#include <string>

#include "./parser/blk_parser.h"
#include "./predictor/predictor.h"
#include "./predictor/oracle_predictor.h"
#include "./predictor/cdh_predictor.h"
#include "./predictor/history_predictor.h"

#define PREDICTION_INTERVAL 5
#define CDH_T_EXPIRE 30
#define CDH_N_WB (CDH_T_EXPIRE/PREDICTION_INTERVAL)
#define MB 1024
#define GB 1024*1024

#define NUM_PREDICTOR 5

int main(int argc, char** argv) {
  dpsim::BlkParser *parser = new dpsim::BlkParser(argc, argv);

  dpsim::Predictor *predictors[NUM_PREDICTOR];

  predictors[0] =
    new dpsim::CDHPredictor(0.8, CDH_T_EXPIRE, CDH_N_WB, 10*MB, 0);
  predictors[1] =
    new dpsim::CDHPredictor(0.8, CDH_T_EXPIRE, CDH_N_WB, 10*MB, 5);
  predictors[2] =
    new dpsim::CDHPredictor(0.8, CDH_T_EXPIRE, CDH_N_WB, 10*MB, 10);
  predictors[3] =
    new dpsim::CDHPredictor(0.8, CDH_T_EXPIRE, CDH_N_WB, 10*MB, 50);
  predictors[4] =
    new dpsim::CDHPredictor(0.8, CDH_T_EXPIRE, CDH_N_WB, 10*MB, 100);


  int processed = 0;
  int filtered_processed = 0;

  std::string line;

  std::ifstream fin;
  std::ofstream fout;
  struct dpsim::trace parsed_trace;

  fin.open(parser->get_input_file_name());
  fout.open(parser->get_output_file_name());

  parser->print_options();

  for (int i = 0; i < NUM_PREDICTOR; i++) {
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

    while (!predictors[i]->prediction_empty()) {
      struct dpsim::prediction pred = predictors[i]->pop_prediction();
      fout << pred.start_time << "," << pred.data_size << std::endl;
    }

    delete predictors[i];
  }


  fin.close();
  fout.close();

  delete parser;
}
