#include <iostream>
#include <stdio.h>
#include <istream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <string.h>
#include <cstdlib>
#include <ctime>

using namespace std;

int readWriteFile( char* inFileName, char* outFileName )
{

	ifstream file( inFileName );

	if( !file.is_open() )
	{
		cerr << "Input file failed to open" << endl;
		return 1;
	}

	vector< vector<string> > splitLine_Train;
	vector< vector<string> > splitLine_Valid;
	vector< vector<string> > splitLine_Test;
	string line;

	bool headers = true;

	while( getline( file, line ) )
	{
		stringstream streamLine(line);
		string value;

		vector<string> lineVec;

		while( getline( streamLine, value, ',') )
		{
			lineVec.push_back(value);
		}

		if( headers )
		{	
			splitLine_Train.push_back( lineVec );
			splitLine_Valid.push_back( lineVec );
			splitLine_Test.push_back( lineVec );
			headers = false;
		}
		else
		{

			int random = (std::rand() % 10) + 1;

			if( random <= 7 )
			{
				splitLine_Train.push_back( lineVec );
			}
			else if( random == 8 )
			{
				splitLine_Valid.push_back( lineVec );
			}
			else
			{
				splitLine_Test.push_back( lineVec );
			}
		}
	}

	file.close();

	int numTrain = splitLine_Train.size();
	int numValid = splitLine_Valid.size();
	int numTest  = splitLine_Test.size();

	int size  = splitLine_Test[0].size();

	int total = numTrain + numValid + numTest;

	cerr << "Percent Splits: " << endl;
	cerr << "Train:\t" << (float)numTrain / (float)total  << endl;
	cerr << "Valid:\t" << (float)numValid / (float)total  << endl;
	cerr << "Test: \t" <<  (float)numTest / (float)total  << endl;

	char outFileTrain[256];
	sprintf(outFileTrain, "Train_%s", outFileName );
	
	char outFileValid[256];
	sprintf(outFileValid, "Valid_%s", outFileName );

	char outFileTest[256];
	sprintf(outFileTest, "Test_%s", outFileName );

	ofstream train ( outFileTrain );

	if( !train.is_open() )
	{
		cerr << "Train file failed to open" << endl;
		return 1;
	}

	for( int j = 0; j < numTrain; j++ )
	{
		for( int i = 0; i < size; i++ )
		{
			train << splitLine_Train[j][i];
			if( i+1 < size )
			{
				 train << ",";
			} 
		}
		train << endl;
	}

	train.close();

	ofstream valid ( outFileValid );

	if( !valid.is_open() )
	{
		cerr << "Train file failed to open" << endl;
		return 1;
	}

	for( int j = 0; j < numTrain; j++ )
	{
		for( int i = 0; i < size; i++ )
		{
			valid << splitLine_Train[j][i];
			if( i+1 < size )
			{
				 valid << ",";
			} 
		}
		valid << endl;
	}

	valid.close();

	ofstream test ( outFileTest );

	if( !test.is_open() )
	{
		cerr << "Train file failed to open" << endl;
		return 1;
	}

	for( int j = 0; j < numTrain; j++ )
	{
		for( int i = 0; i < size; i++ )
		{
			test << splitLine_Train[j][i];
			if( i+1 < size )
			{
				 test << ",";
			} 
		}
		test << endl;
	}

	test.close();

	return 0;

}

int main( int argc, char** argv )
{

	if( argc != 3 )
	{
		cerr << "Need arguments: run function like this:\n./<executable> <input_file_name> <output_file_name>" << endl;
		cerr << "\t" << "<executable> should be pretty self explanatory." << endl;
		cerr << "\t" << "<input_file_name> is the input file in .csv format." << endl;
		cerr << "\t" << "<output_file_name> is the basename for output files." << endl;
		return 1;
	}

	cerr << argv[1] << " " << argv[2] << endl;

	srand( std::time(NULL) );

	return readWriteFile( argv[1], argv[2] );

}


