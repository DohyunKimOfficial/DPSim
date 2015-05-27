#include "./blk_parser.h"

#include <string>
#include <cstdio>

namespace dpsim {
  BlkParser::BlkParser() {
    this->initialize_options();
  }

  BlkParser::BlkParser(int argc, char** args) {
    this->initialize_options();
    this->parse_options(argc, args);
  }

  void
  BlkParser::initialize_options() {
    this->opt.infile = "input.txt";
    this->opt.outfile = "output.txt";

    this->opt.filter.device_major_num = 0;
    this->opt.filter.device_minor_num = 0;
    this->opt.filter.cpu_id_num = 0;
    this->opt.filter.process_id_num = 0;
    this->opt.filter.action_num = 0;
    this->opt.filter.rwbs_num = 0;
    this->opt.filter.process_name[0] = '\0';
  }

  int
  BlkParser::parse_int_options(int* addr, char *option) {
    int option_num;
    char *token, *saveptr;

    assert(option != NULL);

    token = strtok_r(option, ",", &saveptr);

    for (option_num = 0;
         option_num < FILTER_MAX && token != NULL;
         token = strtok_r(NULL, ",", &saveptr)) {
      addr[option_num++] = atoi(token);
    }

    return option_num;
  }

  void
  BlkParser::parse_options(int argc, char** args) {
    int option_select, option_index = 0;

    static struct option long_options[] = {
      {"infile",      required_argument, 0, 'i'},
      {"outfile",     required_argument, 0, 'o'},
      {"dmajor",      required_argument, 0, 'D'},
      {"dminor",      required_argument, 0, 'd'},
      {"cpuid",       required_argument, 0, 'c'},
      {"processid",   required_argument, 0, 'p'},
      {"action",      required_argument, 0, 'a'},
      {"rwbs",        required_argument, 0, 'r'},
      {"processname", required_argument, 0, 'n'},
      {0, 0, 0, 0}
    };

    while ((option_select = getopt_long(argc, args,
                                        "i:o:D:d:c:p:a:r:n:",
                                        long_options, &option_index))
           != -1) {
      switch (option_select) {
        case 'i':
          this->opt.infile = optarg;
          break;
        case 'o':
          this->opt.outfile = optarg;
          break;
        case 'D':
          this->opt.filter.device_major_num =
            parse_int_options(&(this->opt.filter.device_major[0]), optarg);
          break;
        case 'd':
          this->opt.filter.device_minor_num =
            parse_int_options(&(this->opt.filter.device_minor[0]), optarg);
          break;
        case 'c':
          this->opt.filter.cpu_id_num =
            parse_int_options(&(this->opt.filter.cpu_id[0]), optarg);
          break;
        case 'p':
          this->opt.filter.process_id_num =
            parse_int_options(&(this->opt.filter.process_id[0]), optarg);
          break;
        case 'a':
          strncpy(this->opt.filter.action, optarg, FILTER_MAX);
          this->opt.filter.action_num = strlen(this->opt.filter.action);
          break;
        case 'r':
          strncpy(this->opt.filter.rwbs, optarg, FILTER_MAX);
          this->opt.filter.rwbs_num = strlen(this->opt.filter.rwbs);
          break;
        case 'n':
          strncpy(this->opt.filter.process_name, optarg, PROCESS_NAME_MAX);
          break;
      }
    }
  }

  void
  BlkParser::print_options() {
    std::cout << "#############################" << std::endl
              << "####### PARSE OPTIONS #######" << std::endl
              << "-----------------------------" << std::endl
              << "# Input file: " << this->opt.infile << std::endl
              << "# Output file: " << this->opt.outfile << std::endl;

    if (this->opt.filter.device_major_num == 0) {
      std::cout << "# Device major filter disabled" << std::endl;
    } else {
      std::cout << "# Device major filter("
                << this->opt.filter.device_major_num << "): ";
      for (int i = 0; i < this->opt.filter.device_major_num; i++) {
        std::cout << this->opt.filter.device_major[i] << " ";
      }
      std::cout << std::endl;
    }

    if (this->opt.filter.device_minor_num == 0) {
      std::cout << "# Device minor filter disabled" << std::endl;
    } else {
      std::cout << "# Device minor filter("
                << this->opt.filter.device_minor_num << "(: ";
      for (int i = 0; i < this->opt.filter.device_minor_num; i++) {
        std::cout << this->opt.filter.device_minor[i] << " ";
      }
      std::cout << std::endl;
    }

    if (this->opt.filter.cpu_id_num == 0) {
      std::cout << "# Cpu id filter disabled" << std::endl;
    } else {
      std::cout << "# Cpu id filter("
                << this->opt.filter.cpu_id_num
                << "): " << std::endl;
      for (int i = 0; i < this->opt.filter.cpu_id_num; i++) {
        std::cout << this->opt.filter.cpu_id[i] << " ";
      }
      std::cout << std::endl;
    }

    if (this->opt.filter.process_id_num == 0) {
      std::cout << "# Process id filter disabled" << std::endl;
    } else {
      std::cout << "# Process id filter("
                << this->opt.filter.process_id_num << "): ";
      for (int i = 0; i < this->opt.filter.process_id_num; i++) {
        std::cout << this->opt.filter.process_id[i] << " ";
      }
      std::cout << std::endl;
    }

    if (this->opt.filter.action_num == 0) {
      std::cout << "# Action filter disabled" << std::endl;
    } else {
      std::cout << "# Action filter("
                << this->opt.filter.action_num << "):"
                << this->opt.filter.action << std::endl;
    }

    if (this->opt.filter.rwbs_num == 0) {
      std::cout << "# Rwbs filter disabled" << std::endl;
    } else {
      std::cout << "# Rwbs filter("
                << this->opt.filter.rwbs_num << "):"
                << this->opt.filter.rwbs << std::endl;
    }

    if (this->opt.filter.process_name[0] == '\0') {
      std::cout << "# Process name filter disabled" << std::endl;
    } else {
      std::cout << "# Process name filter: "
                << this->opt.filter.process_name << std::endl;
    }
  }

