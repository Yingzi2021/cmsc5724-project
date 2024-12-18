#include<iostream>
#include <fstream>
#include<sstream>
#include<string>
#include<vector>
#include<map>
#include <fcntl.h>
#include<limits>

using namespace std;

enum attr_names{
    age=0, // continueous
    workclass,
    fnlwgt,// continueous
    education,
    education_num,// continueous
    marital_status,
    occupation,
    relationship,
    race,
    sex,
    capital_gain,// continueous
    capital_loss,// continueous
    hours_per_week,// continueous
    label
};
vector<vector<string>>values = {
        {},
        {"Private", "Self-emp-not-inc", "Self-emp-inc", "Federal-gov", "Local-gov", "State-gov", "Without-pay", "Never-worked"},
        {},
        {"Bachelors", "Some-college", "11th", "HS-grad", "Prof-school", "Assoc-acdm", "Assoc-voc", "9th", "7th-8th", "12th", "Masters", "1st-4th", "10th", "Doctorate", "5th-6th", "Preschool"},
        {},
        {"Married-civ-spouse", "Divorced", "Never-married", "Separated", "Widowed", "Married-spouse-absent", "Married-AF-spouse"},
        {"Tech-support", "Craft-repair", "Other-service", "Sales", "Exec-managerial", "Prof-specialty", "Handlers-cleaners", "Machine-op-inspct", "Adm-clerical", "Farming-fishing", "Transport-moving", "Priv-house-serv", "Protective-serv", "Armed-Forces"},
        {"Wife", "Own-child", "Husband", "Not-in-family", "Other-relative", "Unmarried"},
        {"White", "Asian-Pac-Islander", "Amer-Indian-Eskimo", "Other", "Black"},
        {"Female", "Male"},
        {},
        {},
        {},
        {"<=50K",">50K"}
};

vector<map<string, int>>mappings; // map string to int
vector<vector<int>>max_min(label + 1, vector<int>{INT32_MIN, INT32_MAX, 0});// max, min, interval

// Function declarations
void init_mappings();
void group_continueous_attrs(vector<vector<int>>&dataset);
vector<vector<int>> parse_data(string filepath);
void calc_class_probability(vector<vector<int>>&dataset, vector<double>&class_probability);
vector<vector<vector<double>>> calc_conditional_probability(vector<vector<int>>&dataset);
vector<int>predict(vector<vector<int>>&testset, vector<vector<vector<double>>>&cond_probability, vector<double>&class_probability);
double evaluate(vector<vector<int>>&testset, vector<int>&predicted);
void generate_report(string test_path, string output_path, vector<int>&predict_label);

int main(){
    init_mappings();
    
    //parse data from file
    string train_path = "train.data";
    string test_path = "test.data";
    vector<vector<int>>dataset = parse_data(train_path);
    vector<vector<int>>testset = parse_data(test_path);
    
    //1.Training
    //calculte probability for each label (in this case we only have two labels)
    vector<double>class_probability(2, 0.0);
    calc_class_probability(dataset, class_probability);

    //calculate conditional probability, i.e. p(x[i] = a | y) & p(x[i] = a | n) for every attribute. 
    //Note that after grouping discrete & continueous attrs become the same
    vector<vector<vector<double>>>cond_probability = calc_conditional_probability(dataset);
    
    //2.Prediction
    vector<int>predict_res = predict(testset,cond_probability,class_probability);

    //3.Evaluation
    double error = evaluate(testset, predict_res);
    cout << "The prediction error rate is " << error << endl;

    //4.Generate report
    string report_path = "report.txt";
    generate_report(test_path,report_path,predict_res);

    return 0;
}

void init_mappings(){
    for(vector<string>attr : values){
        map<string, int>tmp;
        int i = 0;
        for(string ele : attr){
            tmp[ele] = i;
            i++;
        }
        mappings.push_back(tmp);
    }
}

// group continueous attrs into bundles. first find the max & min range of each continueous attribute, then divide it by intervals.
void group_continueous_attrs(vector<vector<int>>&dataset){
    int num = dataset[0].size();

    for(int i = 0; i < num; i++){
        if(i == age || i == fnlwgt || i == education_num || i == capital_gain || i == capital_loss || i == hours_per_week){//continueous
            //traverse the dataset and find its range, store to this max_min
            for(vector<int>record : dataset){
                if(record.size() == 0){
                    break;
                }
                if(record[i] >= max_min[i][0]){// max
                    max_min[i][0] = record[i];
                }
                if(record[i] <= max_min[i][1]){// min
                    max_min[i][1] = record[i];
                }
            }
        }
    }

    //use the max_min to update the dataset(divide continueous attrs into intervals)
    for(int i = 0; i < num; i++){
        if(i == age || i == fnlwgt || i == education_num || i == capital_gain || i == capital_loss || i == hours_per_week){//continueous
            int interval = (max_min[i][0] - max_min[i][1]) / 10;
            max_min[i][2] = interval;
            for(int j = 0; j < dataset.size(); j++){
                dataset[j][i] = (dataset[j][i] - max_min[i][1]) / interval; //"discrete" them to 1 ~ 10. can be restored with max_min table
            }
        }
    }
}

