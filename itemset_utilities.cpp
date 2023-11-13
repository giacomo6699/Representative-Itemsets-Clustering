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

int bin_to_int(vector<int> binary_trans, set<Tuple*, tuple_set_comparator> &int_trans, int trans_index){
    for (int i = 0; i < binary_trans.size(); i++){
        if (binary_trans[i] == 1){
            Tuple* tuple = new Tuple(trans_index, i);
            int_trans.insert(tuple);
        }
    }
    return int_trans.size();
}

void int_to_bin(set<Tuple*, tuple_set_comparator> int_trans, vector<int> &binary_trans, int max_syntrans_size){
    for (int i = 0; i < max_syntrans_size; i++){
        binary_trans.push_back(0);
    }
    for (Tuple* tuple : int_trans){
        binary_trans[tuple->item] = 1;
    }
}

void int_to_bin(set<int> int_trans, vector<int> &binary_trans, int max_syntrans_size){
    for (int i = 0; i < max_syntrans_size; i++){
        binary_trans.push_back(0);
    }
    for (int item : int_trans){
        binary_trans[item] = 1;
    }
}

double compute_distance(set<int> itemset1, set<int> itemset2){
    return jaccard_distance(itemset1, itemset2);
}

double hamming_distance(set<int> itemset1, set<int> itemset2){
	// union - intersection
	set<int> intersect;
	intersection_set(itemset1, itemset2, intersect);
	double distance = itemset1.size() + itemset2.size() - 2 * intersect.size();
	return distance;
}

double hamming_distance(vector<int> itemset1, vector<int> itemset2){
	double distance = 0;
	for (int index = 0; index < itemset1.size(); index++){
		if (itemset1[index] != itemset2[index]){
			distance += 1.0;
		}
	}
	return distance;
}

double jaccard_distance(set<int> itemset1, set<int> itemset2){
	set<int> intersect;
	intersection_set(itemset1, itemset2, intersect);
	double similarity = (double) intersect.size() / (itemset1.size() + itemset2.size() - intersect.size());
	double distance = 1.0 - similarity;
	return distance;
}

double jaccard_distance_bin(vector<int> itemset1, vector<int> itemset2){
    double intersection_size = 0.0;
    double union_size = 0.0;
    for (int i = 0; i < itemset1.size(); i++){
        if (itemset1[i] == 1 && itemset2[i] == 1){
            intersection_size += 1.0;
            union_size += 1.0;
        } else if (itemset1[i] == 1 || itemset2[i] == 1){
            union_size += 1.0;
        }
    }
	double similarity = (double) intersection_size / union_size;
	double distance = 1.0 - similarity;
	return distance;
}