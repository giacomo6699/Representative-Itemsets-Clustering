#include <vector>
#include <set>
#include "itemset_utilities.h"
#include "apriori.h"
#include <iostream>

using namespace std;

int build_freq1(vector<set<Tuple*, tuple_set_comparator> > &database, vector<ksized_counter> &all_freqk, double min_support, set<set<int> > &freq_itemsets){
    ksized_counter cand1(1);
    int items_number = 0;
    for (int i = 0; i < database.size(); i++){
        for (Tuple* tuple : database[i]){
            set<int> itemset;
            itemset.insert(tuple->item);
            if (cand1.counter.count(itemset) > 0){
                cand1.counter[itemset] += 1;
            } else {
                cand1.counter[itemset] = 1;
                items_number++;
            }
        }
    }

    double min_support_count = database.size() * min_support;

    ksized_counter freq1(1);

    for (pair<set<int>, int> element : cand1.counter){
        if (element.second >= min_support_count){
            freq1.counter.insert(element);
            //freq_itemsets.insert(element.first);   -->  uncomment this if we want to have also candidate representatives of size 1
        }
    }

    //cand1.counter.clear();
    all_freqk.push_back(freq1);
    return items_number;
}

void generate_cand(ksized_counter &new_cands, ksized_counter &old_cands){

    new_cands.k = old_cands.k + 1;
    new_cands.counter.clear();

    set<int> old_items;

    for (pair<set<int>, int> element : old_cands.counter){
        old_items.insert(element.first.begin(), element.first.end());
    }

    for (pair<set<int>, int> element : old_cands.counter){
        for (int item : old_items){
            if (element.first.count(item) == 0){

                element.first.insert(item);

                if (new_cands.counter.count(element.first) == 0){
                    
                    set<int> temp = element.first;

                    bool to_generate = true;

                    for (int item2 : element.first){
                        
                        temp.erase(item2);

                        if (old_cands.counter.count(temp) == 0){
                            to_generate = false;
                            temp.insert(item2);
                            break;
                        }

                        temp.insert(item2);
                    }

                    if (to_generate){
                        new_cands.counter.insert(pair<set<int>, int>(element.first, 0));
                    }
                }

                element.first.erase(item);
            }
        }
    }
}

// counts how many itemsets of size k are in a set of size d, that is (d k)
int select_counting_algorithm(int d, int k){ // 
	if(k==1)
		return d;
	if(k==d)
		return 1;
	if(k > d)
		return 0;
	return select_counting_algorithm(d-1,k) + select_counting_algorithm(d-1,k-1);
}

// counts how many itemsets of size k are in a set of size d, that is (d k)
int factorial(int n)
{
    if (n == 0 || n == 1)
        return 1;
    return n * factorial(n - 1);
}

int ksized_number(int n, int k){
    int numerator = factorial(n);
    int factorial_k = factorial(k);
    int factorial_nk = factorial(n-k);
    return numerator/(factorial_k * factorial_nk);
}

void subset_finder(set<int> &sset, int size, int left_size, set<int>::iterator index, ksized_counter &candk, set<int> &temp){
    if(left_size==0){
        if(candk.counter.count(temp)>0){
            candk.counter[temp] +=1;
        }
        return;
    }

    for(set<int>::iterator it=index; it!=sset.end(); ++it){
        temp.insert(*it);
        subset_finder(sset,size,left_size-1,++index, candk, temp);
        temp.erase(*it);
    }
    return;
}

void keep_freq(vector<set<Tuple*, tuple_set_comparator> > &database, ksized_counter &candk, vector<ksized_counter> &all_freqk, double min_support, set<set<int> > &freq_itemsets){

    set<int> temp;

    for (int i = 0; i < database.size(); i++){

        set<int> transaction;
        for (Tuple* tuple : database[i]){
            transaction.insert(tuple->item);
        }

        if (transaction.size() >= candk.k){

            if (candk.counter.size() < select_counting_algorithm(transaction.size(), candk.k)){

                for (pair<set<int>, int> element : candk.counter){

                    bool to_count = true;

                    for (int item : element.first){

                        if (transaction.count(item) == 0){
                            to_count = false;
                            break;
                        }

                    }

                    if (to_count){
                        candk.counter[element.first] += 1;
                    }

                }

            } else {
                subset_finder(transaction, transaction.size(), candk.k, transaction.begin(), candk, temp);
            }

        }

    }

    ksized_counter new_freq(candk.k);
    double min_support_count = database.size() * min_support;

    int count;
    for (pair<set<int>, int> element : candk.counter){
        if (element.second >= min_support_count){
            new_freq.counter.insert(element);
            freq_itemsets.insert(element.first);
        }
    }

    all_freqk.push_back(new_freq);

}