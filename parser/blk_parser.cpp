#include "blk_parser.h"

using namespace std;

BlkParser::BlkParser () {
  this->initialize_options ();
}

BlkParser::BlkParser (int argc, char** args) {
  this->initialize_options ();
  this->parse_options (argc, args);
}

void
BlkParser::initialize_options ()
{
  this->opt.infile = "input.txt";
  this->opt.outfile = "output.txt";

  this->opt.filter.device_major_num = 0;
  this->opt.filter.device_minor_num = 0;
  this->opt.filter.cpu_id_num = 0;
  this->opt.filter.process_id_num = 0;
  this->opt.filter.action_num = 0;
  this->opt.filter.rwbs_num = 0;
}

int
BlkParser::parse_int_options (int* addr, char *option) {
  int option_num;
  char* token;

  assert (option != NULL);

  token = strtok (option, ",");

  for (option_num = 0; option_num < FILTER_MAX && token != NULL;
       token = strtok (NULL, ",")) {
    addr[option_num++] = atoi(token);
  }

  return option_num;
}

void
BlkParser::parse_options (int argc, char** args) {
  int option_select, option_index = 0;

  static struct option long_options[] =
  {
    {"infile",    required_argument, 0, 'i'},
    {"outfile",   required_argument, 0, 'o'},
    {"dmajor",    required_argument, 0, 'D'},
    {"dminor",    required_argument, 0, 'd'},
    {"cpuid",     required_argument, 0, 'c'},
    {"processid", required_argument, 0, 'p'},
    {"action",    required_argument, 0, 'a'},
    {"rwbs",      required_arugment, 0, 'r'},
    {0, 0, 0, 0}
  };

  option_select = getopt_long (argc, args, long_options, &option_index);

  while (option_select != -1) {
    switch (option_select) {
      case 'i':
        this->opt.infile = optarg;
        break;
      case 'o':
        this->opt.outfile = optarg;
        break;
      case 'D':
        this->opt.filter.device_major_num =
          parse_int_options (&(this->opt.filter.device_major[0]), optarg);
        break;
      case 'd':
        this->opt.filter.device_minor_num =
          parse_int_options (&(this->opt.filter.device_minor[0]), optarg);
        break;
      case 'c':
        this->opt.filter.cpu_id_num =
          parse_int_options (&(this->opt.filter.cpu_id[0]), optarg);
        break;
      case 'p':
        this->opt.filter.process_id_num =
          parse_int_options (&(this->opt.filter.process_id[0]), optarg);
        break;
      case 'a':
        strncpy (this->opt.filter.action, optarg, ACTION_MAX);
        this->opt.filter.action_num = strlen (this->opt.filter.action);
        break;
      case 'r':
        strncpy (this->opt.filter.action, optarg, RWBS_MAX);
        this->opt.filter.rwbs_num = strlen (this->opt.filter.rwbs);
        break;
    }
  }
}

void
BlkParser::print_options () {
  cout << "#############################" << endl
       << "####### PARSE OPTIONS #######" << endl
       << "-----------------------------" << endl
       << "# Input file: " << this->opt.filter.infile << endl
       << "# Output file: " << this->opt.filter.outfile << endl;

  if (this->opt.filter.device_major_num == 0) {
    cout << "# Device major filter disabled" << endl;
  }
  else {
    cout << "# Device major filter: ";
    for (int i = 0; i < this->opt.filter.device_major_num; i++) {
      cout << this->opt.filter.device_major[i] << " ";
    }
    cout << endl;
  }

  if (this->opt.filter.device_minor_num == 0) {
    cout << "# Device minor filter disabled" << endl;
  }
  else {
    cout << "# Device minor filter: ";
    for (int i = 0; i < this->opt.filter.device_minor_num; i++) {
      cout << this->opt.filter.device_minor[i] << " ";
    }
    cout << endl;
  }

  if (this->opt.filter.device_cpu_id_num == 0) {
    cout << "# Cpu id filter disabled" << endl;
  }
  else {
    cout << "# Cpu id filter: ";
    for (int i = 0; i < this->opt.filter.cpu_id_num; i++) {
      cout << this->opt.filter.cpu_id[i] << " ";
    }
    cout << endl;
  }

  if (this->opt.filter.process_id_num == 0) {
    cout << "# Process id filter disabled" << endl;
  }
  else {
    cout << "# Process id filter: ";
    for (int i = 0; i < this->opt.filter.process_id_num; i++) {
      cout << this->opt.filter.process_id[i] << " ";
    }
    cout << endl;
  }

  if (this->opt.filter.action_num == 0) {
    cout << "# Action filter disabled" << endl;
  }
  else {
    cout << "# Action filter: " << this->opt.filter.action << endl;
  }

  if (this->opt.filter.rwbs_num == 0) {
    cout << "# RWBS filter disabled" << endl;
  }
  else {
    cout << "# RWBS filter: " << this->opt.filter.rwbs << endl;
  }
}

void
BlkParser::parse_trace (struct trace* parsed_trace, string line) {
  // TODO
}

int
BlkParser::process_trace_line (struct trace* parsed_trace, string line) {
  // TODO
}
