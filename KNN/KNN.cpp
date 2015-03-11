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

int readExpected( string expectedFileName, vector<float>* expected )
{

	ifstream file( expectedFileName.c_str() );

	if( !file.is_open() )
	{
		cerr << "Expected file failed to open" << endl;
		return 1;
	}

	string line;

	while( getline( file, line ) )
	{
		stringstream streamLine(line);
		expected->push_back( atof( line.c_str() ) );
	}

	file.close();

	return 0;
}

int binData( int numN, int test_size, string outFile, arma::Mat<size_t> &neighbors, vector<float> *expected )
{
	//Need to loop over expected->size() and for each we need to loop over numN in neighbors( i, j ) and add to  bin where bins are 0 - 70?
	int cumulativeBins;
	int totBins;
	int bins[71];

	stringstream ss;
	ss << "prob_" << outFile;
	string probOutFileName = ss.str();

	ofstream file( probOutFileName.c_str() );

	if( !file.is_open() )
	{
		cerr << "Prob file failed to open" << endl;	
		return 1;
	}

	file << "Id" << endl;
	for( int c = 0; c < 70; c++ )
	{
		bins[c] = 0;
		file << "\tP(y<" << c << ")";
	}
	file << endl;

	for(int i = 0; i < test_size; i++)
	{
		for( int j = 0; j < numN; j++ )
		{
			int index = neighbors( j, i );
			float val = expected->at(index);

			if( val <= 0 )
			{
				bins[ 0 ]++;
			}
			else if( val > 69 )
			{
				bins[ 70 ]++;	
			}
			else
			{
				bins[ (int) (val+1) ];
			}
		}

		totBins = 0;

		for( int c = 0; c < 70; c++ )
		{
			totBins += bins[c];
		}

		cumulativeBins = 0;

		file << i;
		for( int c = 0; c < 70; c++ )
		{
			cumulativeBins += bins[c];
			file << "\t" << (float)cumulativeBins/(float)totBins;
		}
		file << endl;

	}

	return 0;
	
}

int main( int argc, char** argv )
{

	if( argc != 6 )
	{
		cerr << "Need arguments: run function like this:\n./<executable> <input_file_name> <output_file_name> <is_train_data>" << endl;
		cerr << "\t" << "<executable> should be pretty self explanatory." << endl;
		cerr << "\t" << "<train_file_name>(string) is the training file in .csv format." << endl;
		cerr << "\t" << "<test_file_name>(string) is the testing for output files." << endl;
		cerr << "\t" << "<num_neighbors>(int) is the number of neighbors for nearest neighbor search." << endl;
		cerr << "\t" << "<output_base_file_name>(string) is the basename for output files." << endl;
		cerr << "\t" << "<expected_values_file_name>(string) is the name of file containing expected values for each data points." << endl;
		return 1;
	}

	cerr << argv[1] << " " << argv[2] << " " << argv[3] << " " << argv[4] << " " << argv[5] << endl;

	cout << "Begining " << argv[3] << "-Nearest Neighbor Search Algorithm" << endl;

	string trainFile = argv[1];
	string testFile  = argv[2];
	int k = atoi( argv[3] );
	string outputFileName = argv[4];
	string expectedFilename = argv[5];

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


	cout << "Reading Expected Data" << endl;

	vector<float> Expected;

	int stat1 = readExpected( expectedFilename, &Expected );
	if( stat1 == 1 )
	{
		cerr << "Error reading file - exiting" << endl;
		return 1;
	}

	cout << "Binning Data" << endl;
	cout << "Binning Data and Writing Probabilities" << endl;
	
	int stat2 = binData( k, testData.n_cols, outputFileName, neighbors, &Expected );
	if( stat2 == 1 )
	{
		cerr << "Error reading file - exiting" << endl;
		return 1;
	}

	cout << "Probability file written - writting neighbors file" << endl;

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
