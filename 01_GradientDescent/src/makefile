SRCS=$(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
exe_sources=$(filter main%.cpp,$(SRCS))
EXEC=$(exe_sources:.cpp=)

main:
	g++ -O3 -std=c++20 \
    	-I../include -I${mkEigenInc} \
    	main.cpp \
    	-o main.out

main_debug:
	g++ -O3 -std=c++20 -DDEBUG \
    	-I../include -I${mkEigenInc} \
    	main.cpp \
    	-o main.out

main_mac:
	g++ -O3 -std=c++20 \
    	-I"/opt/homebrew/include/eigen3" \
    	-I"../include/" \
    	main.cpp \
    	-o main.out
main_debug_mac:
	g++ -O3 -std=c++20 -DDEBUG \
    	-I"/opt/homebrew/include/eigen3" \
    	-I"../include/" \
    	main.cpp \
    	-o main.out

clean:
	-\rm -f $(EXEC) $(OBJS)

distclean:
	$(MAKE) clean
	$(RM) -f ./doc $(DEPEND)
	$(RM) *.out *.bak *~