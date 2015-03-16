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

	int numOk[20];
	memset( numOk, 0, sizeof(int)*20 );
	double okCol[20];
	memset(okCol, 0.0, sizeof(double)*20 );

	int index[20] = { 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1 };
	int RMap[20]  = { 0, 1, 2, 3, 4, 6, 8, 9, 10, 12, 14, 16, 18, 19, 21, 23, 25, 27, 29, 31 };
	int IMap [33] = { 0, 1, 2, 3, 4, -1, 5, -1, 6, 7, 8, -1, 9, -1, 10, -1, 11, -1, 12, 13, -1, 14, -1, 15, -1, 16, -1, 17, -1, 18, -1, 19, -1 };
	
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

			vector<string> transpose;
			for( int j = 0; j < helper.size(); j++)
			{
				transpose.push_back( helper[j][i] );
	
				if( index[j] == 1 ){

					if( helper[j][i] == "-99900.0" || 
						helper[j][i] == "-99903.0" || 
						helper[j][i] == "-99901.0" || 
						helper[j][i] == "nan" || 
						helper[j][i] == "999.0" )
					{
						char buff[256];
						sprintf(buff, "%d", 1);
						transpose.push_back( buff );
					}
					else
					{
						numOk[j]++;
						okCol[j] += atof( helper[j][i].c_str() );
						char buff[256];
						sprintf(buff, "%d", 0);
						transpose.push_back( buff );
					}
				}
			}

			splitLine.push_back( transpose );
			cNew++;
		}
		cOld++;
	}

	file.close();

	for( int i = 0; i < 20; i++ )
	{
		okCol[i] /= numOk[i]; 
	}

	int size = splitLine[0].size();

	for( int i = 0; i < splitLine.size(); i++ )
	{
		for( int j = 0; j < size; j++ )
		{
			if( IMap[j] != -1 )
			{
				if( splitLine[i][j] == "-99900.0" || 
					splitLine[i][j] == "-99903.0" || 
					splitLine[i][j] == "-99901.0" || 
					splitLine[i][j] == "nan" || 
					splitLine[i][j] == "999.0" )
				{
					char buff[256];
					sprintf(buff, "%f", okCol[ IMap[j] ] );
					splitLine[i][j] = buff;
				}				
			}
		}
	}

	char filename[256];
	sprintf(filename, "avgERR_%s", inFileName );

	ofstream outfile ( filename );

	if( !outfile.is_open() )
	{
		cerr << "Output file failed to open" << endl;
		return 1;
	}

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

	return readWriteFile( argv[1]  );

}


