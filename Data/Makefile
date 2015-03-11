all: data tvt comp

data:
	g++ -O3 DataManipulator_Exp.C -o ../../Data/expander

tvt:
	g++ DataManipulator_Val_Test.C -o ../../Data/tvt

comp:
	g++ CompositCount.C -o ../../Data/compcount

clean:
	@rm ../../Data/expander
	@rm ../../Data/tvt
	@rm ../../Data/compcount
