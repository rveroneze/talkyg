#include "stdafx.h"
#include "globalsv.h"

data_t MVS = 999999; // value that represents a Missing Value
unsigned short g_output = 1; // format of the output file: 1 - matlab; 2 - python

unsigned g_cont = 0; // number of patterns in the output
ofstream g_filebics; // pointer to the output file
unordered_map<string, forward_list<itemset_t>> g_st; // store the mined itemsets to avoid redundancy

// ----- Variables for the search using class labels -----
unsigned short *g_classes; // vector to store the class label of each object
unsigned short g_maxLabel; // maximum label
double g_minconf = 0; // minimum confidence
unsigned short g_ignoreLabel; // patterns that represents label g_ignoreLabel will not be outputted
// --------------------------------------------------------