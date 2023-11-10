#ifndef APRIORI
#define APRIORI

#include <map>
#include <set>

using namespace std;

class ksized_counter{
    public:
	map<set<int>,int> counter; /* the count of an itemset */
	int k; /* the size of the itemset */

    ksized_counter(){}

    ksized_counter(int size){ k = size; }
};

int build_freq1(vector<set<Tuple*, tuple_set_comparator> > &database, vector<ksized_counter> &all_freqk, double min_support, set<set<int> > &freq_itemsets);

void generate_cand(ksized_counter &new_cands, ksized_counter &old_cands);

int select_counting_algorithm(int d, int k);

void subset_finder(set<int> &sset, int size, int left_size, set<int>::iterator index, ksized_counter &candk, set<int> &temp);

void keep_freq(vector<set<Tuple*, tuple_set_comparator> > &database, ksized_counter &candk, vector<ksized_counter> &all_freqk, double min_support, set<set<int> > &freq_itemsets);

int ksized_number(int n, int k);

#endif