#include "stdafx.h"
#include "Utils.h"

void openPrintFile(const string &filename)
{
	g_filebics.open(filename);
	if (g_output == 2) // python
	{
		g_filebics << "#!/usr/bin/env python3" << endl << endl;
		g_filebics << "bics = []" << endl;
	}
}

void printPattern(const pnode_t pattern, const vector<item_t> &items)
{
	if (g_minconf > 0 && getConfidence(pattern->tidset, pattern->sup) < g_minconf)
		return;

	++g_cont;

	if (g_output == 1) // matlab
	{
		g_filebics << "A{" << g_cont << "} = [";
		for (row_t i = 0; i < pattern->sup; ++i)
			g_filebics << pattern->tidset[i] + 1 << " ";
		g_filebics << "];\nB{" << g_cont << "} = [";
		for (col_t i = 0; i < pattern->length; ++i)
		{
			col_t idx = pattern->idxItems[i];
			// g_filebics << items[idx].idx + 1 << "=" << items[idx].value << " " ;
			g_filebics << items[idx].idx + 1 << " " ;
		}		
		g_filebics << "];\n";
	}
	else // python
	{
		g_filebics << "bics.append([[";
		for (row_t i = 0; i < pattern->sup; ++i)
			g_filebics << pattern->tidset[i]  << ",";
		g_filebics << "],[";
		for (col_t i = 0; i < pattern->length; ++i)
		{
			col_t idx = pattern->idxItems[i];
			g_filebics << items[idx].idx << ",";
		}
		g_filebics << "]])" << endl;
	}
}

void closePrintFile()
{
	g_filebics.close();
}



// Compute the confidence of a pattern
double getConfidence(const row_t *A, const row_t &size)
{
	row_t *contClass = new row_t[g_maxLabel];
	for (unsigned short i = 0; i < g_maxLabel; ++i) contClass[i] = 0; // initialize vector

	for (row_t i = 0; i < size; ++i) ++contClass[ g_classes[A[i]] ]; // counting the representativeness of each class label
	
	row_t maior = 0;
	unsigned short label;
	for (unsigned short i = 0; i < g_maxLabel; ++i)
	{
		if (contClass[i] > maior)
		{
			maior = contClass[i];
			label = i;
		}
	}

	/*
	cout << "Confidence" << endl;
	for (row_t i = 0; i < size; ++i)
	{
		cout << "Sample " << A[i] << ": " << g_classes[A[i]] << endl;
	}
	cout << maior << "/" << size << "=" << maior / (double) size << endl;
	*/

	delete [] contClass;

	if (g_ignoreLabel == label) return 0;
	return maior / (double) size;
}