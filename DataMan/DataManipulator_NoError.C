#include <iostream>
#include <stdio.h>
#include <istream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <string.h>
#include <cstdlib>

using namespace std;

int readWriteFile( char* inFileName )
{

	ifstream file( inFileName );

	if( !file.is_open() )
	{
		cerr << "Input file failed to open" << endl;
		return 1;
	}

	vector< vector<string> > splitLine;
	string line;

	int cOld = 0;	
	int cNew = 0;
	
	while( getline( file, line ) )
	{
		stringstream streamLine(line);
		string feature;

		vector< vector<string> > helper;
		while( getline( streamLine, feature, ',') )
		{
			stringstream streamSeries(feature);
			string value;

			vector<string> lineVec;
			while( getline( streamSeries, value, ' ') )
			{
				lineVec.push_back( value );
			}
			helper.push_back( lineVec );
		}
	
		for(int i = 0; i < helper[1].size(); i++)
		{

			bool ERROR = false;

			vector<string> transpose;
			for( int j = 0; j < helper.size(); j++)
			{
				if( helper[j][i] == "-99900.0" || 
					helper[j][i] == "-99903.0" || 
					helper[j][i] == "-99901.0" || 
					helper[j][i] == "nan" || 
					helper[j][i] == "999.0" )
				{
					ERROR = true;
				}

				transpose.push_back( helper[j][i] );
			}

			if( !ERROR )
			{
				splitLine.push_back( transpose );
			}
			cNew++;
		}
		cOld++;
	}

	file.close();

	char buff[256];
	sprintf(buff, "noERR_%s", inFileName);

	ofstream outfile ( buff );

	if( !outfile.is_open() )
	{
		cerr << "Output file failed to open" << endl;
		return 1;
	}

	int size = splitLine[0].size();

	for( int i = 1; i < splitLine.size(); i++ )
	{
		for( int j = 0; j < size; j++ )
		{
			outfile << splitLine[i][j];
			if( j+1 < size )
			{
				outfile << ",";
			} 
		}
		outfile << endl;
	}	

	outfile.close();

	return 0;

}

int main( int argc, char** argv )
{

	if( argc != 2 )
	{
		cerr << "Need arguments: run function like this:\n./<executable> <input_file_name>" << endl;
		cerr << "\t" << "<executable> should be pretty self explanatory." << endl;
		cerr << "\t" << "<input_file_name> is the input file in .csv format." << endl;
		return 1;
	}

	cerr << argv[1] << endl;

	return readWriteFile( argv[1] );

}


