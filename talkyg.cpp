#include "talkyg.h"

float runTalkyG(const vector<item_t> &items, deque<pnode_t> *root)
{
    g_st.reserve(NBUCKETS);
	clock_t clocks = clock();

    // loop over children of root from right-to-left
    for (deque<pnode_t>::reverse_iterator rit = root->rbegin(); rit!=root->rend(); ++rit)
    {
        // process the pattern
        printPattern( (*rit), items );

        // discover the subtree below rit
        extend(items, root, rit);
    }

	clocks = clock() - clocks;
	return ((float)clocks) / CLOCKS_PER_SEC;
}

void extend(const vector<item_t> &items, deque<pnode_t> *father, deque<pnode_t>::reverse_iterator &curr)
{
    (*curr)->children = new deque<pnode_t>;

    // loop over siblings of curr from left-to-right
    for (deque<pnode_t>::iterator other = curr.base(); other!=father->end(); ++other)
    {
        pnode_t node = getNextGenerator(curr, other);
        if (node != NULL) (*curr)->children->push_back(node);
    }

    // loop over children of curr from right-to-left
    for (deque<pnode_t>::reverse_iterator child = (*curr)->children->rbegin(); child!=(*curr)->children->rend(); ++child)
    {
        // process the pattern
        printPattern( (*child), items );

        // discover the subtree below child
        extend(items, (*curr)->children, child);
    }

    // Deallocating memory
    for (deque<pnode_t>::reverse_iterator child = (*curr)->children->rbegin(); child!=(*curr)->children->rend(); ++child)
    {
        // the pointer (*child)->idxItems is used in the hash
        delete [] (*child)->tidset;
        delete (*child);
    }
    delete (*curr)->children;
}

pnode_t getNextGenerator(const deque<pnode_t>::reverse_iterator &curr, const deque<pnode_t>::iterator &other)
{
    pnode_t cand = new node_t;

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
    if (cand->sup == smaller)
    {
        delete [] cand->tidset;
        delete cand;
        return NULL;
    }
    cand->biggerSup = getbiggerSup(cand->tidset, cand->sup);
    if (cand->biggerSup == 0)
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

    // TEST the candidate itemset: it has a proper subset with the same support in the hash?
    if (!IsGenerator(cand))
    {
        delete [] cand->tidset;
        delete [] cand->idxItems;
        delete cand;
        return NULL;
    }

    return cand;
}

// To compare less pairs of itemsets, I am using the tidset as the hash key
bool IsGenerator(const pnode_t &node)
{
    string s = "";
    char buffer[SIZE_BUFFER];
    for (row_t i = 0; i < node->sup; ++i)
    {
        snprintf(buffer, SIZE_BUFFER, "%d ", node->tidset[i]);
        s = s + buffer;
    }
    if (g_st.count(s) > 0)
    {
        for (forward_list<itemset_t>::iterator it = g_st[s].begin(); it != g_st[s].end(); ++it )
        {
            if ((*it).length < node->length)
            {
                if (includes(node->idxItems, node->idxItems+node->length, (*it).idxItems, (*it).idxItems+(*it).length) ) return false;
            }
        }
    }
    itemset_t itemset;
    itemset.length = node->length;
    itemset.idxItems = node->idxItems;
    g_st[s].push_front(itemset);
    return true;
}


// Compute the bigger support of a pattern
row_t getbiggerSup(const row_t *A, const row_t &size)
{
    unsigned short label;
	row_t *support = new row_t[g_maxLabel], biggerSup = 0;
	for (unsigned short i = 0; i < g_maxLabel; ++i) support[i] = 0; // initialize vector

     // counting the representativeness of each class label
	for (row_t i = 0; i < size; ++i)
    {
        label = g_classes[A[i]];
        ++support[label];
        if (support[label] >= g_minsups[label])
        {
            if (support[label] > biggerSup) biggerSup = support[label];
        }
    }

	delete [] support;

	return biggerSup;
}