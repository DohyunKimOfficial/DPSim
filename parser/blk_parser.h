#ifndef BLK_PARSER_H

#include <cassert>
#include <getopt.h>
#include <cstring>
#include <cstdlib>
#include <iostream>

#define FILTER_MAX 10
#define ACTION_MAX  5
#define RWBS_MAX    5

// options
struct parse_option {
  string infile; // input file directory
  string outfile; // output file directory
  struct parse_filter filter; // filter struct
};

// filters
struct parse_filter {
  int device_major_num;
  int device_major_num[FILTER_MAX];
  int device_minor_num;
  int device_minor_num[FILTER_MAX];
  int cpu_id_num;
  int cpu_id[FILTER_MAX];
  int process_id_num;
  int process_id[FILTER_MAX];
  int action_num;
  char action[ACTION_MAX];
  int rwbs_num;
  char rwbs[RWBS_MAX];
};

struct trace {
  int device_major;
  int device_minor;
  int cpu_id;
  int process_id;
  char action[ACTION_MAX];
  char rwbs[ACTION_MAX];

  size_t start_block_num;
  size_t block_size;

  char* process_name;
};

class BlkParser {
  private:
    struct parse_option opt;

  public:
    BlkParser ();
    BlkParser (int argc, char** args);

    // Option related functions
    void initialize_options ();
    int parse_int_options (int* addr, char *option);
    void parse_options (int argc, char** args);
    void print_options ();

    // Trace related functions
    void parse_trace (struct trace* parsed_trace, string line);
    int process_trace_line (struct trace* parsed_trace, string line);
};

#endif // BLK_PARSER_H
