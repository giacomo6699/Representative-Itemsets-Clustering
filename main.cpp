#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <queue>
#include "utilities.h"
#include "itemset_utilities.h"
#include "apriori.h"

using namespace std;

#define c1 					2
#define c2					2
#define c3					1

/**
 * @brief computes the coverage-cost ratio of a tuple
 * 
 * @param database 
 * @param transaction_index index of the transaction that contains the tuple
 * @return double 
 */
double coveragecost_tuple(vector<set<Tuple*, tuple_set_comparator> > &database, int transaction_index){
	double ratio = (double) database[transaction_index].size() / c2;
	return ratio;
}

/**
 * @brief computes the coverage-cost ratio of the set_R with the representative candidate
 * 
 * @param database 
 * @param representative 
 * @param set_R 
 * @param subtransactions 
 * @return double 
 */
double coveragecost_R(vector<set<Tuple*, tuple_set_comparator> > &database, set<int> representative, set<set<int> > set_R, map<set<int>, pair<int, set<int> > > &subtransactions){
	double distance = 0;
	int numerator = 0;
	set<pair<int, int> > tuples;
	for (set<int> itemset : set_R){ // for every S in R
		set<int> transaction_indexes = subtransactions[itemset].second;
		for (int transaction_index : transaction_indexes){
			for (Tuple* tuple : database[transaction_index]){
				if (tuple->state == -1){
					if (itemset.count(tuple->item)){
						pair<int, int> t;
						t.first = tuple->item;
						t.second = transaction_index;
						if (tuples.count(t) == 0){
							numerator = numerator + 1;
							tuples.insert(t);
						}
					}
				}
			}
		}
		distance = distance + compute_distance(itemset, representative);
	}
	double denominator = c1 + c3 * distance;
	double result = (double) numerator/denominator;
	return result;
}

/**
 * @brief set the tuple as permanently uncovered
 * 
 * @param tuple 
 * @param affected_traj set that will contain the indexes of the trajectories affected by the last iteration of the algorithm
 */
void cover_tuple(Tuple* tuple, set<int> &affected_traj){
	tuple->state = 0;
	affected_traj.clear();
	affected_traj.insert(tuple->transaction_index);
}

/**
 * @brief set the tuples is set_R as covered
 * 
 * @param database 
 * @param set_R 
 * @param subtransactions 
 * @param affected_trans set that will contain the indexes of the trajectories affected by the last iteration of the algorithm
 */
void cover_set_R(vector<set<Tuple*, tuple_set_comparator> > &database, set<set<int> > set_R, map<set<int>, pair<int, set<int> > > &subtransactions, set<int> &affected_trans){
	affected_trans.clear();
	for (set<int> itemset : set_R){ // for every S in R
		set<int> transaction_indexes = subtransactions[itemset].second;
		for (int transaction_index : transaction_indexes){
			for (Tuple* tuple : database[transaction_index]){
				if (tuple->state == -1){
					if (itemset.count(tuple->item)){
						tuple->state = 1;
						affected_trans.insert(transaction_index);
					}
				}
			}                              
		}
		subtransactions.erase(itemset);
	}
}

/**
 * @brief computes the overlap cofficient of two sets of ints
 * 
 * @param I1 
 * @param I2 
 * @return double 
 */
double overlap_coefficient(set<int> I1, set<int> I2){
	double denominator = (I1.size() <= I2.size()) ? I1.size() : I2.size();
	set<int> intersect;
	intersection_set(I1, I2, intersect);
	double numerator = (double) intersect.size();
	return numerator/denominator;
}

/**
 * @brief checks if the coverage-cost ratio of the representative candidate has been affected by the last iteration of the algorithm
 * 
 * @param candidate_repr 
 * @param representative 
 * @param affected_trans 
 * @return true 
 * @return false 
 */
bool representative_cost_unchanged(map<set<int>, pair<int, set<int> > > &candidate_repr, set<int> representative, set<int> affected_trans){
	if (affected_trans.size() == 0) return false;
	set<int> candidate_trans = candidate_repr[representative].second;
	for (int item : candidate_trans){
		if (affected_trans.count(item)){
			return false;
		}
	}
	return true;
}

/**
 * @brief checks if the set_Td contains items involved in the last iteration of the algorithm
 * 
 * @param old_Td 
 * @param current_Td 
 * @return true 
 * @return false 
 */
bool is_involved(set<set<int> > old_Td, set<set<int> > current_Td){
	if (current_Td.empty()) return true;
	for (set<int> set_d : current_Td){
		for (set<int> old_d : old_Td){
			for (int item : set_d){
				if (old_d.count(item)){
					return true;
				}
			}
		}
	}
	return false;
}

/**
 * @brief computes the frequent itemsets
 * 
 * @param database 
 * @param freq_itemsets 
 * @param min_support 
 */
