#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<map>
#include<unordered_map>
#include<algorithm>

using namespace std;

//global parameters
int minSupp;
double minConf;

//data structure
struct itemset{
    vector<int>items;
    int support = 0;
};

struct asso_rule{
    vector<int>l_items, r_items;
    int support = 0;
    double conf = 0;
};

bool operator==(const vector<int>&v1, const vector<int>&v2){
    if(v1.size() != v2.size()){
        return false;
    }
    int n = v1.size();
    for(int i = 0; i < n; i++){
        if(v1[i] != v2[i]){
            return false;
        }
    }
    return true;
}

bool operator<(const itemset &v1, const itemset &v2){
    int p1 = 0, p2 = 0;
    while(p1 < v1.items.size() && p2 < v2.items.size()){
        if(v1.items[p1] == v2.items[p2]){
            ++p1;
            ++p2;
        }else if(v1.items[p1] < v2.items[p2]){
            return true;
        }else{
            return false;
        }
    }
    if(v1.items.size() < v2.items.size()){
        return true;
    }else{
        return false;
    }
}

vector<int> operator-(const vector<int>&A, const vector<int>&B){// A and B are in ascending order
    vector<int>res;
    if(A.size() < B.size()){
        return res;
    }
    int p = 0;
    for(int ele : A){
        if(ele != B[p]){
            res.push_back(ele);
        }else{
            p++;
        }
    }
    return res;
}

int load_data(vector<vector<int>>& dataset, string path); //load data from file
vector<itemset> find_F1(const vector<vector<int>>& dataset, int minSupp); //find frequent itemset with size 1;
vector<vector<int>>pairwise_combination(const vector<int>&prefix, const vector<int>&elements);
vector<vector<int>> find_candidate_set(vector<itemset>& F); //generate candidate set Ci from Fi-1
bool is_contain(const vector<int>&v1, const vector<int>&v2);//verify if v1 is in v2. e.g, v1 = {2,4}, v2 = {1,2,3,4} then the answer is yes (similar to LCS problem but here use naive implementation.)
vector<itemset> find_Fi(const vector<vector<int>>& dataset, const vector<vector<int>>&C, int minSupp);//confirm and find the frequent itemset in candidate set C
map<vector<int>, int>  apiori(const vector<vector<int>>& dataset, int minSupp); //step 1, frequent itemset mining(of all size)
vector<vector<int>>comb(int num, int k, vector<int>vec);//combination, n choose k. for each I, we have such kind of ways to split and generate rules.
vector<asso_rule> generate_rule_from_I(vector<int>I, int supp_I, double minConf, map<vector<int>, int>& freq_itemset);
vector<asso_rule> generate_asso_rule(map<vector<int>, int>& freq_itemset, double minConf); //setp 2, rule generation
vector<asso_rule> asso_rule_mining(const vector<vector<int>>& dataset, int minSupp, double minConf);
int generate_report(const vector<asso_rule>& rules, string path); //detailing all the asso rules found and their respective support and confidence values.

int main(){
    //load data from file
    string input_path = "../asso.csv", output_path = "report.txt";
    vector<vector<int>> dataset;
    if(load_data(dataset, input_path)){
        cerr << "Error: fail to load data from " << input_path << endl;
        exit(1);
    }
    
    //specify parameters
    int n = dataset.size();
    minSupp = 0.1 * n;
    minConf = 0.9;

    //sort every itemset in dataset alphabatically
    for(int i = 0; i < dataset.size(); i++){
        sort(dataset[i].begin(), dataset[i].end());
    }
    sort(dataset.begin(), dataset.end());
    
    //1:3次, 2:6, 3:1, 4:4, 5:2, 6:6, 7:3, 8:1, 9:5
    // vector<itemset> F1 = find_F1(dataset, minSupp);
    // vector<vector<int>> C = find_candidate_set(F1);
    // vector<itemset> Fi = find_Fi(dataset, C, minSupp);
    //map<vector<int>, int> freq_itemset = apiori(dataset, minSupp);
    // rule generation
    //vector<asso_rule> rules = generate_asso_rule(freq_itemset, minConf);// R: I1->I1/I, supp(I), conf = supp(I1) / supp(I1)
    
    //perform associaction rule mining
    vector<asso_rule> found = asso_rule_mining(dataset, minSupp, minConf);

    //generate report detailing the rules found.
    if(generate_report(found, output_path)){
        cerr << "Error: fail to generate report to " << output_path << endl;
        exit(1);
    }

    return 0;
}

