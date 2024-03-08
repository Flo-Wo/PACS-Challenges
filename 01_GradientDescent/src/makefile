# Compiler flags with include directory
CXXFLAGS := -std=c++20 -Wall -Wextra -pedantic -I../include  -I"/opt/homebrew/include/eigen3"
# LDFLAGS :=  
# LIBS := -lEigen3 
# get all files *.cpp
SRCS=$(wildcard *.cpp)
# get the corresponding object file
OBJS = $(SRCS:.cpp=.o)
# get all headers in the working directory
HEADERS=$(wildcard *.hpp)
#
exe_sources=$(filter main%.cpp,$(SRCS))
EXEC=$(exe_sources:.cpp=)

#========================== ORA LA DEFINIZIONE DEGLI OBIETTIVI
.phony= all clean distclean doc

.DEFAULT_GOAL = all

all: $(DEPEND) $(EXEC)

clean:
	-\rm -f $(EXEC) $(OBJS)

distclean:
	$(MAKE) clean
	$(RM) -f ./doc $(DEPEND)
	$(RM) *.out *.bak *~

doc:
	doxygen $(DOXYFILE)

$(EXEC): $(OBJS)

$(OBJS): $(SRCS)

$(DEPEND): $(SRCS)
	-\rm $(DEPEND)
	for f in $(SRCS); do \
	$(CXX) $(STDFLAGS) $(CPPFLAGS) -MM $$f >> $(DEPEND); \
	done

-include $(DEPEND)



