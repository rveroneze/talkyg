#include "stdafx.h"
#include "globalsv.h"

data_t MVS = 999999; // value that represents a Missing Value
unsigned short g_output = 1; // format of the output file: 1 - matlab; 2 - python

unsigned g_cont = 0; // number of patterns in the output
ofstream g_filebics; // pointer to the output file
unordered_set<string> g_st(MAX_PATTERNS); // store the mined itemsets
