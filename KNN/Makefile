all: knn

knn: unmap.o neighbor_search.hpp neighbor_search_stat.hpp
	g++ unmap.o KNN.cpp -o ../../Data/knn -lmlpack

unmap: unmap.hpp
	g++ -c unmap.cpp -lmlpack

clean:
	@rm unmap.o
	@rm ../../Data/knn
