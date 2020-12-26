#pragma once

#include "stdafx.h"
#include "globalsv.h"
#include "Utils.h"

float runTalkyG(const vector<item_t> &items, deque<pnode_t> *root, const row_t &minsup);
void extend(const vector<item_t> &items, deque<pnode_t> *father, const row_t &minsup, deque<pnode_t>::reverse_iterator &curr);
pnode_t getNextGenerator(const deque<pnode_t>::reverse_iterator &curr, const deque<pnode_t>::iterator &other, const row_t &minsup);
bool IsGenerator(const pnode_t &node);