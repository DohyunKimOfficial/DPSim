#include <iostream>
#include <fstream>
#include <string>

#include "parser/blk_parser.h"

int main(int argc, char** argv) {
  // initialize parser with given argc, argv
  dpsim::BlkParser *parser = new dpsim::BlkParser(argc, argv);

  // variables
  int processed = 0;  // number of processed trace lines
  int filtered_processed = 0;  // number of filtered trace lines
  std::string line;  // temporary string for line input

  std::ifstream fin;  // file input stream
  std::ofstream fout;  // file output stream
  struct dpsim::trace parsed_trace;  // temporary parsed trace input

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
