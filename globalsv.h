#pragma once

#include "stdafx.h"

#define NBUCKETS 1000000
#define SIZE_BUFFER 20

extern data_t MVS;
extern unsigned short g_output;

extern unsigned g_cont;
extern ofstream g_filebics;
extern unordered_map<string, forward_list<itemset_t>> g_st;

// ----- Variables for the search using class labels -----
extern unsigned short *g_classes;
extern unsigned short g_maxLabel;
extern double g_minconf;
extern row_t *g_minsups;
extern row_t g_smallerMinsup, g_biggerMinsup;
// --------------------------------------------------------
