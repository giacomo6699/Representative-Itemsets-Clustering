#ifndef UTILITIES
#define UTILITIES

#include <set>
#include <map>
#include <queue>
#include <string>
#include "itemset_utilities.h"
using namespace std;

struct instance {
    double timelimit;
    int randomseed;
    string filename;
    int trans_number;
    int max_trans_length;
    double single_item_prob;
    int clusters_number;
    double noise_prob;
    string cluster_file;
    int m;
    bool ground_truth;
};

void parse_command_line(int argc, char** argv, instance &inst);

void read_input(vector<set<Tuple*, tuple_set_comparator> > &database, instance &inst);

void free_memory(vector<set<Tuple*, tuple_set_comparator> > &database);

void tokenize(string const &str, const char delim, vector<string> &out);

void combinationUtil(int arr[], int n, int r, int index, int data[], int i, set<Itemset*, itemset_set_comparator> &possibleitemsets);

void addCombination(int arr[], int n, int r, set<Itemset*, itemset_set_comparator> &possibleitemsets);

void combinationUtil(int arr[], int n, int r, int index, int data[], int i, set<Itemset*, itemset_set_comparator> &possibleitemsets);

void addSubtransactions(set<Tuple*, tuple_set_comparator> transaction, int transaction_index, int n, int r, map<set<int>, pair<int, set<int> > > &subtransactions);

void subtransactionUtil(int arr[], int n, int r, int index, int data[], int i, map<set<int>, pair<int, set<int> > > &subtransactions, int transaction_index);

void addSubtransactions2(set<Tuple*, tuple_set_comparator> transaction, int transaction_index, int n, int r, map<int, set<set<int> > > &subtransactions);

void subtransactionUtil2(int arr[], int n, int r, int index, int data[], int i, map<int, set<set<int> > > &subtransactions, int transaction_index);

double compute_distance(set<int> itemset1, set<int> itemset2);

double hamming_distance(set<int> itemset1, set<int> itemset2);

double hamming_distance(vector<int> itemset1, vector<int> itemset2);

double jaccard_distance(set<int> itemset1, set<int> itemset2);

double jaccard_distance_bin(vector<int> itemset1, vector<int> itemset2);

int bin_to_int(vector<int> binary_trans, set<Tuple*, tuple_set_comparator> &int_trans, int trans_index);

void int_to_bin(set<Tuple*, tuple_set_comparator> int_trans, vector<int> &binary_trans, int max_syntrans_size);

void int_to_bin(set<int> int_trans, vector<int> &binary_trans, int max_syntrans_size);

double random01();

void print_error(string err);

void print_itemset(Itemset itemset);

void print_database(vector<set<Tuple*, tuple_set_comparator> > database);

void create_database(vector<set<Tuple*, tuple_set_comparator> > &database, instance &inst, set<set<int> > &ground_truth_clusters);

void add_noise(vector<set<Tuple*, tuple_set_comparator> > &database, instance &inst, double c1, double c3);

#endif
