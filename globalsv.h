#pragma once

#include "stdafx.h"

#define NBUCKETS 1000000
#define SIZE_BUFFER 20

extern data_t MVS;
extern unsigned short g_output;

extern unsigned g_cont;
extern ofstream g_filebics;
extern unordered_map<string, forward_list<itemset_t>> g_st;