  void
  BlkParser::parse_trace(struct trace* parsed_trace, std::string line) {
    assert(parsed_trace != NULL);

    char *cpy_line = new char[line.length()+1];
    char *token, *saveptr;

    snprintf(cpy_line, line.length()+1, "%s", line.c_str());

    // %D field
    token = strtok_r(cpy_line, ",", &saveptr);
    parsed_trace->device_major = atoi(token);

    token = strtok_r(NULL, " ", &saveptr);
    parsed_trace->device_minor = atoi(token);

    // %2c field
    token = strtok_r(NULL, " ", &saveptr);
    parsed_trace->cpu_id = atoi(token);

    // %8s field
    token = strtok_r(NULL, " ", &saveptr);

    // %5T.%9t field
    token = strtok_r(NULL, " ", &saveptr);
    parsed_trace->timestamp = atof(token);

    // %5p field
    token = strtok_r(NULL, " ", &saveptr);
    parsed_trace->process_id = atoi(token);

    // %2a
    token = strtok_r(NULL, " ", &saveptr);
    strncpy(parsed_trace->action, token, FILTER_MAX);

    // %3d
    token = strtok_r(NULL, " ", &saveptr);
    strncpy(parsed_trace->rwbs, token, FILTER_MAX);

    // data
    token = strtok_r(NULL, " ", &saveptr);
    if (*token >= '0' && *token <= '9') {
      parsed_trace->start_block_num = atoi(token);

      token = strtok_r(NULL, " ", &saveptr);
      if (*token == '+') {
        token = strtok_r(NULL, " ", &saveptr);
        parsed_trace->block_size = atoi(token);

        token = strtok_r(NULL, " ", &saveptr);
      }
    }

    if (*token == '[') {
      strncpy(parsed_trace->process_name, token+1, PROCESS_NAME_MAX);
      parsed_trace->process_name[strlen(token)-2] = '\0';
    }

    delete[] cpy_line;
  }

  bool
  BlkParser::filter_trace(struct trace* parsed_trace) {
    assert(parsed_trace != NULL);

    bool filtered = true;
    bool filtered_temp = false;

    // device major filter (OR filter)
    filtered_temp = (this->opt.filter.device_major_num == 0);
    for (int i = 0; i < this->opt.filter.device_major_num; i++) {
      if (parsed_trace->device_major == this->opt.filter.device_major[i]) {
        filtered_temp = true;
      }
    }
    filtered &= filtered_temp;

    // device minor filter (OR filter)
    filtered_temp = (this->opt.filter.device_minor_num == 0);
    for (int i = 0; i < this->opt.filter.device_minor_num; i++) {
      if (parsed_trace->device_minor == this->opt.filter.device_minor[i]) {
        filtered_temp = true;
      }
    }
    filtered &= filtered_temp;

    // cpu id filter (OR filter)
    filtered_temp = (this->opt.filter.cpu_id_num == 0);
    for (int i = 0; i < this->opt.filter.cpu_id_num; i++) {
      if (parsed_trace->cpu_id == this->opt.filter.cpu_id[i]) {
        filtered_temp = true;
      }
    }
    filtered &= filtered_temp;

    // process id filter (OR filter)
    filtered_temp = (this->opt.filter.process_id_num == 0);
    for (int i = 0; i < this->opt.filter.process_id_num; i++) {
      if (parsed_trace->process_id == this->opt.filter.process_id[i]) {
        filtered_temp = true;
      }
    }
    filtered &= filtered_temp;

    // action filter (AND filter)
    filtered_temp = true;
    for (int i = 0; i < this->opt.filter.action_num; i++) {
      if (strchr(parsed_trace->action, this->opt.filter.action[i]) == NULL) {
        filtered_temp = false;
      }
    }
    filtered &= filtered_temp;

    // rwbs filter (AND filter)
    filtered_temp = true;
    for (int i = 0; i < this->opt.filter.rwbs_num; i++) {
      if (strchr(parsed_trace->rwbs, this->opt.filter.rwbs[i]) == NULL) {
        filtered_temp = false;
      }
    }
    filtered &= filtered_temp;

    // process name filter
    filtered_temp = (strlen(this->opt.filter.process_name) == 0) ||
                    (strstr(parsed_trace->process_name,
                            this->opt.filter.process_name) != NULL);
    filtered &= filtered_temp;

    return filtered;
  }

  bool
  BlkParser::process_trace_line(struct trace* parsed_trace, std::string line) {
    this->parse_trace(parsed_trace, line);
    return this->filter_trace(parsed_trace);
  }

  std::string
  BlkParser::get_input_file_name() {
    return this->opt.infile;
  }

  std::string
  BlkParser::get_output_file_name() {
    return this->opt.outfile;
  }
}  // namespace dpsim
