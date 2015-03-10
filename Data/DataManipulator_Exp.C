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

int readWriteFile( char* inFileName, char* outFileName, bool train )
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
			}

			splitLine.push_back( transpose );		
			cNew++;
		}
		cOld++;
	}

	file.close();

	char outFileHeaders[256];
	sprintf(outFileHeaders, "header_%s", outFileName );
	
	char outFileSolutions[256];
	sprintf(outFileSolutions, "expected_%s", outFileName );

	int size;

	if( train )
	{
		size = splitLine[0].size()-1;
	}
	else
	{
		size = splitLine[0].size();
	}

	ofstream headers ( outFileHeaders );

	if( !headers.is_open() )
	{
		cerr << "Headers file failed to open" << endl;
		return 1;
	}

	for( int j = 0; j < size; j++ )
	{
		headers << splitLine[0][j];
		if( j+1 < size )
		{
			headers << ",";
		} 
	}
	headers << endl;

	headers.close();

	if( train )
	{

		ofstream solutions ( outFileSolutions );

		if( !solutions.is_open() )
		{
			cerr << "Solutions file failed to open" << endl;
			return 1;
		}
	
		for( int i = 1; i < splitLine.size(); i++ )
		{
			solutions << splitLine[ i ][ size ];
			solutions << endl;
		}
	
		solutions.close();
	
	}

	ofstream outfile ( outFileName );

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

	if( argc != 4 )
	{
		cerr << "Need arguments: run function like this:\n./<executable> <input_file_name> <output_file_name> <is_train_data>" << endl;
		cerr << "\t" << "<executable> should be pretty self explanatory." << endl;
		cerr << "\t" << "<input_file_name> is the input file in .csv format." << endl;
		cerr << "\t" << "<output_file_name> is the basename for output files." << endl;
		cerr << "\t" << "<is_train_data> is a boolean flag if the input is training data." << endl;
		return 1;
	}

	cerr << argv[1] << " " << argv[2] << " " << argv[3] << endl;

	int train = atoi( argv[3] );

	return readWriteFile( argv[1], argv[2], (bool)train );

}


