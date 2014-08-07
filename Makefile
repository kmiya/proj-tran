CXX = g++
CXXFLAGS = -O2 -Wall -Wextra -std=c++11

SRCDIR = ./src
SRCS   = $(wildcard $(SRCDIR)/*.cpp)
OBJS   = $(subst .cpp,.o, $(SRCS))

all: proj

proj: $(OBJS)
	$(CXX) -o $@ $^ `pkg-config --libs opencv`

$(SRCDIR)/%.o: %.h %.cpp `pkg-config --cflags opencv`

clean:
	rm -f $(SRCDIR)/*.o
