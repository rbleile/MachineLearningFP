all: data avge noerr sub tvt comp

data:
	g++ -O3 DataManipulator_Exp.C -o ../../Data/expander

avge:
	g++ -g DataManipulator_AvgErr.C -o ../../Data/avgerr

noerr:
	g++ -O3 DataManipulator_NoError.C -o ../../Data/noerr

sub:
	g++ -O3 DataManipulator_SubSelect.C -o ../../Data/subs

tvt:
	g++ DataManipulator_Val_Test.C -o ../../Data/tvt

comp:
	g++ CompositCount.C -o ../../Data/compcount

clean:
	@rm ../../Data/expander
	@rm ../../Data/noerr
	@rm ../../Data/subs
	@rm ../../Data/tvt
	@rm ../../Data/compcount
