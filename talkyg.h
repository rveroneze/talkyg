#pragma once

#include "stdafx.h"
#include "globalsv.h"
#include "Utils.h"

float runTalkyG(const vector<item_t> &items, deque<pitemset_t> *root, const row_t &minsup);
void extend(const vector<item_t> &items, deque<pitemset_t> *father, const row_t &minsup, deque<pitemset_t>::reverse_iterator &curr);
pitemset_t getNextGenerator(const deque<pitemset_t>::reverse_iterator &curr, const deque<pitemset_t>::iterator &other, const row_t &minsup);
bool IsCanonical(const row_t *tidset, const row_t &minsup);