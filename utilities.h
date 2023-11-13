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

double second();

/**
 * @brief computes the parsing of the command line arguments
 * 
 * @param argc 
 * @param argv 
 * @param inst 
 */
void parse_command_line(int argc, char** argv, instance &inst);

/**
 * @brief populates the database from the file given in input
 * 
 * @param database 
 * @param inst 
 */
void read_input(vector<set<Tuple*, tuple_set_comparator> > &database, instance &inst);

/**
 * @brief frees the memory
 * 
 * @param database 
 */
void free_memory(vector<set<Tuple*, tuple_set_comparator> > &database);

/**
 * @brief given a delimiter, splits the string into parts
 * 
 * @param str 
 * @param delim 
 * @param out 
 */
void tokenize(string const &str, const char delim, vector<string> &out);

/**
 * @brief adds the possible subtransactions of size r
 * 
 * @param transaction 
 * @param transaction_index 
 * @param n 
 * @param r 
 * @param subtransactions 
 */
void addSubtransactions(set<Tuple*, tuple_set_comparator> transaction, int transaction_index, int n, int r, map<set<int>, pair<int, set<int> > > &subtransactions);

void subtransactionUtil(int arr[], int n, int r, int index, int data[], int i, map<set<int>, pair<int, set<int> > > &subtransactions, int transaction_index);

/**
 * @brief generates a random number in [0, 1)
 * 
 * @return double 
 */
double random01();

/**
 * @brief prints a string and terminates the program execution
 * 
 * @param err 
 */
void print_error(string err);

/**
 * @brief prints itemset
 * 
 * @param itemset 
 */
void print_itemset(set<int> itemset);

/**
 * @brief prints the whole database
 * 
 * @param database 
 */
void print_database(vector<set<Tuple*, tuple_set_comparator> > database);

/**
 * @brief Creates a database
 * 
 * @param database 
 * @param inst 
 * @param ground_truth_clusters 
 */
void create_database(vector<set<Tuple*, tuple_set_comparator> > &database, instance &inst, set<set<int> > &ground_truth_clusters);

/**
 * @brief adds noise to the database
 * 
 * @param database 
 * @param inst 
 * @param c1 
 * @param c3 
 */
void add_noise(vector<set<Tuple*, tuple_set_comparator> > &database, instance &inst, double c1, double c3);

#endif