int load_data(vector<vector<int>>& dataset, string path){
    ifstream input(path);
    if(!input.is_open()){
        cerr << "Error: fail to open file" << path << endl;
        return 1;
    }
    string line;
    while(getline(input, line)){
        stringstream ss(line);
        string token;
        vector<int>trans;
        while(getline(ss, token, ',')){
            if(token[0] == ' '){
                token.erase(token.begin());
            }
            int tmp = stoi(token);
            trans.push_back(tmp);
        }
        trans.erase(trans.begin()); //remove the idx for each trans.
        dataset.push_back(trans);
    }
    return 0;
}

vector<itemset> find_F1(const vector<vector<int>>& dataset, int minSupp){
    vector<itemset>F1;
    unordered_map<int, int>mp; //(item_name, support)
    //scan the dataset once
    for(auto trans : dataset){
        for(auto i : trans){
            mp[i]++;
        }
    }
    //fill the result to F1 (TODO: use map<vector<int>, int> ?)
    for(unordered_map<int, int>::iterator it = mp.begin(); it != mp.end(); it++){
        itemset tmp;
        if(it->second > minSupp){
            tmp.items.push_back(it->first);
            tmp.support = it->second;
            F1.push_back(tmp);
        }
    }
    return F1;

}

//pair-wise
vector<vector<int>>pairwise_combination(const vector<int>&prefix, const vector<int>&elements){
    int n = elements.size();
    if(n == 0){
        cerr << "Error: element size = 0 in function: pairwise_combination" << endl;
        exit(1);
    }
    vector<vector<int>>res;
    for(int i = 0; i < n; i++){
        for(int j = i + 1; j < n; j++){
            vector<int>tmp = prefix;
            tmp.push_back(elements[i]);
            tmp.push_back(elements[j]);//do combination
            res.push_back(tmp);
        }
    }
    return res;
}

vector<vector<int>> find_candidate_set(vector<itemset>& F){//core function
    vector<vector<int>> C;
    //sort every itemset in F alphabatically
    for(int i = 0; i < F.size(); i++){
        sort(F[i].items.begin(), F[i].items.end());
    }
    sort(F.begin(), F.end());
    
    vector<int>prefix;
    //group itemset in Fi-1 by prefix
    map<vector<int>, vector<int>>groups; //(prefix, last_eles)
    for(int i = 0; i < F.size(); i++){
        prefix = F[i].items;
        prefix.pop_back();
        groups[prefix].push_back(F[i].items.back());
    }
    //do pair-wise combination to itemset with the same prefix and generate C
    for(auto group : groups){
        if(group.second.size() > 1){
            vector<vector<int>> tmp = pairwise_combination(group.first, group.second);
            C.insert(C.end(), tmp.begin(), tmp.end());
        }
    }    

    return C;
}

bool is_contain(const vector<int>&v1, const vector<int>&v2){//note that v1 and v2 are both in ascending order.
    if(v2.size() < v1.size()){
        return false;
    }
    int p = 0;
    for(auto ele : v2){
        if(ele == v1[p]){
            p++;
        }
    }
    if(p == v1.size()){
        return true;
    }else{
        return false;
    }
}

vector<itemset> find_Fi(const vector<vector<int>>& dataset, const vector<vector<int>>&C, int minSupp){
    int n = C[0].size();
    vector<itemset>res;
    for(auto candidate : C){
        itemset it;
        it.items = candidate;
        for(auto record : dataset){
            if(is_contain(candidate, record)){
                it.support++;
            }
        }
        if(it.support > minSupp){
            res.push_back(it);
        }
    }
    return res;
}

map<vector<int>, int> apiori(const vector<vector<int>>& dataset, int minSupp){
    vector<itemset>freq, curr_F, next_F;
    map<vector<int>, int>freq_mp;
    curr_F = find_F1(dataset, minSupp); //find frequent itemset with size 1
    freq = curr_F;
    while(1){ //find frequent itemset with size i
        vector<vector<int>> C = find_candidate_set(curr_F);
        if(!C.empty()){
            vector<itemset> next_F = find_Fi(dataset, C, minSupp);
            if(!next_F.empty()){
                freq.insert(freq.end(), next_F.begin(), next_F.end());
                curr_F = next_F;
            }else{
                break;
            }
        }else{
            break;
        }
    }

    for(int i = 0; i < freq.size(); i++){
        freq_mp[freq[i].items] = freq[i].support;
    }
    
    return freq_mp;
}

