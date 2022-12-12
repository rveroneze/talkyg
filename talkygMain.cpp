#include "stdafx.h"
#include "globalsv.h"
#include "Utils.h"
#include "DataUtils.h"
#include "talkyg.h"

bool readConfigFile();

int main(int argc, char* argv[])
{
	if (argc != 7)
	{
		cout << "\n!!! Wrong Arguments !!!" << endl << endl;
		cout << "List of the arguments:" << endl;
		cout << "1 - Dataset's filename;" << endl;
		cout << "2 - Dataset's type (1 - Binary, 2 - Integer, 3 - Transactional);" << endl;
		cout << "3 - Name of the file with the minsup values for each class label;" << endl;
		cout << "4 - Output filename for the list of patterns;" << endl;
		cout << "5 - Class labels' filename;" << endl;
		cout << "6 - Minimum confidence [0,1];" << endl;
		exit(1);
	}

	if (!readConfigFile())
	{
		cout << "\nConfiguration file was not loaded.\n";
		cout << "Default configuration:" << endl;
		cout << "Value that represents a Missing Value: " << MVS << endl;
		cout << "Output format (1 - matlab; 2 - python): " << g_output << endl;
	}

    string dataFileName = argv[1], minsupFileName = argv[3], bicsFileName = argv[4], labelFileName = argv[5];
    short dataType = atoi(argv[2]);
	g_minconf = atof(argv[6]);

	// List the user parameters
	cout << "\nArguments: " << endl;
	cout << "Dataset's filename: " << dataFileName << endl;
	cout << "Dataset's type (1 - Binary, 2 - Integer, 3 - Transactional): " << dataType << endl;
	cout << "minsup: " << minsupFileName << endl;
	cout << "File with the list of patterns: " << bicsFileName << endl;
	cout << "Class labels' filename: " << labelFileName << endl;
	cout << "Minimum confidence: " << g_minconf << endl;

	dataset_t matrix; // pointer to the dataset
	row_t n; // number of dataset's rows
	col_t m; // number of dataset's columns
    cout << "\nLoading data..." << endl;
    if (dataType == 1 || dataType == 2)
    {
        if (!readTabularDataset(dataFileName, matrix, n, m, dataType))
        {
            cout << "Dataset was not loaded!";
            exit(1);
        }
        printf("Dataset loaded: %ldx%ld\n\n", n, m);
        //printData(matrix, n, m);
    }
    else if (dataType == 3)
    {
        if (!readTransDataset(dataFileName, matrix, n, m))
        {
            cout << "Dataset was not loaded!";
            exit(1);
        }
        printf("Transaction dataset loaded: %ld x %ld\n", n, m);
        //printData(matrix, n, m);
    }

	// Read the class label of each sample
	g_classes = new unsigned short[n];
	if (!readClassLabels(labelFileName, n))
	{
		cout << "\nClass labels' file was not loaded!";
		exit(1);
	}
	printf("Class labels loaded\n\n");

	// Read the minsup of each class label
	if (!readMinSupsFile(minsupFileName, n))
	{
		cout << "\nminsups' file was not loaded!";
		exit(1);
	}
	printf("minsups loaded\n\n");

	cout << "Creating the vertical representation of the dataset..." << endl;
	deque<pnode_t> *root = new deque<pnode_t>;
	vector<item_t> items = createVerticalRepresentationWithFItems(matrix, n, m, root);
	cout << "Number of Frequent Items = " << items.size() << endl;

	float tempo;
	openPrintFile(bicsFileName);
	cout << "\nRunning..." << endl;
	tempo = runTalkyG(items, root);
	closePrintFile();

	cout << "\n\nRuntime(s): " << tempo << endl;
	cout << "Number of patterns = " << g_cont << endl;

	// Guarda qtd de bics e runtime num txt
	ofstream myfile;
	myfile.open("summary.txt", ofstream::app);
	myfile << g_cont << '\t'; // Number of patterns
	myfile << tempo << '\t'; // Runtime in seconds
	myfile << dataFileName << '\t'; // Dataset's filename
	myfile << dataType << '\t'; // Dataset's type
	myfile << minsupFileName << '\t'; // minsup
	myfile << bicsFileName << '\t'; // File with the list of patterns
	myfile << labelFileName << '\t'; // Class labels' filename
	myfile << g_minconf << '\t'; // Minimum confidence
	myfile << endl;
	myfile.close();

	//system("pause");
	return 0;
}

bool readConfigFile()
{
	unordered_map<string,string> params;
	ifstream myStream;
	string line;

	myStream.open("config.txt", ifstream::in);

	if (!myStream.is_open())
		return false;
		

	myStream.seekg(0);
	while (myStream.good())
	{
		getline(myStream, line);
		size_t found = line.find("=");
		if (found!=string::npos)
		{
			params[line.substr(0,found)] = line.substr(found+1);
		}
	}

	cout << "myparams contains:" << endl;
	for (auto& x: params)
		cout << x.first << ": " << x.second << endl;
	
	// Setando as variaveis com os parametros
	MVS = stoi(params["MVS"]);
	if (params["OUT"].compare("matlab") == 0)
		g_output = 1;
	else
		g_output = 2;

	myStream.close();
	return true;
}
