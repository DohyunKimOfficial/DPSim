CC=g++
CCFLAGS=-std=c++0x -g -O0

PARSER_DIR=parser

DEPS=$(PARSER_DIR)/blk_parser.h
PARSER=$(PARSER_DIR)/blk_parser.cpp
PARSER_OBJ=$(PARSER_DIR)/blk_parser.o

$(PARSER_OBJ):
	$(CC) $(CCFLAGS) -c -o $@ $(PARSER)

parser: $(PARSER_OBJ)
	$(CC) $(CCFLAGS) -o DPParser DPParser.cpp $(PARSER_OBJ)

clean:
	rm -f $(PARSER_DIR)/*.o
