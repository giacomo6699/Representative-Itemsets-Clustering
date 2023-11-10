#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <queue>
#include "chrono.h"
#include "utilities.h"
#include "itemset_utilities.h"
#include "apriori.h"

using namespace std;

#define c1 					2
#define c2					2
#define c3					1
#define EPS					0.1

double second();

double coveragecost_tuple(vector<set<Tuple*, tuple_set_comparator> > &database, int transaction_index){
	double ratio = (double) database[transaction_index].size() / c2;
	return ratio;
}

bool is_subset(Itemset* itemset, set<Tuple*, tuple_set_comparator> transaction, int &covered_count, int &uncovered_count){
	for (int item : itemset->values){
		bool found = false;
		for (Tuple* tuple : transaction){
			if (item == tuple->item){
				found = true;
				if (tuple->state == -1){
					uncovered_count++;
				} else {
					covered_count++;
				}
			}
		}
		if (!found) return false;
	}
	return true;
}

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
		//numerator = numerator + subtransactions[itemset].first;
		distance = distance + compute_distance(itemset, representative);
	}
	double denominator = c1 + c3 * distance;
	double result = (double) numerator/denominator;
	return result;
}

void cover_tuple(Tuple* tuple, set<int> &affected_traj){
	tuple->state = 0;
	affected_traj.clear();
	affected_traj.insert(tuple->transaction_index);
}

void cover_set_R(vector<set<Tuple*, tuple_set_comparator> > &database, set<set<int> > set_R, map<set<int>, pair<int, set<int> > > &subtransactions, set<int> &affected_traj){
	affected_traj.clear();
	for (set<int> itemset : set_R){ // for every S in R
		set<int> transaction_indexes = subtransactions[itemset].second;
		for (int transaction_index : transaction_indexes){
			for (Tuple* tuple : database[transaction_index]){
				if (tuple->state == -1){
					if (itemset.count(tuple->item)){
						tuple->state = 1;
						affected_traj.insert(transaction_index);
					}
				}
			}                              
		}
		subtransactions.erase(itemset);
	}
}

double overlap_coefficient(set<int> I1, set<int> I2){
	double denominator = (I1.size() <= I2.size()) ? I1.size() : I2.size();
	set<int> intersect;
	intersection_set(I1, I2, intersect);
	double numerator = (double) intersect.size();
	return numerator/denominator;
}

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

bool is_involved(set<set<int> > old_tp, set<set<int> > current_tp){
	// ONLY FOR TESTING THE UTILITY OF THIS METHOD
	//return true;
	//
	
	if (current_tp.empty()) return true;
	for (set<int> setp : current_tp){
		for (set<int> oldp : old_tp){
			for (int item : setp){
				if (oldp.count(item)){
					return true;
				}
			}
		}
	}
	return false;
}


