#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>  // usado para o arquivo config.txt
#include <unordered_set> // usado para guardar os itemsets minerados
#include <vector>
#include <deque>

using namespace std;

typedef unsigned int data_t;
typedef data_t** dataset_t;

typedef unsigned int row_t;
typedef unsigned int col_t;

struct item_t
{
	col_t idx;
	data_t value;
};

struct itemset_t
{
	col_t length;
	col_t *idxItems;
	row_t sup;
	row_t *tidset;
	deque<itemset_t *> *children;
};

typedef itemset_t *pitemset_t;
