/* Includes Ours*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>

/* Includes MLPACK*/
#include <mlpack/core.hpp>

#include "neighbor_search.hpp"
#include "unmap.hpp"

using namespace std;
using namespace mlpack;
using namespace mlpack::neighbor;
using namespace mlpack::tree;

/*
int readFile( char* rFileName, char* qFileName )
{

	ifstream file( rFileName );

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
*/
int main( int argc, char** argv )
{

	if( argc != 5 )
	{
		cerr << "Need arguments: run function like this:\n./<executable> <input_file_name> <output_file_name> <is_train_data>" << endl;
		cerr << "\t" << "<executable> should be pretty self explanatory." << endl;
		cerr << "\t" << "<train_file_name>(string) is the training file in .csv format." << endl;
		cerr << "\t" << "<test_file_name>(string) is the testing for output files." << endl;
		cerr << "\t" << "<num_neighbors>(int) is the number of neighbors for nearest neighbor search." << endl;
		cerr << "\t" << "<output_base_file_name>(string) is the basename for output files." << endl;
		return 1;
	}

	cerr << argv[1] << " " << argv[2] << " " << argv[3] << " " << argv[4] << endl;

	cout << "Begining " << argv[3] << "-Nearest Neighbor Search Algorithm" << endl;

	string trainFile = argv[1];
	string testFile  = argv[2];
	int k = atoi( argv[3] );
	string outputFileName = argv[4];

	cout << "Reading data ( Load into arma::mat )" << endl;

	arma::mat trainData;
	arma::mat testData;
	data::Load( trainFile, trainData, true);
	data::Load( testFile,  testData,  true);

	cout << "Data Read" << endl;

	if (k > trainData.n_cols)
	{
		cerr << "Invalid k: " << k << "; must be greater than 0 and less ";
		cerr << "than or equal to the number of train points (";
		cerr << trainData.n_cols << ")." << endl;

		return 1;
	}

	arma::Mat<size_t> neighbors;
	arma::mat distances;

	// Because we may construct it differently, we need a pointer.
    AllkNN* allknn = NULL;

    // Mappings for when we build the tree.
    std::vector<size_t> oldFromNewRefs;

    // Build trees by hand, so we can save memory: if we pass a tree to
    // NeighborSearch, it does not copy the matrix.

	size_t leafSize = trainData.n_cols;
    std::vector<size_t> oldFromNewQueries;
    size_t leafSize2 = testData.n_cols;

	cout << "Building Binary Space Trees for Nearest Neighbor search" << endl;

    BinarySpaceTree< bound::HRectBound<2>, NeighborSearchStat<NearestNeighborSort> > refTree( trainData, oldFromNewRefs, leafSize );
    BinarySpaceTree<bound::HRectBound<2>, NeighborSearchStat<NearestNeighborSort> > queryTree( testData, oldFromNewQueries, leafSize2);

	cout << "Trees built" << endl;

    // Build trees by hand, so we can save memory: if we pass a tree to
    // NeighborSearch, it does not copy the matrix.
	bool singleMode = false;
    
	cout << "Building allknn data structure" << endl;

	allknn = new AllkNN(&refTree, &queryTree, trainData, testData, singleMode);

	arma::mat distancesOut;
	arma::Mat<size_t> neighborsOut;

	cout << "Doing Nearest Neighbor search" << endl;

	allknn->Search(k, neighborsOut, distancesOut);

	cout << "Finished Search" << endl;

	cout << "Unmapping Data" << endl;

	if( !singleMode )
	{
		Unmap(neighborsOut, distancesOut, oldFromNewRefs, oldFromNewQueries, neighbors, distances);
	}
	else
	{
		Unmap(neighborsOut, distancesOut, oldFromNewRefs, neighbors, distances);
	}

	cout << "Cleaning Up allknn data structure" << endl;

	if( allknn )
		delete allknn;

	bool saveFile = true;

	if( saveFile )
	{

		cout << "Saving File" << endl;

		string distancesFile = "distances_"+outputFileName;
		string neighborsFile = "neighbors_"+outputFileName;

		data::Save(distancesFile, distances);
		data::Save(neighborsFile, neighbors);
	}
}
