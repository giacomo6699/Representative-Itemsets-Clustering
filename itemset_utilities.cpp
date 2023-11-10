#include "itemset_utilities.h"
#include <set>
#include <iostream>
using namespace std;


void intersection_set(set<int> set1, set<int> set2, set<int> &intersectionset){
	for (int item1 : set1){
		for (int item2 : set2){
			if (item1 == item2){
				intersectionset.insert(item1);
			}
		}
	}
}

void union_set(set<int>::const_iterator iter1, set<int>::const_iterator end1, set<int>::const_iterator iter2, set<int>::const_iterator end2, set<int> &unionset){
	for (set<int>::iterator iter = iter1; iter != end1; iter++){
		unionset.insert(*iter);
	}
	for (set<int>::iterator iter = iter2; iter != end2; iter++){
		unionset.insert(*iter);
	}
}

Itemset::Itemset(set<int> starting_values){
	for (int item : starting_values){
		values.insert(item);
	}
}

Itemset::Itemset(){}

Tuple::Tuple(int index, int value){
	transaction_index = index;
	item = value;
	ratio = 0;
	state = -1;
}

Tuple::Tuple(int index, int value, double cost){
	transaction_index = index;
	item = value;
	ratio = cost;
	state = -1;
}

bool operator< (Itemset const& itemset, Itemset const& itemset2){
		if (itemset2.values.size() > itemset.values.size()) return true;
		else if (itemset2.values.size() < itemset.values.size()) return false;
		else return false;
		bool equal = false;
		for (int item1 : itemset.values){
			for (int item2 : itemset2.values){
				if (item1 == item2){
					equal = true;
				}
			}
			if (equal == false) return false;
			equal = false;
		}
		return false;
	}


bool operator== (Itemset const& itemset, Itemset const& itemset2){
		if (itemset.values.size() != itemset2.values.size()) return false;
		bool equal = false;
		for (int item1 : itemset.values){
			for (int item2 : itemset2.values){
				if (item1 == item2){
					equal = true;
				}
			}
			if (equal == false) return false;
			equal = false;
		}
		return true;
	}