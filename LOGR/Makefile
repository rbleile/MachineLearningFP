all: logr

logr: 
	g++ -O3 LOGR.cpp -o ../../Data/logr -lmlpack -Wl,--no-as-needed -L${MKLROOT}/lib/intel64 -lmkl_intel_ilp64 -lmkl_core -lmkl_gnu_thread -ldl -lpthread -lm -DMKL_ILP64 -fopenmp -m64 -I${MKLROOT}/include -larmadillo

clean:
	@rm ../../Data/logr
