#include "talkyg.h"

float runTalkyG(const vector<item_t> &items, deque<pitemset_t> *root, const row_t &minsup)
{
	clock_t clocks = clock();

    // loop over children of root from right-to-left
    for (deque<pitemset_t>::reverse_iterator rit = root->rbegin(); rit!=root->rend(); ++rit)
    {
        // process the itemset
        printPattern( (*rit), items );

        // discover the subtree below rit
        extend(items, root, minsup, rit);
    }

	clocks = clock() - clocks;
	return ((float)clocks) / CLOCKS_PER_SEC;
}

void extend(const vector<item_t> &items, deque<pitemset_t> *father, const row_t &minsup, deque<pitemset_t>::reverse_iterator &curr)
{
    (*curr)->children = new deque<pitemset_t>;

    // loop over siblings of curr from left-to-right
    for (deque<pitemset_t>::iterator other = curr.base(); other!=father->end(); ++other)
    {
        pitemset_t itemset = getNextGenerator(curr, other, minsup);
        if (itemset != NULL) (*curr)->children->push_back(itemset);
    }

    // loop over children of curr from right-to-left
    for (deque<pitemset_t>::reverse_iterator child = (*curr)->children->rbegin(); child!=(*curr)->children->rend(); ++child)
    {
        // process the itemset
        printPattern( (*child), items );

        // discover the subtree below child
        extend(items, (*curr)->children, minsup, child);
    }

    // Deallocating memory
    for (deque<pitemset_t>::reverse_iterator child = (*curr)->children->rbegin(); child!=(*curr)->children->rend(); ++child)
    {
        delete [] (*child)->idxItems;
        delete [] (*child)->tidset;
        delete (*child);
    }
    delete (*curr)->children;
}

pitemset_t getNextGenerator(const deque<pitemset_t>::reverse_iterator &curr, const deque<pitemset_t>::iterator &other, const row_t &minsup)
{
    pitemset_t cand = new itemset_t;

    // COMPUTE the candidate tidset
    row_t *aux;
    row_t smaller;
    if ((*curr)->sup > (*other)->sup)
    {
        smaller = (*other)->sup;
        cand->tidset = new row_t[(*curr)->sup];
    }
    else
    {
        smaller = (*curr)->sup;
        cand->tidset = new row_t[(*other)->sup];
    }
	aux = set_intersection ((*curr)->tidset, (*curr)->tidset+(*curr)->sup, (*other)->tidset, (*other)->tidset+(*other)->sup, cand->tidset);
	cand->sup = aux - cand->tidset;
    
    // TEST the candidate tidset
    if (cand->sup < minsup || cand->sup == smaller || !IsCanonical(cand->tidset, cand->sup))
    {
        delete [] cand->tidset;
        delete cand;
        return NULL;
    }

    // COMPUTE the candidate itemset
    col_t i;
    cand->length = (*curr)->length + 1;
    cand->idxItems = new col_t[cand->length];
    for (i = 0; i < (*curr)->length; ++i) cand->idxItems[i] = (*curr)->idxItems[i];
    cand->idxItems[i] = (*other)->idxItems[i-1];

    return cand;
}

bool IsCanonical(const row_t *tidset, const row_t &sup)
{
    string s = "";
    char buffer[SIZE_BUFFER];
    for (row_t i = 0; i < sup; ++i)
    {
        snprintf(buffer, SIZE_BUFFER, "%d ", tidset[i]);
        s = s + buffer;
    }
    if (g_st.count(s) > 0) return false;
    return true;
}