void apriori_method(vector<set<Tuple*, tuple_set_comparator> > &database, set<set<int> > &freq_itemsets, double min_support){
	vector<ksized_counter> all_freqk;
	cout << endl << "Starting APRIORI" << endl;
	build_freq1(database, all_freqk, min_support, freq_itemsets);
	ksized_counter previous_fam = all_freqk[0];
	ksized_counter candk;

	while (previous_fam.counter.size() != 0){
		generate_cand(candk, previous_fam);
		keep_freq(database, candk, all_freqk, min_support, freq_itemsets);
		previous_fam = all_freqk.back();
	}
	all_freqk.pop_back();
	int freq_itemsets_number = 0;
	for (int i = 0; i < all_freqk.size(); i++){
		freq_itemsets_number += all_freqk[i].counter.size();
	}
}

/**
 * @brief adds to freq_itemsets the union of the itemsets that have an overlap coefficient greater that min_overlap
 * 
 * @param freq_itemsets 
 * @param min_overlap 
 */
void add_overlaps(set<set<int> > &freq_itemsets, double min_overlap){
	set<set<int> > temp = freq_itemsets;
	for (set<set<int> >::iterator iter = temp.begin(); iter != temp.end(); iter++){
		for (set<set<int> >::iterator iter2 = iter; iter2 != temp.end(); iter2++){
			if (iter != iter2){
				double overlap = overlap_coefficient(*iter, *iter2);
				if (min_overlap < overlap && overlap < 1){
					set<int> unionset;
					union_set(iter->begin(), iter->end(), iter2->begin(), iter2->end(), unionset);
					freq_itemsets.insert(unionset);
				}
			}
		}
	}
}

/**
 * @brief computes the best set_Td for a given candidate
 * 
 * @param database 
 * @param representative 
 * @param Td 
 * @param subtransactions 
 * @param m 
 * @param affected_trans 
 * @param Td_trans_indexes 
 */
void build_Td(vector<set<Tuple*, tuple_set_comparator> > &database, set<int> representative, set<set<int> > &Td, map<set<int>, pair<int, set<int> > > &subtransactions, int m, set<int> affected_trans, set<int> &Td_trans_indexes){
	double function = 0;
	double gamma_min = 1.0 / (c1 + c3);
    double gamma = gamma_min;
	double learning_rate = 0;
	map<int, double> max_function_values;
	map<int, set<int> > max_subtrans;
	map<int, set<int> > previous_max_subtrans;
	map<int, double> previous_max_function_values;
	bool first_iter = true;
	double difference = 0;
	double starting_difference = 0;
	while (c1*gamma < function || first_iter) {
		if (!first_iter) {
			double rest = (double) (learning_rate - 1.0)/1.5; // /2.0
			learning_rate = 1.0 + rest;
			if (learning_rate < 2){
				learning_rate = 2;
			}
			gamma = gamma * learning_rate;
		}

		previous_max_subtrans.clear();
		previous_max_function_values.clear();
		previous_max_function_values = max_function_values;
		previous_max_subtrans = max_subtrans;

		max_function_values.clear();
		max_subtrans.clear();
		function = 0;

		bool already_comp = false;
		if (affected_trans.size() == 1){
			for (int aff : affected_trans){
				if (aff == -1){
					already_comp = true;
				}
			}
		}

		for (pair<set<int>, pair<int, set<int> > > element : subtransactions){
			bool affected = affected_trans.empty();
			for (int transaction_index : element.second.second){
				if (affected_trans.count(transaction_index)) affected = true;
			}

			int S_hat = 0;
			if (affected && !already_comp){
				for (int transaction_index : element.second.second){
					for (Tuple* tuple : database[transaction_index]){
						if (tuple->state == -1){
							if (element.first.count(tuple->item)){
								S_hat = S_hat + 1;
							}
						}
					}
				}
				subtransactions[element.first].first = S_hat;
			} else {
				S_hat = subtransactions[element.first].first;
			}
			double funct_value = S_hat - c3 * gamma * compute_distance(element.first, representative);
			for (int transaction_index : element.second.second){
				if (max_function_values[transaction_index] < funct_value){
					max_function_values[transaction_index] = funct_value;
					max_subtrans[transaction_index] = element.first;
				}
			}
		}
		for (pair<int, double> element : max_function_values){
			function = function + element.second;
		}
		if (first_iter){
			starting_difference = function - c1*gamma;
			if (starting_difference/5000 > 4){
				learning_rate = starting_difference/5000;
			} else if (starting_difference/1000 > 4){
				learning_rate = starting_difference/1000;
			} else if (starting_difference/500 > 4){
				learning_rate = starting_difference/500;
			} else if (starting_difference/100 > 4){
				learning_rate = starting_difference/100;
			} else if (starting_difference/50 > 4){
				learning_rate = starting_difference/50;
			} else {
				learning_rate = 2;
			}
		}
		first_iter = false;
	}
	for (pair<int, set<int> > element : previous_max_subtrans){
		Td.insert(element.second);
		if (previous_max_function_values[element.first] > 0){
			Td_trans_indexes.insert(element.first);
		}
	}
}

