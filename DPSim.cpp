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

int main(int argc, char** argv) {
  dpsim::BlkParser *parser = new dpsim::BlkParser(argc, argv);
  dpsim::Predictor *predictor1 =
    new dpsim::OraclePredictor(PREDICTION_INTERVAL);
  dpsim::Predictor *predictor2 =
    new dpsim::CDHPredictor(0.8, CDH_T_EXPIRE, CDH_N_WB, 10*MB, 100);
  dpsim::Predictor *predictor3 =
    new dpsim::HistoryPredictor(PREDICTION_INTERVAL, 4);

  int processed = 0;
  int filtered_processed = 0;

  std::string line;

  std::ifstream fin;
  std::ofstream fout;
  struct dpsim::trace parsed_trace;

  fin.open(parser->get_input_file_name());
  fout.open(parser->get_output_file_name());

  parser->print_options();

  while (std::getline(fin, line)) {
    processed++;

    if (parser->process_trace_line(&parsed_trace, line)) {
      predictor1->consume(&parsed_trace);
      predictor2->consume(&parsed_trace);
      predictor3->consume(&parsed_trace);
      filtered_processed++;
    }
  }

  // Cleanup the trace
  predictor1->consume(NULL);
  predictor2->consume(NULL);
  predictor3->consume(NULL);

  std::cout << "processed : " << processed << std::endl;
  std::cout << "filtered : "  << filtered_processed << std::endl;

  while (!predictor1->prediction_empty()) {
    struct dpsim::prediction pred = predictor1->pop_prediction();
    fout << pred.start_time << "," << pred.data_size << std::endl;
  }

  while (!predictor2->prediction_empty()) {
    struct dpsim::prediction pred = predictor2->pop_prediction();
    fout << pred.start_time << "," << pred.data_size << std::endl;
  }

  while (!predictor3->prediction_empty()) {
    struct dpsim::prediction pred = predictor3->pop_prediction();
    fout << pred.start_time << "," << pred.data_size << std::endl;
  }

  fin.close();
  fout.close();

  delete parser;
  delete predictor1;
  delete predictor2;
  delete predictor3;
}
