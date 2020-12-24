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

void printPattern(const pitemset_t pattern, const vector<item_t> &items)
{
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
			//g_filebics << items[idx].idx + 1 << "=" << items[idx].value << " " ;
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