void build_Tp(vector<set<Tuple*, tuple_set_comparator> > &database, set<int> representative, set<set<int> > &Tp, map<set<int>, pair<int, set<int> > > &subtransactions, int m, set<int> affected_trans, set<int> &Tp_trans_indexes){
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

			/*vector<int> bin_element;
			vector<int> bin_repr;
			int_to_bin(element.first, bin_element, 25);
			int_to_bin(representative, bin_repr, 25);

			double funct_value = S_hat - c3 * gamma * jaccard_distance_bin(bin_element, bin_repr);*/
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
		//cout << "Learning rate: " << learning_rate << endl;
		//cout << "Current difference from c1y (" << c1*gamma << ") and function (" << function << ") with y = " << gamma << " is: " << c1*gamma - function << endl;
		//difference = function - c1*gamma;
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
	//cout << "    Difference from c1y and function is: " << c1*gamma - function << endl;
	//set<int> Tp2;
	for (pair<int, set<int> > element : previous_max_subtrans){
		Tp.insert(element.second);
		if (previous_max_function_values[element.first] > 0){
			Tp_trans_indexes.insert(element.first);
		}

		/*set<int> trans_indexes = subtransactions[element.second].second;
		for (int index : trans_indexes){
			Tp2.insert(index);
		}*/
	}
	/*if (Tp2 != Tp_trans_indexes){
		cout << endl << "DIVERSIIIIIII" << endl;
	}*/
	/*cout << "    set Tp is composed by: ";
	for (set<int> setp : Tp){
		print_itemset(Itemset(setp));
		cout << " - ";
	}
	cout << endl;*/
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
		// dataset in input
		read_input(database, inst);
	}

	set<set<int> > freq_itemsets;

	if (inst.ground_truth){
         	freq_itemsets = ground_truth_clusters;       
        } else {

	// APRIORI
	vector<ksized_counter> all_freqk;
	double min_support = 0.004;
	cout << endl << "Starting APRIORI" << endl;
	build_freq1(database, all_freqk, min_support, freq_itemsets);
	//cout << "Freq1 built!" << endl;
	ksized_counter previous_fam = all_freqk[0];
	ksized_counter candk;

	while (previous_fam.counter.size() != 0){
		//cout << "Iteration of the while " << previous_fam.k + 1 << endl;
		generate_cand(candk, previous_fam);
		//cout << "cand generated" << endl;
		keep_freq(database, candk, all_freqk, min_support, freq_itemsets);
		//cout << "freq keep" << endl;
		previous_fam = all_freqk.back();
	}
	all_freqk.pop_back();
	int freq_itemsets_number = 0;
	for (int i = 0; i < all_freqk.size(); i++){
		freq_itemsets_number += all_freqk[i].counter.size();
	}
	// APRIORI


	/*set<set<int> > freq_itemsets;

	set<int> set1;
	set1.insert(3);
	set1.insert(4);
	freq_itemsets.insert(set1);

	set<int> set2;
	set2.insert(5);
	set2.insert(4);
	set2.insert(6);
	freq_itemsets.insert(set2);
	
	set<int> set3;
	set3.insert(7);
	set3.insert(4);
	set3.insert(2);
	freq_itemsets.insert(set3);
	
	set<int> set4;
	set4.insert(3);
	set4.insert(2);
	freq_itemsets.insert(set4);

	set<int> set5;
	set5.insert(1);
	freq_itemsets.insert(set5);

	set<int> set6;
	set6.insert(3);
	freq_itemsets.insert(set6);

	set<int> set7;
	set7.insert(8);
	set7.insert(2);
	set7.insert(1);
	freq_itemsets.insert(set7);*/

	double min_overlap = 1;

	set<set<int> > temp = freq_itemsets;

	//cout << "freq_itemsets size: " << freq_itemsets.size() << endl;
	//cout << "temp size: " << temp.size() << endl;

	for (set<set<int> >::iterator iter = temp.begin(); iter != temp.end(); iter++){
		for (set<set<int> >::iterator iter2 = iter; iter2 != temp.end(); iter2++){
			if (iter != iter2){
				double overlap = overlap_coefficient(*iter, *iter2);
				if (min_overlap < overlap && overlap < 1){
					// add to freq (to a temporary one otherwise we would have some problems)
					set<int> unionset;
					union_set(iter->begin(), iter->end(), iter2->begin(), iter2->end(), unionset);
					freq_itemsets.insert(unionset);
				}
			}
		}
	}

	// comment this if you uncommented the line inside "build_freq1()"
	for (set<Tuple*, tuple_set_comparator> trans : database){
		if (trans.size() == 1){
			set<int> singleton;
			for (Tuple* tuple : trans){
				singleton.insert(tuple->item);
				freq_itemsets.insert(singleton);
			}
		}
	}
	//
	
	cout << "freq_itemsets size: " << freq_itemsets.size() << endl;
	cout << "temp size: " << temp.size() << endl;

	}

	double intermediate = second();
	cout << endl << (intermediate - start) << " seconds to create the database of size " << database.size() << endl << endl;

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
	
	cout << "Number of Subtransactions: " << subtransactions.size() << endl;

	map<set<int>, pair<int, set<int> > > candidate_repr; // representative : (id, transaction indexes)
	map<int, pair<double, set<set<int> > > > candidate_repr_tp; // id : (cost, Tp)

	int id = 0;
	for (set<int> itemset : freq_itemsets){
		candidate_repr[itemset].first = id;
		candidate_repr_tp[id].first = -1;
		id++;
	}

	cout << "Candidate Representatives size: " << candidate_repr.size() << endl;
	
	/*map<int, set<set<int> > > subtransactions2; // (transaction_id, set of subtransactions)

	for (int i = 0; i < database.size(); i++){
		int j = 0;
		for (Tuple* item : database[i]){
			addSubtransactions2(database[i], i, database[i].size(), j, subtransactions2);
			item->ratio = coveragecost_tuple(database, i);
			possibleTuples.push(item);
			j++;
		}
	}*/

	double end = second();

	cout << endl << (end - intermediate) << " seconds to create the subtransactions" << endl;

	set<int> affected_traj;
	
	vector<set<int> > representatives_result;
	int w = 0;

	double distance_for_objfunct = 0.0;
	double total_cost = 0;
	set<set<int> > last_tp;
	while(possibleTuples.size() > 0 && (second() - start) < inst.timelimit){
		if (possibleTuples.top()->state != -1){
			possibleTuples.pop();
		} else {
			w++;
			double max_cost = 0;
			set<set<int> > optimal_set_Tp;
			set<int> optimal_representative;
			int analyzed_repr = 0;
			bool firstiter_of_batch = true;
			set<int> affected_trans_forTp = affected_traj;
			for (set<int> itemset : freq_itemsets){ // for each representative
				if (itemset.size() < 2) continue;
				analyzed_repr++;
				double batch_start = second();
				/*cout << endl << endl << "Iteration " << w << " analyzing the representative number " << analyzed_repr << " ";
				print_itemset(Itemset(itemset));
				cout << endl << endl;*/
				set<set<int> > set_Tp;
				if (!firstiter_of_batch){
					affected_trans_forTp.clear();
					affected_trans_forTp.insert(-1);
				}
				double cost = 0;
				if (representative_cost_unchanged(candidate_repr, itemset, affected_traj)){
					/*cout << "cost unchanged of ";
					print_itemset(Itemset(itemset));
					cout << " since ";
					print_itemset(Itemset(candidate_repr[itemset].second));
					cout << endl;*/
					int id = candidate_repr[itemset].first;
					cost = candidate_repr_tp[id].first;
					set_Tp = candidate_repr_tp[id].second;
				} else {
					int id = candidate_repr[itemset].first;
					set<set<int> > current_tp = candidate_repr_tp[id].second;

					if (is_involved(last_tp, current_tp)){
						/*print_itemset(Itemset(itemset));
						cout << " involved since indexes: ";
						print_itemset(Itemset(candidate_repr[itemset].second));
						cout << endl;*/
						set<int> Tp_trans_indexes;
						build_Tp(database, itemset, set_Tp, subtransactions, inst.m, affected_trans_forTp, Tp_trans_indexes); // create the correspondent Tp
						cost = coveragecost_R(database, itemset, set_Tp, subtransactions); 
						int id = candidate_repr[itemset].first;
						candidate_repr_tp[id].first = cost;
						candidate_repr_tp[id].second = set_Tp;
						candidate_repr[itemset].second = Tp_trans_indexes;

					} else {
						/*print_itemset(Itemset(itemset));
						cout << "not involved since ";
						print_itemset(Itemset(candidate_repr[itemset].second));
						cout << endl;*/
						int id = candidate_repr[itemset].first;
						cost = candidate_repr_tp[id].first;
						set_Tp = candidate_repr_tp[id].second;
					}
				}

				if (cost > max_cost){ // find the max value and store the correspondent terms
					max_cost = cost;
					optimal_representative = itemset;
					optimal_set_Tp = set_Tp;
				}
				firstiter_of_batch = false;
				//cout << "The processing of the iteration " << analyzed_repr << " took: " << second() - batch_start << " seconds" << endl;
			}
			if (max_cost != 0){ // if at least a value is found. TODO: add else statement and also the checks for a empty priority_queue
				if (max_cost >= possibleTuples.top()->ratio){
					total_cost += max_cost;
					
					for (set<int> subt : optimal_set_Tp){
						distance_for_objfunct += jaccard_distance(optimal_representative, subt);
					}

					cover_set_R(database, optimal_set_Tp, subtransactions, affected_traj); // set as covered the correspondent items
					cout << endl << "cover_set_R of ratio = " << max_cost << " with representative: ";
					print_itemset(Itemset(optimal_representative));
					cout << " covering " << optimal_set_Tp.size() << " subtransactions" << endl;
					cout << "these subtransactions are:" << endl;
					for (set<int> R : optimal_set_Tp){
						print_itemset(Itemset(R));
						cout << endl;
					}
					cout << "of indexes ";
					print_itemset(Itemset(candidate_repr[optimal_representative].second));
					cout << endl;
					cout << "affected transactions: ";
					print_itemset(Itemset(affected_traj));
					cout << endl;
					// add optimal_representative to the final result set
					representatives_result.push_back(optimal_representative);
					last_tp.clear();
					last_tp = optimal_set_Tp;
					// update costs if you don't want to compute all of them in every iteration from scratch
				} else {
					last_tp.clear();
					set<int> set_item;
					set_item.insert(possibleTuples.top()->item);
					last_tp.insert(set_item);
					total_cost += possibleTuples.top()->ratio;
					Tuple* chosen_tuple = possibleTuples.top();
					cout << "max_cost != 0 and chosen tuple: item = " << chosen_tuple->item << "  trans_index = " << chosen_tuple->transaction_index << "  state = " << chosen_tuple->state << "  ratio = " << chosen_tuple->ratio << endl;
					cover_tuple(chosen_tuple, affected_traj); // set as uncovered the best tuple
					possibleTuples.pop();
					// update costs if you don't want to compute all of them in every iteration from scratch
				}
			} else {
				last_tp.clear();
				set<int> set_item;
				set_item.insert(possibleTuples.top()->item);
				last_tp.insert(set_item);
				total_cost += possibleTuples.top()->ratio;
				Tuple* chosen_tuple = possibleTuples.top();
				cout << "max_cost == 0 and chosen tuple: item = " << chosen_tuple->item << "  trans_index = " << chosen_tuple->transaction_index << "  state = " << chosen_tuple->state << "  ratio = " << chosen_tuple->ratio << endl;
				cover_tuple(chosen_tuple, affected_traj); // set as uncovered the best tuple
				possibleTuples.pop();
			}
		}
	}
	
	cout << (second() - end) << " seconds to cover all" << endl;

	cout << endl << "Total cost of the solution is: " << total_cost << endl;

	/*vector<int> transactions;
	vector<int> categories;
	map<int, int> map1;
	

	for (int cat : categories){
		map1[cat] = 0;
		for (int trans : transactions){
			if (trans.category() == cat){
				double startms = start.getTime();
				double endms = end.getTime();
				String time;
				String[] arraygiorno = time.split('T')[0].split('-');
				String[] arrayora = time.split('T')[1].split(':');
				if (trans.time().isright()){
					int value = map1[cat] + trans.amount();
					map1[cat] = value;
				}
			}
		}
	}*/
	
	cout << "W: " << w << endl;

	cout << "possibleTuples size: " << possibleTuples.size() << endl;

	cout << endl << "representatives_result size: " << representatives_result.size() << endl << endl;
	cout << "representatives list: " << endl;
	for (int i = 0; i < representatives_result.size(); i++){
		print_itemset(Itemset(representatives_result[i]));
		/*cout << " cover subtrans of trans indexes ";
		set<int> trans_indexes = candidate_repr[representatives_result[i]].second;
		print_itemset(Itemset(trans_indexes));*/
		cout << endl;
	}
	cout << endl;

	cout << "Transactions size: " << database.size() << endl << endl;

	double second_term = 0.0;
	for (int i = 0; i < database.size(); i++){
		double count = 0.0;
		for (Tuple* tuple : database[i]){
			if (tuple->state == 0){
				count += 1.0;
			}
		}
		second_term += count / (double) database[i].size();
	}

	vector<set<int> > final_repr_result = representatives_result;

	for (int i = 0; i < representatives_result.size(); i++){
		for (int j = i+1; j < representatives_result.size(); j++){
			if (representatives_result[i] == representatives_result[j]){
				final_repr_result.erase(final_repr_result.begin() + i);
			}
		}
	}

	cout << "Final representatives size: " << final_repr_result.size() << endl << endl;

	double final_obj_funct = c1 * final_repr_result.size() + c2 * second_term + c3 * distance_for_objfunct;

	cout << "Final objective function value: " << final_obj_funct << endl << endl;

	free_memory(database);

    return 0;
}
