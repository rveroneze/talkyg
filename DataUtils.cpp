#include "DataUtils.h"

bool readTabularDataset(const string &dataSetName, dataset_t &matrix, row_t &n, col_t &m, const short &option)
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
		{
			myStream >> matrix[i][j];
			if (option == 1 && matrix[i][j] == 0) matrix[i][j] = MVS; // if reading a binary dataset...
		}
	}

	myStream.close();
	return true;
}

bool readTransDataset(const string &dataSetName, dataset_t &matrix, row_t &n, col_t &m)
{
	ifstream myStream;
	myStream.open(dataSetName, ifstream::in);

	if (!myStream.is_open())
		return false;

	//Discovering the number of rows
	n = count(istreambuf_iterator<char>(myStream), istreambuf_iterator<char>(), '\n');

	//Discovering the number of columns
	row_t maior = 0, menor = LONG_MAX;
    string line;
    col_t dbltmp;
    myStream.seekg(0);
    for (row_t i = 0; i < n; ++i)
    {
        getline(myStream, line);
        stringstream stream(line);
        while (stream.good())
        {
            stream >> dbltmp;
			if (dbltmp < 0)
            {
                cout << "PROBLEM IN THE INPUT FILE: There are values less than zero in the input file.";
                myStream.close();
                return false;
            }
            if (dbltmp > maior) maior = dbltmp;
			if (dbltmp < menor) menor = dbltmp;
        }
    }
	m = maior - menor + 1;
    //cout << "m = " << m << endl;

	//Allocating memory and Initializing
	matrix = new data_t*[n];
	for (row_t i = 0; i < n; ++i)
	{
		matrix[i] = new data_t[m];
		for (col_t j = 0; j < m; ++j) matrix[i][j] = MVS;
	}

	//Storing the data
	myStream.seekg(0);
    for (row_t i = 0; i < n; ++i)
    {
        getline(myStream, line);
        stringstream stream(line);
        while (stream.good())
        {
            stream >> dbltmp;
			matrix[i][dbltmp - menor] = 1;
        }
    }

	myStream.close();
	return true;
}

void printData(const dataset_t &matrix, const row_t &n, const col_t &m)
{
	for (row_t i = 0; i < n; ++i)
	{
		for (col_t j = 0; j < m; ++j)
			cout << matrix[i][j] << '\t';
		cout << endl;
	}
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
				if (support >= minsup && support < n) // a 1-long itemset is a minimal generator iff its support<100%
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

// Read tha class label of each object, and set g_maxLabel
bool readClassLabels(const string &fileName, const row_t &n)
{
	g_maxLabel = 0;

	ifstream myStream;
	myStream.open(fileName, ifstream::in);

	if (!myStream.is_open())
		return false;

	//Storing the data
	myStream.seekg(0);
	for (row_t i = 0; i < n; ++i)
	{
		myStream >> g_classes[i];
		if (g_classes[i] > g_maxLabel) g_maxLabel = g_classes[i];
	}

	myStream.close();
	++g_maxLabel;

	/*
	cout << "Label of each sample:" << endl;
	for (row_t i = 0; i < n; ++i) cout << "Sample " << i << ": " << g_classes[i] << endl;
	cout << "g_maxLabel: " << g_maxLabel << endl;
	*/

	return true;
}