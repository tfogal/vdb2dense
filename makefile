CXXFLAGS=-std=c++11 -Wall -Wextra -ggdb
LIBS=-lopenvdb -ltbb
obj=vdb2dense.o

all: $(obj) vdb2dense

vdb2dense: vdb2dense.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LIBS)

clean:
	rm -f vdb2dense $(obj)
.PHONY: clean
