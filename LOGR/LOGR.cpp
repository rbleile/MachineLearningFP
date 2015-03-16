#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>

#include <mlpack/core.hpp>
#include "logistic_regression.hpp"

#include <mlpack/core/optimizers/sgd/sgd.hpp>

using namespace std;
using namespace mlpack;
using namespace mlpack::regression;
using namespace mlpack::optimization;

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

int main(int argc, char** argv)
{

	if( argc < 5 )
	{
		cerr << "Need arguments: run function like this:\n" << endl;
		cerr << "\t" << "<executable> should be pretty self explanatory." << endl;
		cerr << "\t" << "<train_file_name>(string) is the training file in .csv format." << endl;
		cerr << "\t" << "<expected_values_file_name>(string) is the name of file containing expected values for each data points." << endl;
		cerr << "\t" << "<test_file_name>(string) is the testing for output files." << endl;
		cerr << "\t" << "<output_base_file_name>(string) is the basename for output files." << endl;
		cerr << "\t" << "<lambda>            'optional argument' list giving l2-reg param. default 0" << endl;
		cerr << "\t" << "<tolerance>         'optional argument' list giving convergance tolerance. defialt 1e-10"<< endl;
		cerr << "\t" << "<maxIterations>     'optional argument' list' giving maximum number of iterations. defualt 1000"<< endl;
		cerr << "\t" << "<decision boundary> 'optional argument' list giving decision boundary for test decisions. defialt 0.5"<< endl;
		return 1;			
	}


	// Collect command-line options.
	const string inputFile			= argv[1];
	const string inputResponsesFile	= argv[2];
	const string testFile			= argv[3];
	const string outputFile			= argv[4];

	double lambda = 0.1;
	double tolerance = 1e-10;
	size_t maxIterations = 1000;
	double decisionBoundary = 0.5;
	const double stepSize = 0.01;
	if( argc == 9 )
	{
		lambda = atof( argv[5] );
		tolerance = atof( argv[6] );
		maxIterations = atoi( argv[7] );
		decisionBoundary = atof( argv[8] );	
	}

//	const string optimizerType = "lbfgs";
	  const string optimizerType = "sgd";
	//const double stepSize = 0.01;

	vector<float> ExpectedVals;
	vector<int> ExpectedBin;

	readExpected( inputResponsesFile, &ExpectedVals);

	ExpectedBin.resize( ExpectedVals.size(), 0 );

	for( int i = 0; i < 70; i++ )
	{

		for( int j = 0; j < ExpectedVals.size(); j++ )
		{
			ExpectedBin[j] = ( ExpectedVals[j] <= i ) ? 1.0  : 0.0;
		}

		arma::mat regressors;
		data::Load(inputFile, regressors, true);

		arma::mat responses;
		responses = arma::conv_to<arma::mat>::from( ExpectedBin ); 

		if (responses.n_rows == 1)
			responses = responses.t();
		if (responses.n_rows != regressors.n_cols)
		{
			cerr << "The responses (--input_responses) must have the same " << "number of points as the input dataset (--input_file)." << endl;
			return 0;
		}

		
		arma::mat model;

		// We need to train the model.  Prepare the optimizers.
		arma::vec responsesVec = responses.unsafe_col(0);//arma::conv_to<arma::vec>::from( ExpectedBin );

		LogisticRegressionFunction lrf(regressors, responsesVec, lambda);
		// Set the initial point, if necessary.

		if (optimizerType == "lbfgs")
		{
			L_BFGS<LogisticRegressionFunction> lbfgsOpt(lrf);
			lbfgsOpt.MaxIterations() = maxIterations;
			lbfgsOpt.MinGradientNorm() = tolerance;
			cerr << "Training model with L-BFGS optimizer." << endl;

			// This will train the model.
			LogisticRegression<L_BFGS> lr(lbfgsOpt);

			cerr << "Parameters" << endl;
			// Extract the newly trained model.
			model = lr.Parameters();
		}
		else if (optimizerType == "sgd")
		{
			SGD<LogisticRegressionFunction> sgdOpt(lrf);
			sgdOpt.MaxIterations() = maxIterations;
			sgdOpt.Tolerance() = tolerance;
			sgdOpt.StepSize() = stepSize;
			cerr << "Training model with SGD optimizer." << endl;

			  // This will train the model.
			LogisticRegression<SGD> lr(sgdOpt);
			// Extract the newly trained model.
			model = lr.Parameters();
		}

		arma::mat testSet;
		data::Load(testFile, testSet, true);

		arma::vec predictions;

		stringstream ss;
		ss << i << "_" << outputFile;

		string outputPredictionsFile = ss.str();

		// We must perform predictions on the test set.  Training (and the
		// optimizer) are irrelevant here; we'll pass in the model we have.
		LogisticRegression<> lr(model);

		cerr << "Predicting classes of points in '" << testFile  << " for " << i << "'."<< endl;
		

		//model.print();
		cout << endl;

		lr.Predict(testSet, predictions, decisionBoundary);

		data::Save(outputPredictionsFile, predictions, false, false);
	}
}
