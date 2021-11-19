#pragma once

#include "stdafx.h"
#include "globalsv.h"

bool readTabularDataset(const string &dataSetName, dataset_t &matrix, size_t &n, size_t &m, const short &option);
bool readTransDataset(const string &dataSetName, dataset_t &matrix, size_t &n, size_t &m);
void printData(const dataset_t &matrix, const row_t &n, const col_t &m);
vector<item_t> createVerticalRepresentationWithFItems(const dataset_t &D, const row_t &n, const col_t &m, const row_t &minsup, deque<pnode_t> *root);
bool readClassLabels(const string &fileName, const row_t &n);