#ifndef ITEMSETUTILITIES
#define ITEMSETUTILITIES

#include <set>
using namespace std;

void intersection_set(set<int> set1, set<int> set2, set<int> &intersectionset);

void union_set(set<int>::const_iterator iter1, set<int>::const_iterator end1, set<int>::const_iterator iter2, set<int>::const_iterator end2, set<int> &unionset);

class Itemset {
	public:
	set<int> values;

	Itemset(set<int> starting_values);

	Itemset();

};

bool operator< (Itemset const& itemset, Itemset const& itemset2);

bool operator== (Itemset const& itemset, Itemset const& itemset2);

class Tuple {
	public:
	int transaction_index;
	int item;
	int state;
	double ratio;

	Tuple(int index, int value);

	Tuple(int index, int value, double cost);
};

struct itemset_set_comparator
{
    // queue elements are vectors so we need to compare those
    bool operator()(Itemset* const& a, Itemset* const& b) const
    {
		if (a->values.size() != b->values.size()) return true;
		//if (a->ratio != b->ratio) return true;
		bool equal = false;
		for (int item1 : a->values){
			for (int item2 : b->values){
				if (item1 == item2){
					equal = true;
				}
			}
			if (equal == false) return true;
			equal = false;
		}
		return false;
    }
};

struct itemset_map_comparator
{
    // queue elements are vectors so we need to compare those
    bool operator()(Itemset const& a, Itemset const& b) const
    {
		return a < b;
		if (a.values.size() < b.values.size()) return a.values.size() < b.values.size();
		else if (a.values.size() > b.values.size()) return a.values.size() > b.values.size();
		else {
			bool equal = false;
			for (int item1 : a.values){
				for (int item2 : b.values){
					if (item1 == item2){
						equal = true;
					}
				}
				if (equal == false) return true;
				equal = false;
			}
			return false;
		}
    }
};

struct tuple_priority_comparator
{
    // queue elements are vectors so we need to compare those
    bool operator()(Tuple* const& a, Tuple* const& b) const
    {
		//if (a.item != b.item || a.transaction_index != b.transaction_index) return true;
		//return false;
		return a->ratio < b->ratio;
    }
};

struct tuple_set_comparator
{
    // queue elements are vectors so we need to compare those
    bool operator()(Tuple* const& a, Tuple* const& b) const
    {
		if (a->item != b->item || a->transaction_index != b->transaction_index) return true;
		return false;
    }
};

#endif
