#include "utilities.h"
#include "itemset_utilities.h"
#include <string>
#include <set>
#include <map>
#include <queue>
#include <iostream>
#include <sstream>
#include <fstream>

#include <time.h>
#include <time.h>
#include <time.h>
#include <stdlib.h>

#ifdef _WIN32

double second()
{
	return ((double)clock()/(double)CLK_TCK);
}

#else 

#include <sys/resource.h>

#if defined(__MACH__) && defined(__APPLE__)
#include <mach/mach.h>
#include <mach/mach_time.h>
#endif
	
double myWallTime()
{
#ifdef __APPLE__
	static double timeConvert = 0.0;
	if ( timeConvert == 0.0 )
	{
		mach_timebase_info_data_t timeBase;
		mach_timebase_info(&timeBase);
		timeConvert = (double)timeBase.numer / (double)timeBase.denom / 1000000000.0;
	}
	return mach_absolute_time() * timeConvert;
#else
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (double)ts.tv_sec + 1.0e-9*((double)ts.tv_nsec);
#endif // __APPLE__
}

double second()
{
	double t = myWallTime();
	return(t);
}

#endif

using namespace std;

void parse_command_line(int argc, char** argv, instance &inst) 
{ 
	// default   
	inst.filename = "NULL";
	inst.randomseed = 0; 
	inst.timelimit = 36000;
	inst.trans_number = 500;
	inst.max_trans_length = 15;
	inst.clusters_number = 10;
	inst.single_item_prob = 0.5;
	inst.noise_prob = 0;
	inst.ground_truth = false;
    inst.cluster_file = "NULL";
    int help = 0; if ( argc < 1 ) help = 1;	
	for ( int i = 1; i < argc; i++ ) 
	{ 
        string argvi = argv[i];
		if (argvi == "-file") { inst.filename = argv[++i]; continue; } 			
		if (argvi == "-input") { inst.filename = argv[++i]; continue; } 			
		if (argvi == "-f") { inst.filename = argv[++i]; continue; } 				
		if (argvi == "-tl") { inst.timelimit = atof(argv[++i]); continue; }				
		if (argvi == "-n") { inst.trans_number = atoi(argv[++i]); continue; } 					
		if (argvi == "-noise") { inst.noise_prob = atof(argv[++i]); continue; } 		
		if (argvi == "-itemprob") { inst.single_item_prob = atof(argv[++i]); continue; } 		
		if (argvi == "-seed") { inst.randomseed = abs(atoi(argv[++i])); continue; } 		
		if (argvi == "-cln") { inst.clusters_number = abs(atoi(argv[++i])); continue; }
		if (argvi == "-clf") { inst.cluster_file = argv[++i]; continue; }
		if (argvi == "-maxtlength") { inst.max_trans_length = atoi(argv[++i]); continue; }
		if (argvi == "-gtruth") { inst.ground_truth = true; continue; }
		if (argvi == "-help") { help = 1; continue; } 									
		if (argvi == "--help") { help = 1; continue; } 								
		help = 1;
    }   

	if ( help)		// print current parameters
	{
        cout << endl << endl << "Available parameters --------------------------------------------------------------------" << endl;
        cout << "-file " << inst.filename << endl;
        cout << "-tl " << inst.timelimit << endl;
        cout << "-seed " << inst.randomseed << endl;
        cout << "-n " << inst.trans_number << endl;
        cout << "-noise " << inst.noise_prob << endl;
        cout << "-itemprob " << inst.single_item_prob << endl;
        cout << "-cln " << inst.clusters_number << endl;
        cout << "-clf " << inst.cluster_file << endl;
        cout << "-maxtlength " << inst.max_trans_length << endl;
        cout << endl << "Enter -help or --help for help" << endl << "----------------------------------------------------------------------------------------------" << endl << endl;
	}        
	
	//if ( help ) exit(1);

}   

void read_input(vector<set<Tuple*, tuple_set_comparator> > &database, instance &inst){
    int m = 0;
	//ifstream infile(argv[1]);
	ifstream infile(inst.filename);
    //ofstream outfile("chesssmallshort.dat");

	string line;
	while (getline(infile, line)) // 3196 transactions
	{
		istringstream iss(line);
		int a, b;
		if (!(iss >> a >> b)) { break; } // error
		const char delim = ' '; 
    	vector<string> out; 
    	tokenize(line, delim, out); 
		set<Tuple*, tuple_set_comparator> transaction;
		for (string &s: out) { 
            /*if (transaction.size() < 15){
                outfile << s << " ";
            } else if (transaction.size() == 15){
                outfile << endl;
            }*/
			Tuple *item = new Tuple(database.size(), stoi(s));
			transaction.insert(item);
            m++;
		}
		database.push_back(transaction);
	}
    inst.m = m;
    //outfile.close();
}

