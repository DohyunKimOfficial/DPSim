#ifndef PARSER_BLK_PARSER_H_
#define PARSER_BLK_PARSER_H_

#include <getopt.h>

#include <cassert>
#include <cstring>
#include <string>
#include <cstdlib>
#include <iostream>

// Config: maximum number of filters
#define FILTER_MAX   10
// Config: maximum length of process name
#define PROCESS_NAME_MAX    128

// Namespace for block parser
namespace dpsim {

// filters
struct parse_filter {
  int device_major_num;
  int device_major[FILTER_MAX];
  int device_minor_num;
  int device_minor[FILTER_MAX];
  int cpu_id_num;
  int cpu_id[FILTER_MAX];
  int process_id_num;
  int process_id[FILTER_MAX];
  int action_num;
  char action[FILTER_MAX];
  int rwbs_num;
  char rwbs[FILTER_MAX];
  char process_name[PROCESS_NAME_MAX];
};

// options
struct parse_option {
  // input file path
  std::string infile;
  // output file path
  std::string outfile;
  // parsing filter
  struct parse_filter filter;
};
 /* The standard header (or initial fields displayed) include:
 * "%D %2c %8s %5T.%9t %5p %2a %3d"
 * Breaking this down:
 *   %D
 *     Displays the event's device major/minor as: %3d,%-3d.
 *   %2c
 *     CPU ID (2-character field).
 *   %8s
 *     Sequence number
 *   %5T.%9t
 *     5-character field for the seconds portion of the time stamp
 *     and a 9-character field for the nanoseconds in the time stamp.
 *   %5p
 *     5-character field for the process ID.
 *   %2a
 *     2-character field for one of the actions.
 *   %3d
 *     3-character field for the RWBS data.
 */
struct trace {
  int device_major;
  int device_minor;
  int cpu_id;
  double timestamp;
  int process_id;
  char action[FILTER_MAX];
  char rwbs[FILTER_MAX];
  size_t start_block_num;
  size_t block_size;

  char process_name[PROCESS_NAME_MAX];
};

// BlkParser
class BlkParser {
 private:
  struct parse_option opt;

 public:
  BlkParser();
  BlkParser(int argc, char** args);

  // Option related functions
  void initialize_options();
  int parse_int_options(int* addr, char *option);
  void parse_options(int argc, char** args);
  void print_options();

  // Trace related functions
  void parse_trace(struct trace* parsed_trace, std::string line);
  bool process_trace_line(struct trace* parsed_trace, std::string line);
  bool filter_trace(struct trace* parsed_trace);

  // Print related functions
  std::string get_input_file_name();
  std::string get_output_file_name();
};
}  // namespace dpsim

#endif  // PARSER_BLK_PARSER_H_
