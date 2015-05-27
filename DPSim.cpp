#include <iostream>
#include <fstream>
#include <string>

#include "./parser/blk_parser.h"
#include "./predictor/predictor.h"
#include "./predictor/oracle_predictor.h"

#define PREDICTION_INTERVAL 5

int main(int argc, char** argv) {
  dpsim::BlkParser *parser = new dpsim::BlkParser(argc, argv);
  dpsim::Predictor *predictor =
    new dpsim::OraclePredictor(PREDICTION_INTERVAL);

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
      predictor->consume(&parsed_trace);
      filtered_processed++;
    }
  }

  // Cleanup the trace
  predictor->consume(NULL);

  std::cout << "processed : " << processed << std::endl;
  std::cout << "filtered : "  << filtered_processed << std::endl;

  while (!predictor->prediction_empty()) {
    struct dpsim::prediction pred = predictor->pop_prediction();
    fout << pred.start_time << "," << pred.data_size << std::endl;
  }

  fin.close();
  fout.close();

  delete parser;
  delete predictor;
}
