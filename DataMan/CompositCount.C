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
		//cerr << endl;
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
			//DEBUG cerr << "IndexN count: " << cNew;
			vector<string> transpose;
			for( int j = 0; j <= helper.size(); j++)
			{
				if(j == 0)
				{
					transpose.push_back( helper[j][0] );
				}
				else if (j == 1)
				{
					if(cOld == 0)
					{
						transpose.push_back( "IdN" );
					}
					else
					{
						char buff[256];
						sprintf(buff, "%d", cNew);
						transpose.push_back( buff );
					}
				} 
				else if(j == helper.size()){
						transpose.push_back( helper[j - 1][0] );
				}	
				else{
						transpose.push_back( helper[j-1][i] );
				}
					//DEBUG cerr << helper.size() << " " << j-1 << endl;
				//DEBUG cerr << " " << transpose[j];
			}
			//DEBUG cerr << endl << helper[0].size() << " " << i << endl;
			//DEBUG cerr << endl;

			splitLine.push_back( transpose );		
			cNew++;
		}
		cOld++;
	}

	file.close();

	int size = splitLine.size()-1;
	int size2 = splitLine[1].size()-1;
	int counter = 0;

	for( int i = 0; i <= size; i++ )
	{
		if( atof( splitLine[i][4].c_str()) < 0.0 && atof(splitLine[i][size2].c_str()) != 0.0 )
		{
			counter++;			
	//		cerr << i << "\t\t" << splitLine[i][1] << "\t" << splitLine[i][4] << "\t" << splitLine[i][size2] << endl;
		}
	}

	cerr << "Counter: " << counter << endl;
	cerr << "Percent: " << ((float)counter / size) * 100.0 << endl;

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

	return readWriteFile( argv[1] );

}


