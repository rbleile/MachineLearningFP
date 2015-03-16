all: knn

knn: unmap.o neighbor_search.hpp neighbor_search_stat.hpp
	g++ -O3 unmap.o KNN.cpp -o ../../Data/knn -lmlpack -Wl,--no-as-needed -L${MKLROOT}/lib/intel64 -lmkl_intel_ilp64 -lmkl_core -lmkl_gnu_thread -ldl -lpthread -lm -DMKL_ILP64 -fopenmp -m64 -I${MKLROOT}/include

unmap: unmap.hpp
	g++ -c unmap.cpp -lmlpack

clean:
	@rm unmap.o
	@rm ../../Data/knn
