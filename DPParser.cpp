#include <iostream>
#include <fstream>
#include <string>

#include "parser/blk_parser.h"

int main(int argc, char** argv) {
  dpsim::BlkParser *parser = new dpsim::BlkParser(argc, argv);

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
      fout << line << std::endl;
      filtered_processed++;
    }
  }

  std::cout << "process ended with " << processed << std::endl;
  std::cout << "filtered processes : " << filtered_processed << std::endl;

  fin.close();
  fout.close();

  delete parser;
}
