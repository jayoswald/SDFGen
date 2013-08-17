# C++ compiler (and linker)
CXX=ccache g++

# Include libraries
CFLAGS+= -I.
LFLAGS+=

# VTK library
# Disable these line if you don't want to build with vtk
CFLAGS+=-DENABLE_VTK
CFLAGS+=-I$(wildcard /usr/include/vtk-5.*)
LFLAGS+=-L$(wildcard /usr/lib/vtk-5.*) -lvtkIO -lvtkCommon -lvtkFiltering

# Linker flags
LFLAGS+=-llapack -lpthread -lrt

SRC=$(wildcard *.cpp */*.cpp)
OBJ=$(SRC:.cpp=.o)
EXE=SDFgen

all: $(SRC) $(EXE) $(OBJ) tags
all: CFLAGS+=-O3 -fopenmp
all: LFLAGS+= -fopenmp

debug: $(SRC) $(EXE) $(OBJ)
debug: CFLAGS+=-g -fopenmp
debug: LFLAGS+=-g -fopenmp

profile: $(SRC) $(EXE) $(OBJ)
profile: CFLAGS+=-O2 -pg
profile: LFLAGS+=-pg

doc:
	@cd ../doc && doxygen

tags: $(SRC)
	@ctags $(wildcard *.h */*.h *.cpp */*.cpp)

$(EXE): $(OBJ) $(EX_LIB_FILE)
	$(CXX) $(OBJ) $(LFLAGS) -o $@

.cpp.o:
	$(CXX) -c -std=c++0x $(CFLAGS) $< -o $@

clean:
	@rm -f $(OBJ) tags