// read file and fill data to a vector for convenience
vector<vector<int>> parse_data(string filepath){
    vector<vector<int>>res;
    
    ifstream input(filepath);
    if(!input){
        cerr << "Unable to open file!" << endl;
        exit(1);
    }
    string line;
    while(getline(input, line)){
        vector<int>record;
        string item;
        int tmp;
        stringstream ss(line);
        int i = 0; // i-th attr in one record
        while(getline(ss, item, ',')){
            if(i == age || i == fnlwgt || i == education_num || i == capital_gain || i == capital_loss || i == hours_per_week){//continueous
                tmp = stoi(item);
            }else{//discrete
                tmp = mappings[i][item];
            }
            record.push_back(tmp);
            i++;
        }
        res.push_back(record);
    }

    group_continueous_attrs(res); //further process. group continueous attrs into bundles

    return res;
}

// calculate probability for each class, i.e.Pr(class = "<=50K") & Pr(class = ">50K")
void calc_class_probability(vector<vector<int>>&dataset, vector<double>&class_probability){
    double num_y, num_n;
    for(vector<int>record : dataset){
        if(record.size() < label + 1){
            continue;
        }
        if((record.size() >= label + 1) && record[label] == 0){
            num_n++;
        }else{
            num_y++;
        }
    }
    class_probability[0] = num_n / dataset.size();
    class_probability[1] = num_y / dataset.size();
}


vector<vector<vector<double>>> calc_conditional_probability(vector<vector<int>>&dataset){
    int num = dataset[0].size();
    vector<vector<vector<double>>>res(num, vector<vector<double>>(20, vector<double>(2, 0.001)));// more than enough

    for(int i = 0; i < num; i++){
        for(vector<int>record : dataset){
            if(record.size() == 0){
                break;
            }
            res[i][record[i]][record[label]] ++;
        }
    }
    
    for(int i = 0; i < num; i++){
        int possible_value_num;
        if(i == age || i == fnlwgt || i == education_num || i == capital_gain || i == capital_loss || i == hours_per_week){//continueous
            possible_value_num = 10;  
        }else{
            possible_value_num = values[i].size();
        }
        
        for(int j = 0; j <= possible_value_num; j++){
            res[i][j][0] /= dataset.size(); // p(x[i] = a | n)
            res[i][j][1] /= dataset.size(); // p(x[i] = a | y)
        }    
    }

    return res;
}

vector<int>predict(vector<vector<int>>&testset, vector<vector<vector<double>>>&cond_probability, vector<double>&class_probability){
    double p_y = 0.0, p_n = 0.0;
    vector<int>predicted_label;
    for(vector<int>record : testset){
        p_y = class_probability[1], p_n = class_probability[0];
        for(int i = 0; i < label; i++){
            p_y *= cond_probability[i][record[i]][1];
            p_n *= cond_probability[i][record[i]][0];
        }
        //predict
        if(p_y >= p_n){
            predicted_label.push_back(1);
        }else{
            predicted_label.push_back(0);
        }
    }

    return predicted_label;
}

//evaluate the classifier in terms of error rate
double evaluate(vector<vector<int>>&testset, vector<int>&predicted){
    double error = 0.0;
    int idx = 0;
    for(vector<int>record : testset){
        if(record[label] != predicted[idx++]){
            error ++;
        }
    }
    error /= testset.size();
    return error;
}

void generate_report(string test_path, string output_path, vector<int>&predict_label){
    ifstream input(test_path);
    if(!input){
        cerr << "Unable to open input file!" << endl;
        exit(1);
    }
    ofstream output(output_path);
    if(!output){
        cerr << "Unable to open output file!" << endl;
        exit(1);
    }

    string line;
    int idx = 0;
    while(getline(input, line)){
        int pos = line.find_last_of(',');
        string true_label = line.substr(pos+1);
        if(mappings[label][true_label] == predict_label[idx++]){
            line += ",successfully classified.\n";
        }else{
            line += ",failed.\n";
        }
        output << line;
    }
    cout << "report file " << output_path << "successfully generated." << endl;
}