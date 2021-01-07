#pragma once

#include "stdafx.h"
#include "globalsv.h"
#include "Utils.h"

float runTalkyG(const vector<item_t> &items, deque<pnode_t> *root);
void extend(const vector<item_t> &items, deque<pnode_t> *father, deque<pnode_t>::reverse_iterator &curr);
pnode_t getNextGenerator(const deque<pnode_t>::reverse_iterator &curr, const deque<pnode_t>::iterator &other);
bool IsGenerator(const pnode_t &node);
row_t getbiggerSup(const row_t *A, const row_t &size);