void free_memory(vector<set<Tuple*, tuple_set_comparator> > &database){
	for (set<Tuple*, tuple_set_comparator> transaction : database){
		for (Tuple* tuple : transaction){
			delete tuple;
		}
	}
}

void tokenize(string const &str, const char delim, vector<string> &out) 
{ 
    // construct a stream from the string 
    stringstream ss(str); 
 
    string s; 
    while (getline(ss, s, delim)) { 
        out.push_back(s); 
    } 
}

void addSubtransactions(set<Tuple*, tuple_set_comparator> transaction, int transaction_index, int n, int r, map<set<int>, pair<int, set<int> > > &subtransactions)
{
    // A temporary array to store
    // all combination one by one
    int data[r];
	int arr[transaction.size()];
	int i = 0;
	for (Tuple* item : transaction){
		arr[i] = item->item;
		i++;
	}
 
    // Print all combination using
    // temporary array 'data[]'
    subtransactionUtil(arr, n, r, 0, data, 0, subtransactions, transaction_index);
}

void subtransactionUtil(int arr[], int n, int r, int index, int data[], int i, map<set<int>, pair<int, set<int> > > &subtransactions, int transaction_index)
{
    // Current combination is ready, print it
    if (index == r)
    {
		set<int> itemset;
        for (int j = 0; j < r; j++){
			//cout << data[j];
			itemset.insert(data[j]);
		}
        subtransactions[itemset].first = -1;
        subtransactions[itemset].second.insert(transaction_index);
        return;
    }

    // When no more elements are there to put in data[]
    if (i >= n)
        return;
 
    // current is included, put next at next location
    data[index] = arr[i];
    subtransactionUtil(arr, n, r, index + 1, data, i + 1, subtransactions, transaction_index);
 
    // current is excluded, replace it with next (Note that
    // i+1 is passed, but index is not changed)
    subtransactionUtil(arr, n, r, index, data, i + 1, subtransactions, transaction_index);
}

void print_error(string err) {
    cout << "ERROR: " << err << endl; fflush(NULL); exit(1);
}  

void print_itemset(set<int> itemset){
	cout << "[";
	int i = 0;
	for (int item : itemset){
		if (i == itemset.size() - 1)
			cout << item;
		else 
			cout << item << ", ";
		i++;
	}
	cout << "]";
}

void print_database(vector<set<Tuple*, tuple_set_comparator> > database){
    for (int i = 0; i < database.size(); i++){
        cout << "[";
	    int j = 0;
	    for (Tuple* tuple : database[i]){
	    	if (j == database[i].size() - 1)
			    cout << tuple->item;
		    else 
			    cout << tuple->item << ", ";
		    j++;
	    }
	    cout << "]" << endl;
    }
}

double random01() { 
	return ((double) rand() / RAND_MAX); 
}

