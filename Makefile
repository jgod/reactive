CXX = clang++
CXXFLAGS = -stdlib=libc++ \
-ansi \
-std=c++11

OUTDIR = ./build
TESTS_DEPS = tests/main.cpp

all: clean test

clean:
	rm -rf $(OUTDIR)/*

test: $(TESTS_DEPS)
	mkdir -p $(OUTDIR)
	$(CXX) $(CXXFLAGS) ./tests/main.cpp -o $(OUTDIR)/test.a

lint: $(TESTS_DEPS)
	cppcheck -v ./src/reactive.h --report-progress --enable=all