# Makefile
#
# author: Thanasis Vergoulis

#./bin/ -- Contains the binary files.
#./src/ -- Contains the source files.
#./tmp/ -- Contains the objective files.


# make -f arsm.make #Make a simple executable.
# make "OFLAGS = -c -g" "XFLAG = -g -o" -f arsm.make #Make executable containing debug info (for Valgrind).
# Macros.
OFLAGS = -c -O3 #to be  o
#OFLAGS = -c -g
XFLAG = -o
#XFLAG = -g -o
EXECFILE = dn_aligner.out
BUILDIDX = build_idx.out
DATABUILDER = data_builder.out
QUERYBUILDER = query_builder.out
SYSTEMCONSTANTS = system_constants.out
OPTIMALPHI = optimal_phi.out
CODEPATH = ..
LIBS = $(CODEPATH)/Libraries

# Rules.
# In the first line we determine the files which have to be changed to run the rule. In the second 
# line we determine the command to be executed when we run the rule.

all: main.o Utilities.o MemObserver.o ArgReader.o MyMaths.o SparseBitset.o StrHash.o HitmapHash.o Algorithms.o Validator.o HitmapAligner.o SeqGen.o Files.o
	g++ $(XFLAG) bin/$(EXECFILE) main.o Utilities.o MemObserver.o ArgReader.o MyMaths.o SparseBitset.o StrHash.o HitmapHash.o Algorithms.o Validator.o HitmapAligner.o SeqGen.o Files.o

MemObserver.o: $(LIBS)/Misc/MemObserver.h $(LIBS)/Misc/MemObserver.cpp
	g++ $(OFLAGS) $(LIBS)/Misc/MemObserver.cpp

ArgReader.o: $(LIBS)/Misc/ArgReader.h $(LIBS)/Misc/ArgReader.cpp
	g++ $(OFLAGS) $(LIBS)/Misc/ArgReader.cpp

MyMaths.o: $(LIBS)/Math/MyMaths.h $(LIBS)/Math/MyMaths.cpp
	g++ $(OFLAGS) $(LIBS)/Math/MyMaths.cpp

SparseBitset.o: $(LIBS)/Bitsets/SparseBitset.h $(LIBS)/Bitsets/SparseBitset.cpp
	g++ -I $(LIBS)/External/boost_1_84_0 $(OFLAGS) $(LIBS)/Bitsets/SparseBitset.cpp 

StrHash.o: $(LIBS)/Sequences/SHash/StrHash.h $(LIBS)/Sequences/SHash/StrHash.cpp
	g++ -I $(LIBS)/External/boost_1_84_0 $(OFLAGS) $(LIBS)/Sequences/SHash/StrHash.cpp

HitmapHash.o: $(LIBS)/Sequences/SHash/HitmapHash.h $(LIBS)/Sequences/SHash/HitmapHash.cpp StrHash.o
	g++ -I $(LIBS)/External/boost_1_84_0 $(OFLAGS) $(LIBS)/Sequences/SHash/HitmapHash.cpp

Algorithms.o: $(LIBS)/Sequences/Algorithms.h $(LIBS)/Sequences/Algorithms.cpp
	g++ $(OFLAGS) $(LIBS)/Sequences/Algorithms.cpp
	
Misc.o: Misc.h Misc.cpp
	g++ $(OFLAGS) Misc.cpp

Validator.o: Validator.h Validator.cpp
	g++ $(OFLAGS) Validator.cpp

HitmapAligner.o: HitmapAligner.h HitmapAligner.cpp 
	g++ -I $(LIBS)/External/boost_1_84_0 $(OFLAGS) HitmapAligner.cpp
	
main.o: main.cpp
	g++ -I $(LIBS)/External/boost_1_84_0 $(OFLAGS) main.cpp
	
SeqGen.o: $(LIBS)/Sequences/SeqGen/SeqGen.h $(LIBS)/Sequences/SeqGen/SeqGen.h
	g++ $(OFLAGS) $(LIBS)/Sequences/SeqGen/SeqGen.cpp
	
Utilities.o: $(LIBS)/Misc/Utilities.h $(LIBS)/Misc/Utilities.cpp
	g++ $(OFLAGS) $(LIBS)/Misc/Utilities.cpp
	
Files.o: $(LIBS)/Files/Files.h $(LIBS)/Files/Files.cpp
	g++ $(OFLAGS) $(LIBS)/Files/Files.cpp
	
data_builder: dataset_creator.o ArgReader.o Files.o Utilities.o SeqGen.o
	g++ $(XFLAG) bin/$(DATABUILDER) dataset_creator.o ArgReader.o Files.o Utilities.o SeqGen.o

dataset_creator.o: dataset_creator.cpp
	g++ $(OFLAGS) dataset_creator.cpp
	
query_builder: query_creator.o ArgReader.o Utilities.o SeqGen.o
	g++ $(XFLAG) bin/$(QUERYBUILDER) query_creator.o ArgReader.o Utilities.o SeqGen.o

query_creator.o: query_creator.cpp
	g++ $(OFLAGS) query_creator.cpp

build_idx: build_idx.o HitmapAligner.o Utilities.o MyMaths.o HitmapHash.o StrHash.o SparseBitset.o Algorithms.o ArgReader.o
	g++ $(XFLAG) bin/$(BUILDIDX) build_idx.o HitmapAligner.o Utilities.o MyMaths.o HitmapHash.o StrHash.o SparseBitset.o Algorithms.o ArgReader.o

build_idx.o: build_idx.cpp
	g++ -I $(LIBS)/External/boost_1_84_0 $(OFLAGS) build_idx.cpp
	
system_constants: system_constants.o Algorithms.o SeqGen.o StrHash.o SparseBitset.o
	g++ $(XFLAG) bin/$(SYSTEMCONSTANTS) system_constants.o Algorithms.o SeqGen.o StrHash.o SparseBitset.o

system_constants.o: system_constants.cpp
	g++ $(OFLAGS) system_constants.cpp
	
optimal_phi: optimal_phi.o ArgReader.o HitmapHash.o StrHash.o SparseBitset.o Algorithms.o MyMaths.o
	g++ $(XFLAG) bin/$(OPTIMALPHI) optimal_phi.o ArgReader.o HitmapHash.o StrHash.o SparseBitset.o Algorithms.o MyMaths.o
	
optimal_phi.o: optimal_phi.cpp
	g++ $(OFLAGS) optimal_phi.cpp

clean:
	rm *.o
	rm bin/*.out