void create_database(vector<set<Tuple*, tuple_set_comparator> > &database, instance &inst, set<set<int> > &ground_truth_clusters){
    int m = 0;
    srand(82356+abs(inst.randomseed));

	for (int k = 0; k < 1000; k++) rand();
    // create representatives
    vector<vector<int> > representatives_set;
    if (inst.cluster_file == "NULL"){
        fflush(NULL);
	double start = second();
	double min_distance = 0.8;
	if (inst.max_trans_length < 15) min_distance = 0.7;
        while (representatives_set.size() < inst.clusters_number){
            vector<int> binary_repr;
            for (int i = 0; i < inst.max_trans_length; i++){
                if (random01() < inst.single_item_prob){
                    binary_repr.push_back(1);
                } else {
                    binary_repr.push_back(0);
                }
            }
            bool present = false;
            for (int i = 0; i < representatives_set.size(); i++){
                if (representatives_set[i] == binary_repr){
                    present = true;
                    break;
                }
            }
            bool greater_than_one = false;
            int g = 0;
            for (int item : binary_repr){
                if (item == 1){
                    g++;
                    if (g > 1) {
                        greater_than_one = true;
                        break;
                    }
                }
            }
            if (!present && greater_than_one) {
                bool too_close = false;
                for (vector<int> repr : representatives_set){
                    if (jaccard_distance_bin(repr, binary_repr) < min_distance){
                        too_close = true;
                    }
                }
                if (!too_close) representatives_set.push_back(binary_repr);
            }
	    if (second() - start > 60){
		min_distance = 0.7;
		if (inst.max_trans_length < 15) min_distance = 0.6;
	    }
	    if (second() - start > 120){
		print_error("Time limit for creating clusters expired");
		break;
	    }
        }
	cout << endl << "min_distance between clusters: " << min_distance << endl;
    } else {
        // read clusters from input file
        ifstream infile(inst.cluster_file);
        string line;
	    while (getline(infile, line)) // 3196 transactions
	    {
		    istringstream iss(line);
            int a, b;
            if (!(iss >> a >> b)) { break; } // error
            const char delim = ' '; 
            vector<string> out; 
            tokenize(line, delim, out); 
            vector<int> transaction;
            for (int i = 0; i < inst.max_trans_length; i++){
                transaction.push_back(0);
            }
            for (string &s: out) { 
                /*if (transaction.size() < 15){
                    outfile << s << " ";
                } else if (transaction.size() == 15){
                    outfile << endl;
                }*/
                transaction[stoi(s)] = 1;
                m++;
            }
            representatives_set.push_back(transaction);
	    }
        inst.clusters_number = representatives_set.size();
    }

    /*string file = "results/instance_n" + to_string(inst.trans_number) + "_c" + to_string(inst.clusters_number)
    + "_max" + to_string(inst.max_trans_length) + " _itprob" + to_string(inst.single_item_prob)
    + "_noise" + to_string(inst.noise_prob) + "_seed" + to_string(inst.randomseed) + "_tl" + to_string(inst.timelimit);
    ofstream outfile(file);*/

    // create actual database
    vector<int> repr_counts;
    for (int i = 0; i < inst.clusters_number; i++){
        repr_counts.push_back(0);
    }
    vector<set<int> > int_repr_set;
    for (int i = 0; i < inst.trans_number; i++){
        int repr_index = random01() * inst.clusters_number;

        // da eliminare per avere un numero randomico di elementi per ogni cluster
        //repr_index = i / 10;
        //

        set<Tuple*, tuple_set_comparator> trans;
        bin_to_int(representatives_set[repr_index], trans, i);
        m += trans.size();
        database.push_back(trans);
        int count = repr_counts[repr_index] + 1;
        repr_counts[repr_index] = count;
    }
    for (int index = 0; index < representatives_set.size(); index++){
        cout << endl;
        set<Tuple*, tuple_set_comparator> trans;
        bin_to_int(representatives_set[index], trans, 1);
        set<int> itemset;
        cout << "[";
        for (Tuple* tuple : trans){
            itemset.insert(tuple->item);
            cout << tuple->item << " ";
        }
	ground_truth_clusters.insert(itemset);
        cout << "] : " << repr_counts[index] << endl;
        //print_itemset(Itemset(itemset));
        //cout << " : " << repr_counts[index] << endl;

    }
    cout << endl;
    //outfile.close();
    inst.m = m;
}

void add_noise(vector<set<Tuple*, tuple_set_comparator> > &database, instance &inst, double c1, double c3){
    int m = 0;
    /*string file = "datasets/dataset_n" + to_string(inst.trans_number) + "_c" + to_string(inst.clusters_number)
    + "_max" + to_string(inst.max_trans_length) + " _itprob" + to_string(inst.single_item_prob)
    + "_noise" + to_string(inst.noise_prob) + "_seed" + to_string(inst.randomseed) + "_tl" + to_string(inst.timelimit);
    ofstream outfile(file, ios::app);*/
    double trans_length_sum = 0;
    int changes = 0;
    double dist_for_objfunct = 0.0;
    for (int i = 0; i < database.size(); i++){
        vector<int> bin_trans;
        int_to_bin(database[i], bin_trans, inst.max_trans_length);
        //cout << "int_to_bin done" << endl;
	vector<int> starting_cluster = bin_trans;
        for (int index = 0; index < bin_trans.size(); index++){
            if (random01() < inst.noise_prob){
                changes++;
                if (bin_trans[index] == 1)
                    bin_trans[index] = 0;
                else 
                    bin_trans[index] = 1;
            }
        }

        //cout << "noise added" << endl;
        for (Tuple* tuple : database[i]){
            delete tuple;
        }
        //cout << "trans deleted" << endl;
	dist_for_objfunct += jaccard_distance_bin(starting_cluster, bin_trans);
	//
        set<Tuple*, tuple_set_comparator> noisy;
        m += bin_to_int(bin_trans, noisy, i);
        database[i] = noisy;
        trans_length_sum = trans_length_sum + (double) noisy.size();
        /*for (Tuple* tuple : noisy){
            outfile << tuple->item << " ";
        }
        outfile << endl;*/
    }
    double avg_length = trans_length_sum / (double) database.size();
    cout << "Transactions Average Length: " << avg_length << endl << endl;
    double ground_truth_objfunct = c1 * ((double) inst.clusters_number) + c3 * dist_for_objfunct;
    cout << "Ground Truth Objective Function: " << ground_truth_objfunct << endl << endl;
    cout << "Number of changes caused by Noise: " << changes << endl << endl;
    //outfile.close();
    inst.m = m;
}
