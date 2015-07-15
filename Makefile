CC=g++
CCFLAGS=-std=c++0x -g -O0

PARSER_DIR=parser
PREDICTOR_DIR=predictor

PARSER_DEPS=$(PARSER_DIR)/blk_parser.h
PREDICTOR_DEPS=$(PREDICTOR_DIR)/predictor.h \
							 $(PREDICTOR_DIR)/oracle_predictor.h \
							 $(PREDICTOR_DIR)/cdh_predictor.h \
							 $(PREDICTOR_DIR)/history_predictor.h \
							 $(PREDICTOR_DIR)/fb_history_predictor.h \
							 $(PREDICTOR_DIR)/periodic_predictor.h \
							 $(PREDICTOR_DIR)/neural_predictor.h

DEPS=$(PARSER_DEPS) \
		 $(PREDCITRO_DEPS)

PARSER_OBJ=$(patsubst %.h, %.o, $(PARSER_DEPS))
PREDICTOR_OBJ=$(patsubst %.h, %.o, $(PREDICTOR_DEPS))

DPPARSER=DPParser
DPPARSER_SRC=$(DPPARSER).cpp
DPSIM=DPSim
DPSIM_SRC=$(DPSIM).cpp

all: parser simulator

%.o: %.cpp
	$(CC) $(CCFLAGS) -c -o $@ $<

parser: $(PARSER_OBJ)
	$(CC) $(CCFLAGS) -o $(DPPARSER) $(DPPARSER_SRC) $(PARSER_OBJ)

simulator: $(PARSER_OBJ) $(PREDICTOR_OBJ)
	$(CC) $(CCFLAGS) -o $(DPSIM) $(DPSIM_SRC) $(PARSER_OBJ) $(PREDICTOR_OBJ)

clean:
	rm -f $(PARSER_OBJ) $(PREDICTOR_OBJ) $(DPPARSER) $(DPSIM)