vector<vector<int>>comb(int num, int k, vector<int>vec){
    vector<vector<int>> res;
    if (k == 0) {
        res.push_back({});
        return res;
    }
    if (num < k) {
        return res; // 空的
    }

    if (num == k) {
        res.push_back(vec);
        return res;
    }

    // ---------------------------------------------
    // 核心递归思路：
    //  - 取首元素 vec[0]，然后在剩下 num-1 个元素里选 k-1
    //  - 不取首元素 vec[0]，然后在剩下 num-1 个元素里选 k
    // 再把这两部分结果合并起来。
    // ---------------------------------------------
    
    vector<int> subVec(vec.begin() + 1, vec.end());
    vector<vector<int>> combosFirst = comb(num - 1, k - 1, subVec);
    for (auto & c : combosFirst) {
        c.insert(c.begin(), vec[0]);
        res.push_back(c);
    }

    vector<vector<int>> combosSkip = comb(num - 1, k, subVec);
    res.insert(res.end(), combosSkip.begin(), combosSkip.end());

    return res;
}

vector<asso_rule> generate_rule_from_I(vector<int>I, int supp_I, double minConf, map<vector<int>, int>& freq_itemset){
    int n = I.size();
    vector<asso_rule> rules;
    if(n == 1){
        return rules; // no rules generated
    }
    //I1->I/I1
    for(int i = 1; i < n; i++){
        //I1 with length i (we will have n choose i such I1)
        vector<vector<int>>I1 = comb(n, i, I);
        for(int j = 0; j < I1.size(); j++){//compute the confidence for each candidate rule
            double conf = (double)supp_I / (double)freq_itemset[I1[j]];
            if(conf > minConf){
                asso_rule r;
                r.l_items = I1[j];
                r.r_items = I-I1[j];
                r.support = supp_I;
                r.conf = conf;
                rules.push_back(r);
            }
        }
    }
    return rules;
}

vector<asso_rule> generate_asso_rule(map<vector<int>, int>& freq_itemset, double minConf){
    vector<asso_rule>rules;
    map<vector<int>, int>::iterator it ;
    for(it = freq_itemset.begin(); it != freq_itemset.end(); it++){
        vector<asso_rule>r_i = generate_rule_from_I(it->first, it->second, minConf, freq_itemset);//multiply ways to split I
        rules.insert(rules.end(), r_i.begin(), r_i.end());
    }
    return rules;
}

vector<asso_rule> asso_rule_mining(const vector<vector<int>>& dataset, int minSupp, double minConf){
    // step 1: frequent itemset mining
    map<vector<int>, int> freq_itemset = apiori(dataset, minSupp);
    // step 2: rule generation
    vector<asso_rule> rules = generate_asso_rule(freq_itemset, minConf);// R: I1->I1/I, supp(I), conf = supp(I1) / supp(I1)
    return rules;
}

int generate_report(const vector<asso_rule>& rules, string path){
    ofstream output(path);
    if(!output.is_open()){
        cerr << "Error: fail to open file" << path << endl;
        return 1;
    }
    
    if(rules.empty()){
        output << "no rules generated" << endl;
        return 0;
    }

    for(auto r : rules){
        //left half
        output << "{";
        for(int i = 0; i < r.l_items.size(); i++){
            if(i != r.l_items.size() - 1)
                output << r.l_items[i] << ", ";
            else{
                output << r.l_items[i] << "} ";
            }
        }

        output << " --> {" ;
        //right half
        for(int i = 0; i < r.r_items.size(); i++){
            if(i != r.r_items.size() - 1)
                output << r.r_items[i] << ", ";
            else{
                output << r.r_items[i] << "} ";
            }
        }
        //supp & conf
        output << "    " << "support(R) = " << r.support << ", conf(R) = " << r.conf << endl;
    }
    cout << "report file " << path << " successfully generated." << endl;
    return 0;
}