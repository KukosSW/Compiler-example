# Very Simple Makefile - this project is simple we dont need a BIG, OP Makefile

# Shell
RM := rm -rf

# DIRS
IDIR  := ./inc
SDIR  := ./src
VMDIR := ./vm
TDIR  := ./test

# COMPI, DEFAULT G++
CXX ?= g++
LEX := flex
YAC := bison

CXX_STD   = -std=c++17
CXX_OPT   = -O3
CXX_FLAGS =
CXX_WARNS =

ifeq ($(CXX),clang++)
	CXX_WARNS += -Weverything
else ifneq (, $(filter $(CC), cc gcc))
	CXX_WARNS += -Wall -Wextra
endif

ifeq ("$(origin DEBUG)", "command line")
	GGDB := -ggdb3
else
	GGDB :=
endif

CXX_FLAGS += $(CXX_STD) $(CXX_OPT) $(GGDB) $(CXX_WARNS)

EXEC = kompilator
SRC = $(wildcard $(SDIR)/*.cpp)
OBJ = $(SRC:%.cpp=%.o)
LIBS = fl

H_INC := $(foreach d, $(IDIR), -I$d)
L_INC := $(foreach l, $(LIB), -l$l)

all: compiler vm

.PHONY:compiler
compiler: $(EXEC)

$(SDIR)/parser_tab.c $(IDIR)/parser_tab.h: $(SDIR)/parser.y
	$(YAC) --debug -Wall --defines=$(IDIR)/parser_tab.h $(SDIR)/parser.y -o $(SDIR)/parser_tab.c

$(SDIR)/parser_lex.yy.c: $(SDIR)/parser.l $(IDIR)/parser_tab.h
	$(LEX) -o $@ $(SDIR)/parser.l

%.o:%.cpp _FORCE
	$(CXX) $(CXX_FLAGS) $(H_INC) -c $< -o $@

_FORCE:


$(EXEC): $(SDIR)/parser_lex.yy.c $(SDIR)/parser_tab.c $(IDIR)/parser_tab.h $(OBJ)
	$(CXX) $(CXX_FLAGS) $(H_INC) $(SDIR)/parser_tab.c $(SDIR)/parser_lex.yy.c $(OBJ) $(L_INC) -o $@

.PHONY:vm
vm:
	@cd $(VMDIR) && $(MAKE) --no-print-directory

.PHONY:test
test: compiler vm
	@cd $(TDIR) && ./tests.sh

.PHONY:clean
clean:
	$(RM) $(IDIR)/parser_tab.h
	$(RM) $(SDIR)/parser_tab.c
	$(RM) $(SDIR)/parser_lex.yy.c
	$(RM) $(OBJ)
	$(RM) $(EXEC)
	@cd $(VMDIR) && $(MAKE) cleanall --no-print-directory