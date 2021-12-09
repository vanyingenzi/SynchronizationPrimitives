#!/bin/bash
CC=gcc
CFLAGS=-std=gnu99 -Wall -Werror -g
LIBS=-lpthread -lm
INCLUDE_HEADERS_DIRECTORY=-Iheaders
SRC_DIR=./src
SRC_FILES=$(wildcard $(SRC_DIR)/*.c)
CSV_DIR=./csv
CSV_FILES=$(wildcard $(CSV_DIR)/*.csv)
MUTE_FLAG= > /dev/null 2>&1
NCORES=`grep "^core id" /proc/cpuinfo | sort -u | wc -l`
2_NCORES=$(shell expr ${NCORES} \* 2 )
DUMMY_FILE=JHijezdyPNPz.txt
TAS_PRIMITIVES=$(SRC_DIR)/tas_primitives.o $(SRC_DIR)/my_semaphore.o
TATAS_PRIMITIVES=$(SRC_DIR)/tatas_primitives.o $(SRC_DIR)/my_semaphore.o

### Signaling Makefile to not delete intermediate dependency files
.PRECIOUS: $(SRC_FILES:.c=.o) 

####################### Compiling Targets

.PHONY : compile_all clean_exec_obj

compile_all:	c_s-posix_philosopher		c_s-posix_producer_consumer			c_s-posix_reader_writer \
				c_tas-tas_philosopher		c_tas-tas_producer_consumer			c_tas-posix_reader_writer \
				c_tatas-tatas_philosopher	c_tatas-tatas_producer_consumer		c_tatas-tatas_reader_writer \
				test_test_and_set			test_test_and_test_and_set

### Compile Standardly 
c_s-%: $(SRC_DIR)/%.o 
	@$(eval TARGET=`echo "$@" | rev | cut -d "-" -f1 | rev`)
	@$(CC) $(CFLAGS) -o $(TARGET) $^ $(LIBS)

### Compile Using Test-And-Set Primitives
c_tas-%: $(SRC_DIR)/%.o $(TAS_PRIMITIVES)
	@$(eval TARGET=`echo "$@" | rev | cut -d "-" -f1 | rev`)
	@$(CC) $(CFLAGS) -o $(TARGET) $^ $(LIBS)

### Compile Using Test-And-Test-And-Set Primitives
c_tatas-%: $(SRC_DIR)/%.o $(TATAS_PRIMITIVES)
	@$(eval TARGET=`echo "$@" | rev | cut -d "-" -f1 | rev`)
	@$(CC) $(CFLAGS) -o $(TARGET) $^ $(LIBS)

test_test_and_set: $(SRC_DIR)/test_test_and_set.o $(TAS_PRIMITIVES)
	@$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

test_test_and_test_and_set: $(SRC_DIR)/test_test_and_test_and_set.o $(TATAS_PRIMITIVES)
	@$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

test_semaphore: $(SRC_DIR)/test_semaphore.o $(TAS_PRIMITIVES)
	@$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	
### Compiling object files
%.o: %.c 
	@$(CC) $(INCLUDE_HEADERS_DIRECTORY) $(CFLAGS) -o $@ -c $<

####################### Benchmarking Targets

benchmark : init_list time_test-test_and_test_and_set time_test-test_and_set \
			time_problem-philosopher time_problem-producer_consumer time_problem-reader_writer 

### Initiliaze Varible List that will hold the number of threads
init_list:
	@seq 1 1 $(2_NCORES) > $(DUMMY_FILE)
	$(eval LIST = `cat $(DUMMY_FILE)`)

### Run a problem with all it's implementation
time_problem-%: c_s-posix_% c_tas-tas_% c_tatas-tatas_% 
	@$(eval TARGET=`echo "$@" | rev | cut -d "-" -f1 | rev`)
	@for impl in posix tas tatas; do \
		touch $(CSV_DIR)/$${impl}_${TARGET}.csv; \
		echo nthreads, real_time > $(CSV_DIR)/$${impl}_${TARGET}.csv; \
		for n_thread in $(LIST); do \
			for ignore in 1 2 3 4 5; do \
				echo -n $${n_thread}, >> $(CSV_DIR)/$${impl}_${TARGET}.csv; \
				{ /usr/bin/time --format=%e ./$${impl}_${TARGET} $${n_thread}; } 2>> $(CSV_DIR)/$${impl}_${TARGET}.csv; \
				echo $${impl}_${TARGET} $${n_thread}; \
			done \
		done \
	done \

### Testing Lock mechanisms 
time_test-%: test_%
	@$(eval TARGET=`echo "$@" | rev | cut -d "-" -f1 | rev`)
	@touch $(CSV_DIR)/$(TARGET).csv
	@echo nthreads, real_time > $(CSV_DIR)/$(TARGET).csv
	@for n_thread in $(LIST); do \
		for ignore in 1 2 3 4 5; do \
			echo -n $${n_thread}, >>  $(CSV_DIR)/$(TARGET).csv; \
			{ /usr/bin/time --format=%e ./$^ $${n_thread}; } 2>>  $(CSV_DIR)/$(TARGET).csv; \
			echo $^ $${n_thread}; \
		done \
	done \
	
####################### Cleaning Targets

clean: clean_exec_obj 
	@rm -f $(DUMMY_FILE)

clean_exec_obj: clean_objects
	@rm -f posix_philosopher tas_philosopher tatas_philosopher
	@rm -f posix_producer_consumer tas_producer_consumer tatas_producer_consumer
	@rm -f posix_reader_writer tas_reader_writer tatas_reader_writer
	@rm -f test_and_set test_test_and_set test_test_and_test_and_set

clean_csv:
	@rm -f $(CSV_DIR)/*.csv

clean_objects:
	@rm -f $(SRC_DIR)/*.o

graphs: 
	@rm -f ./graphs/*
	@for file in $(CSV_FILES); \
	do \
		python ./python/script.py $$file; \
	done 
	@python ./python/compare.py ./csv/test_and_set.csv ./csv/test_and_test_and_set.csv 
	@python ./python/compare.py ./csv/posix_philosopher.csv ./csv/tas_philosopher.csv ./csv/tatas_philosopher.csv
	@python ./python/compare.py ./csv/posix_producer_consumer.csv ./csv/tas_producer_consumer.csv ./csv/tatas_producer_consumer.csv
	@python ./python/compare.py ./csv/posix_reader_writer.csv ./csv/tas_reader_writer.csv ./csv/tatas_reader_writer.csv