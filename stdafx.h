#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>  // usado para o arquivo config.txt e para guardar os itemsets minerados
#include <forward_list>
#include <vector>
#include <deque>
#include <climits>

using namespace std;

typedef unsigned int data_t;
typedef data_t** dataset_t;

typedef size_t row_t;
typedef size_t col_t;

struct item_t
{
	col_t idx;
	data_t value;
};

struct node_t
{
	col_t length;
	col_t *idxItems;
	row_t sup;
	row_t *tidset;
	deque<node_t *> *children;
};

typedef node_t *pnode_t;


struct itemset_t
{
	col_t length;
	col_t *idxItems;
};