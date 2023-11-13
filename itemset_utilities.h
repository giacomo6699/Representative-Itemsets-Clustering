#ifndef ITEMSETUTILITIES
#define ITEMSETUTILITIES

#include <set>
using namespace std;

/**
 * @brief computes the intersection between set1 and set2
 * 
 * @param set1 
 * @param set2 
 * @param intersectionset 
 */
void intersection_set(set<int> set1, set<int> set2, set<int> &intersectionset);

/**
 * @brief selects the right distance function to use
 * 
 * @param itemset1 
 * @param itemset2 
 * @return double 
 */
double compute_distance(set<int> itemset1, set<int> itemset2);

/**
 * @brief computes the hamming distance between two sets
 * 
 * @param itemset1 
 * @param itemset2 
 * @return double 
 */
double hamming_distance(set<int> itemset1, set<int> itemset2);

/**
 * @brief computes the hamming distances between two sets in binary form
 * 
 * @param itemset1 
 * @param itemset2 
 * @return double 
 */
double hamming_distance(vector<int> itemset1, vector<int> itemset2);

/**
 * @brief computes the jaccard distance between two sets
 * 
 * @param itemset1 
 * @param itemset2 
 * @return double 
 */
double jaccard_distance(set<int> itemset1, set<int> itemset2);

/**
 * @brief computes the jaccard distance between two sets in binary form
 * 
 * @param itemset1 
 * @param itemset2 
 * @return double 
 */
double jaccard_distance_bin(vector<int> itemset1, vector<int> itemset2);

/**
 * @brief computes the conversion from a set in binary form to a set of integers
 * 
 * @param binary_trans 
 * @param int_trans 
 * @param trans_index 
 * @return int 
 */
int bin_to_int(vector<int> binary_trans, set<Tuple*, tuple_set_comparator> &int_trans, int trans_index);

/**
 * @brief computes the conversion from a set of integers to a set in binary form
 * 
 * @param int_trans 
 * @param binary_trans 
 * @param max_syntrans_size 
 */
void int_to_bin(set<Tuple*, tuple_set_comparator> int_trans, vector<int> &binary_trans, int max_syntrans_size);

/**
 * @brief computes the conversion from a set of integers to a set in binary form
 * 
 * @param int_trans 
 * @param binary_trans 
 * @param max_syntrans_size 
 */
void int_to_bin(set<int> int_trans, vector<int> &binary_trans, int max_syntrans_size);

/**
 * @brief computes the union between two sets
 * 
 * @param iter1 
 * @param end1 
 * @param iter2 
 * @param end2 
 * @param unionset 
 */
void union_set(set<int>::const_iterator iter1, set<int>::const_iterator end1, set<int>::const_iterator iter2, set<int>::const_iterator end2, set<int> &unionset);

class Tuple {
	public:
	int transaction_index;
	int item;
	int state;
	double ratio;

	Tuple(int index, int value);

	Tuple(int index, int value, double cost);
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