int main(int argc, char* argv[]){

	vector<set<Tuple*, tuple_set_comparator> > database;

	double start = second();

	instance inst;

	parse_command_line(argc, argv, inst);

	set<set<int> > ground_truth_clusters;

	if (inst.filename == "NULL"){
		create_database(database, inst, ground_truth_clusters);
		add_noise(database, inst, c1, c3);
		print_database(database);
	} else {
		read_input(database, inst);
	}

	set<set<int> > freq_itemsets;

	apriori_method(database, freq_itemsets, 0.004);

	add_overlaps(freq_itemsets, 1);

	priority_queue<Tuple*, vector<Tuple*>, tuple_priority_comparator> possibleTuples;

	map<set<int>, pair<int, set<int> > > subtransactions; // (subtransaction, set of transaction ids)

	for (int i = 0; i < database.size(); i++){
		int j = 1;
		for (Tuple* item : database[i]){
			addSubtransactions(database[i], i, database[i].size(), j, subtransactions);
			item->ratio = coveragecost_tuple(database, i);
			possibleTuples.push(item);
			j++;
		}
	}

	map<set<int>, pair<int, set<int> > > candidate_repr; // representative : (id, transaction indexes)
	map<int, pair<double, set<set<int> > > > candidate_repr_Td; // id : (cost, Tp)

	int id = 0;
	for (set<int> itemset : freq_itemsets){
		candidate_repr[itemset].first = id;
		candidate_repr_Td[id].first = -1;
		id++;
	}

	set<int> affected_trans;
	
	vector<set<int> > representatives_result;

	double distance_for_objfunct = 0.0;
	double total_cost = 0;
	set<set<int> > last_Td;
	while(possibleTuples.size() > 0 && (second() - start) < inst.timelimit){
		if (possibleTuples.top()->state != -1){
			possibleTuples.pop();
		} else {
			double max_cost = 0;
			set<set<int> > optimal_set_Td;
			set<int> optimal_representative;
			int analyzed_repr = 0;
			bool firstiter_of_batch = true;
			set<int> affected_trans_forTd = affected_trans;
			for (set<int> itemset : freq_itemsets){ // for each representative
				if (itemset.size() < 2) continue;
				analyzed_repr++;
				double batch_start = second();
				set<set<int> > set_Td;
				if (!firstiter_of_batch){
					affected_trans_forTd.clear();
					affected_trans_forTd.insert(-1);
				}
				double cost = 0;
				if (representative_cost_unchanged(candidate_repr, itemset, affected_trans)){
					int id = candidate_repr[itemset].first;
					cost = candidate_repr_Td[id].first;
					set_Td = candidate_repr_Td[id].second;
				} else {
					int id = candidate_repr[itemset].first;
					set<set<int> > current_Td = candidate_repr_Td[id].second;

					if (is_involved(last_Td, current_Td)){
						set<int> Td_trans_indexes;
						build_Td(database, itemset, set_Td, subtransactions, inst.m, affected_trans_forTd, Td_trans_indexes); // create the correspondent Tp
						cost = coveragecost_R(database, itemset, set_Td, subtransactions); 
						int id = candidate_repr[itemset].first;
						candidate_repr_Td[id].first = cost;
						candidate_repr_Td[id].second = set_Td;
						candidate_repr[itemset].second = Td_trans_indexes;
					} else {
						int id = candidate_repr[itemset].first;
						cost = candidate_repr_Td[id].first;
						set_Td = candidate_repr_Td[id].second;
					}
				}
				if (cost > max_cost){ // find the max value and store the correspondent terms
					max_cost = cost;
					optimal_representative = itemset;
					optimal_set_Td = set_Td;
				}
				firstiter_of_batch = false;
			}

			if (max_cost >= possibleTuples.top()->ratio){
				total_cost += max_cost;
					
				for (set<int> subt : optimal_set_Td){
					distance_for_objfunct += jaccard_distance(optimal_representative, subt);
				}

				cover_set_R(database, optimal_set_Td, subtransactions, affected_trans); // set as covered the correspondent items
				// add optimal_representative to the final result set
				representatives_result.push_back(optimal_representative);
				last_Td.clear();
				last_Td = optimal_set_Td;
			} else {
				last_Td.clear();
				set<int> set_item;
				set_item.insert(possibleTuples.top()->item);
				last_Td.insert(set_item);
				total_cost += possibleTuples.top()->ratio;
				Tuple* chosen_tuple = possibleTuples.top();
				cover_tuple(chosen_tuple, affected_trans); // set as uncovered the best tuple
				possibleTuples.pop();
			}
		}
	}

	free_memory(database);

    return 0;
}
