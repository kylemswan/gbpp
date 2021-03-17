# environment configuration
EXE := gbpp
SRCDIR := source source/cpu
INCDIR := include
BLDDIR := build

# compiler configuration
CXX := g++
CXXFLAGS := -MMD -I$(INCDIR)

# link required SFML libraries
LDLIBS := -lsfml-system -lsfml-window -lsfml-graphics

# set VPATH so that source files are found in their (sub) directories
VPATH := $(SRCDIR)

# find source files and generate the corresponding object and dependency names
SRCS := $(foreach DIR, $(SRCDIR), $(notdir $(wildcard $(DIR)/*.cpp)))
OBJS := $(patsubst %.cpp, build/%.o, $(SRCS))
DEPS := $(wildcard build/*.d)

# compilation and linking targets
$(EXE): $(OBJS)
	$(CXX) $^ -o $@ $(LDLIBS)

$(BLDDIR)/%.o: %.cpp | $(BLDDIR)
	$(CXX) -c $< -o $@ $(CXXFLAGS) 

$(BLDDIR):
	mkdir $@

include $(DEPS)

# utility targets
clean:
	rm -rf $(BLDDIR)

remove:
	rm -f $(EXE)
