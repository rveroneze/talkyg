#include "talkyg.h"

float runTalkyG(const vector<item_t> &items, deque<pnode_t> *root, const row_t &minsup)
{
    g_st.reserve(NBUCKETS);
	clock_t clocks = clock();

    // loop over children of root from right-to-left
    for (deque<pnode_t>::reverse_iterator rit = root->rbegin(); rit!=root->rend(); ++rit)
    {
        // process the pattern
        printPattern( (*rit), items );

        // discover the subtree below rit
        extend(items, root, minsup, rit);
    }

	clocks = clock() - clocks;
	return ((float)clocks) / CLOCKS_PER_SEC;
}

void extend(const vector<item_t> &items, deque<pnode_t> *father, const row_t &minsup, deque<pnode_t>::reverse_iterator &curr)
{
    (*curr)->children = new deque<pnode_t>;

    // loop over siblings of curr from left-to-right
    for (deque<pnode_t>::iterator other = curr.base(); other!=father->end(); ++other)
    {
        pnode_t node = getNextGenerator(curr, other, minsup);
        if (node != NULL) (*curr)->children->push_back(node);
    }

    // loop over children of curr from right-to-left
    for (deque<pnode_t>::reverse_iterator child = (*curr)->children->rbegin(); child!=(*curr)->children->rend(); ++child)
    {
        // process the pattern
        printPattern( (*child), items );

        // discover the subtree below child
        extend(items, (*curr)->children, minsup, child);
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

pnode_t getNextGenerator(const deque<pnode_t>::reverse_iterator &curr, const deque<pnode_t>::iterator &other, const row_t &minsup)
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
    if (cand->sup < minsup || cand->sup == smaller)
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