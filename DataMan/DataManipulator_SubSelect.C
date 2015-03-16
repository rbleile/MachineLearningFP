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

int readWriteFile( int numS, char* inFileName, char* outFileName, int b )
{
        string helper = inFileName; 
        string eInFileName = "expected_" + helper;
        helper = outFileName;
        string eOutFileName = "expected_" + helper;

	if( numS <=  0 )
	{
		cerr << "Number of Splits needs to be > 0: " << numS << endl;
		cerr << "Your command line-fu is not over 9000!" << endl;
		return 8999;
	}
	if( b < 0 )
	{
		cerr << "Batch number needs to be 1 or greater: " << b << endl;	
		cerr << "Your command line-fu is not over 9000!" << endl;
		return 8999;
	}

	ifstream file( inFileName );
        ifstream efile( eInFileName.c_str() );
	if( !file.is_open() )
	{
		cerr << "Train data input file failed to open" << endl;
		return 1;
	}
	else if( !efile.is_open() )
	{
	       cerr << "Expected value input file failed to open" << endl;
	       return 1;
	}
	
	vector< vector< vector<string> > > subselect;
	subselect.resize( numS );

        vector< vector<string> > esubselect;
        esubselect.resize( numS );

	string line;
	string eline;


	while( getline( file, line ) )
	{
		stringstream streamLine(line);
		string value;
	        getline( efile, eline );
		vector<string> lineVec;

		while( getline( streamLine, value, ',') )
		{
			lineVec.push_back(value);
		}

	        int random = (std::rand() % numS);
	        subselect[random].push_back( lineVec );
	        esubselect[random].push_back( eline );
	}


	file.close();
	efile.close();

	unsigned int ss = 0;

	for( int i = 0; i < numS; i++ )
	{
		ss += subselect[i].size();
	}

	for( int i = 0; i < numS; i++ )
	{
		cerr  << "Precent Split File: " << i << " " << 100.0 * (float)subselect[i].size() / (float)ss << "%" << endl;
	}

	unsigned int numA = subselect[0][0].size();

	for( int i = 0; i < numS; i++ )
	{
		unsigned int size = subselect[i].size();
		char filename[256];
		char efilename[256];
		
		if( b == 0 )
		{
			sprintf( filename, "%d_%s", i, outFileName );
			sprintf( efilename, "%d_%s", i, eOutFileName.c_str() );
		}
		else
		{
			sprintf( filename, "%d_%d_%s", b-1, i, outFileName );
			sprintf( efilename, "%d_%d_%s", b-1, i, eOutFileName.c_str() );
		}
		
		ofstream out ( filename );
		ofstream eout ( efilename );
	
		if( !out.is_open() )
		{
			cerr << "train data file " << i << " failed to open" << endl;
			return 1;
		}
		else if( !eout.is_open() )
		{
		        cerr << "expected file " << i << " failed to open" << endl;
		        return 1;
		}

		
		for( int j = 0; j < size; j++ )
		{
			for( int k = 0; k < numA; k++ )
			{
				out << subselect[i][j][k];
				if( k+1 < numA )
				{
					 out << ",";
				} 
			}
			out << endl;
			eout << esubselect[i][j] << endl;
		}
	
		out.close();
		eout.close();
	}

	return 0;

}

int main( int argc, char** argv )
{

	if( argc < 4 || argc > 5 )
	{
		cerr << "Need arguments: run function like this:\n./<executable> <input_file_name> <output_file_name> <num_splits> <batch_num>" << endl;
		cerr << "\t" << "<executable> should be pretty self explanatory." << endl;
		cerr << "\t" << "<input_file_name> is the input file in .csv format." << endl;
		cerr << "\t" << "<output_file_name> is the basename for output files." << endl;
		cerr << "\t" << "<num_splits> is the number of times to split the data set." << endl;
		cerr << "\t" << "<batch_num> (optional argument) is the batch number being run, default 1." << endl;
		return 1;
	}

	int batch = 0;


	if( argc == 5 )
	{
		batch = atoi( argv[4] );
		cerr << argv[1] << " " << argv[2] << " " << argv[3] << " " << argv[4] << endl;
	}
	else
	{
		cerr << argv[1] << " " << argv[2] << " " << argv[3] << endl;
	}

	srand( std::time(NULL) );

	return readWriteFile( atoi( argv[3] ), argv[1], argv[2], batch );

}


