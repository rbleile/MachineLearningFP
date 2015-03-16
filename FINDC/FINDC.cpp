/* Includes Ours*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>


using namespace std;


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

float findC( string probFileName, vector<float> &y)
{
     int control = 0;
     int isIndex = 0;
     int counter = 0;
     int heavyside = 0;
     float C = 0.0;
     ifstream file( probFileName.c_str() );

     if( !file.is_open() )
     {
	  cerr << "Probability file failed to open" << endl;
	  return -1.0;
     }

     string line;
     string field;
     while( getline ( file, line ) )
     {
	  stringstream streamLine(line);
	  
	  if( control == 0 )
	  {
	      control++; 
	  }
	  else
	  {
	       while( getline( streamLine, field, ',' ) )
	       {
		    if( isIndex == 0){
			 isIndex++;
		    }
		    else
		    {
			 if( counter >= y.size() )
			 {
			      cerr << "Warning: There are more probabilities than expected values" << endl;
			      cerr << "Exiting Now" << endl;
			      return -1.0;
			 }

			 if( isIndex-1 >= y[counter] )
			 {
			      heavyside = 1;
			 }
			 else
			 {
			      heavyside = 0;
			 }
			 C += ( atof( field.c_str() ) - heavyside )*( atof( field.c_str() ) - heavyside );
			 /*
			 cerr << "(P(y<=" << isIndex-1 << ") - H(" << isIndex-1 << "-" << y[counter] << "))^2 = ";
			 cerr << ( atof( field.c_str() ) - heavyside )*( atof( field.c_str() ) - heavyside ) << ". Where P(y<=" << isIndex-1 << ") = ";
			 cerr << field << " and H(" << isIndex-1 << "-" << y[counter] << ") = " << heavyside << "." << endl;
			 */
			 isIndex++;
		    }
	       }//close inner while
	       isIndex = 0;
	       counter++;
	  }//close else
     }//close outside while
     file.close();
     if( counter < y.size() -1 )
     {
	  cerr << "Warning: There are more expected values than probabilities" << endl;
	  cerr << "Exiting Now" << endl;
	  return -1.0;
     }
     C /= 70.0*counter;
     return C;

}//close readprob/compute C

int main( int argc, char** argv )
{

	if( argc != 4 )
	{
		cerr << "Need arguments: run function like this:\n" << endl;
	        cerr << "./<executable> <expected_values_file_name> <probability_output_file_name> <output_file_name>" << endl; 
		cerr << "\t" << "<executable> should be pretty self explanatory." << endl;
		cerr << "\t" << "<expected_values_file_name>(string) is the name of file containing expected values for each data points." << endl;
		cerr << "\t" << "<probability_output_file_name>(string) is the name of the file containing the cumulative probabilities for rainfall." << endl;
		cerr << "\t" << "<output_file_name>(string) is the name of the file the value of C will be stored in." << endl;
		return 1;
	}

	//cerr << argv[1] << " " << argv[2] << " " << argv[3] << endl;

	cout << "Beginning" << endl;

        float Cvalue = 0.0;

	string expectedFileName = argv[1];
	string probFileName  = argv[2];
	string outFileName = argv[3]; 

        vector<float> Expected;

        cout << "Reading Expected Values" << endl;
	readExpected( expectedFileName, &Expected );
        cout << "Expected Values Read" << endl << "Computing C" << endl;
	  
        Cvalue = findC( probFileName, Expected );

        if( Cvalue == -1.0 ) return 1;

        cout << "C computed, printing results" << endl;

        
        FILE * outFile = fopen( outFileName.c_str(), "w" );
        if(outFile == NULL)
        {
	        cerr << "Output file failed to open" << endl;
	        return 1;
        }
        else{
	        fprintf( outFile, "Probability file used:\t%s\n", probFileName.c_str());
	        fprintf( outFile, "Expected values file used:\t%s\n", expectedFileName.c_str());
	        fprintf( outFile, "Value of C:\t%g", Cvalue);
        }
	fclose( outFile );   
        
        /*ofstream outFile( outFileName.c_str() );
        if( !outFile.is_open() )
        {
	        cerr << "Output file failed to open" << endl;
	        return 1;
        }
        else
        {
	        outFile << "Probability file used:\t" << probFileName << endl;
	        outFile << "Expected values file used:\t" << expectedFileName << endl << endl;
	        outFile << "Value of C:\t" << Cvalue << endl;
        }
        outFile.close();

        */
        cout << "Results Written, Exiting." << endl;
        return 0;

}
