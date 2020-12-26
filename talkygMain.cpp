#include "stdafx.h"
#include "globalsv.h"
#include "Utils.h"
#include "talkyg.h"

bool readDataset(const string &dataSetName, dataset_t &matrix, row_t &n, col_t &m);
bool readConfigFile();
vector<item_t> createVerticalRepresentationWithFItems(const dataset_t &D, const row_t &n, const col_t &m, const row_t &minsup, deque<pnode_t> *root);

int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		cout << "\n!!! Wrong Arguments !!!" << endl << endl;
		cout << "List of the arguments:" << endl;
		cout << "1 - Dataset's filename;" << endl;
		cout << "2 - minsup;" << endl;
		cout << "3 - Output filename for the list of patterns;" << endl;
		exit(1);
	}

	if (!readConfigFile())
	{
		cout << "\nConfiguration file was not loaded.\n";
		cout << "Default configuration:" << endl;
		cout << "Value that represents a Missing Value: " << MVS << endl;
		cout << "Output format (1 - matlab; 2 - python): " << g_output << endl;
	}

	row_t minsup= atoi(argv[2]);

	// List the user parameters
	cout << "\nArguments: " << endl;
	cout << "Dataset's filename: " << argv[1] << endl;
	cout << "minsup: " << minsup << endl;
	cout << "File with the list of patterns: " << argv[3] << endl;

	dataset_t matrix; // pointer to the dataset
	row_t n; // number of dataset's rows
	col_t m; // number of dataset's columns
	if (!readDataset(argv[1], matrix, n, m))
	{
		cout << "\nDataset was not loaded!";
		exit(1);
	}
	printf("\nDataset loaded: %dx%d\n\n", n, m);

	cout << "Creating the vertical representation of the dataset..." << endl;
	deque<pnode_t> *root = new deque<pnode_t>;
	vector<item_t> items = createVerticalRepresentationWithFItems(matrix, n, m, minsup, root);
	cout << "Number of Frequent Items = " << items.size() << endl;

	float tempo;
	openPrintFile(argv[3]);
	cout << "\nRunning..." << endl;
	tempo = runTalkyG(items, root, minsup);
	closePrintFile();

	cout << "\n\nRuntime(s): " << tempo << endl;
	cout << "Number of patterns = " << g_cont << endl;

	// Guarda qtd de bics e runtime num txt
	ofstream myfile;
	myfile.open("summary.txt", ofstream::app);
	myfile << g_cont << '\t';
	myfile << tempo << '\t';
	myfile << endl;
	myfile.close();

	//system("pause");
	return 0;
}

bool readDataset(const string &dataSetName, dataset_t &matrix, row_t &n, col_t &m)
{
	ifstream myStream;
	myStream.open(dataSetName, ifstream::in);

	if (!myStream.is_open())
		return false;

	//Discovering the number of rows
	n = count(istreambuf_iterator<char>(myStream), istreambuf_iterator<char>(), '\n');

	//Discovering the number of columns
	data_t dbltmp;
	string line;
	m = 0;
	myStream.seekg(0);
	getline(myStream, line);
	stringstream stream(line);
	while (stream.good())
	{
		stream >> dbltmp;
		++m;
	}

	//Allocating memory
	matrix = new data_t*[n];
	for (row_t i = 0; i < n; ++i)
		matrix[i] = new data_t[m];

	//Storing the data
	myStream.seekg(0);
	for (row_t i = 0; i < n; ++i)
	{
		for (col_t j = 0; j < m; ++j)
			myStream >> matrix[i][j];
	}

	myStream.close();
	return true;
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

// Creates the vertical representation of dataset, ignoring the infrequent items
vector<item_t> createVerticalRepresentationWithFItems(const dataset_t &D, const row_t &n, const col_t &m, const row_t &minsup, deque<pnode_t> *root)
{
	col_t countItems = 0;
	vector<item_t> items;
	pair<data_t, row_t> *rwp = new pair<data_t, row_t>[n];
	for (col_t j = 0; j < m; ++j)
	{
		row_t qnmv = 0; // number of non-missing values
		for (row_t i = 0; i < n; ++i)
		{
			if (D[i][j] != MVS)
			{
				rwp[qnmv].first = D[i][j];
				rwp[qnmv].second = i;
				++qnmv;
			}
		}
		if (qnmv >= minsup)
		{
			sort(rwp, rwp + qnmv);
			row_t p1 = 0, p2 = 0;
			while (p1 <= qnmv - minsup)
			{
				while (p2 < qnmv - 1 && rwp[p2 + 1].first == rwp[p1].first) ++p2;
				row_t support = p2 - p1 + 1;
				//if (support >= minsup && support < n) // a 1-long itemset is a minimal generator iff its support<100%
				if (support >= minsup) // contrary to the authors, I will admit a 1-long itemset with support=100% as a minimal generator
				{
					item_t item;
					item.idx = j;
					item.value = rwp[p1].first;
					items.push_back(item);

					pnode_t node = new node_t;
					node->length = 1;
					node->idxItems = new col_t[1];
					node->idxItems[0] = countItems;
					node->sup = support;
					node->tidset = new row_t[node->sup];
					for (row_t i2 = p1; i2 <= p2; ++i2) node->tidset[i2 - p1] = rwp[i2].second;
					root->push_back(node);

					++countItems;
				}
				p1 = p2 + 1;
				p2 = p1;
			}
		}
	}

	/*
	cout << "Frequent Items:" << endl;
	for (col_t i = 0; i < items.size(); ++i)
	{
		cout << "Item " << i << ": ";
		cout << "idx = " << items[i].idx << ", ";
		cout << "value = " << items[i].value << endl;
	}
	cout << endl;
	cout << "root contains:" << endl;
	for (deque<pnode_t>::iterator it = root->begin(); it != root->end(); ++it)
	{
		cout << "idxItems = " << (*it)->idxItems[0] << ", ";
		cout << "sup = " << (*it)->sup << ", ";
		cout << "tidset = " ;
		for (row_t i = 0; i < (*it)->sup; ++i) cout << (*it)->tidset[i] << " ";
		cout << endl;
	}
	*/

	return items;
}
