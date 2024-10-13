#include<iostream>
#include <fstream>
#include <sstream>
#include<string>
#include<vector>
#include<map>
#include<unordered_map>
using namespace std;

// hash for convert categorical feature into integer
vector<string>categoricalFeatures = {"workclass", "education", "marital-status", "occupation", "relationship", "race", "sex", "native-country"};
map<string, unordered_map<string, int>>categoryHash;

void initMaps(string path);
void preprocessing(string path); // process the categorical / integer feature; dealing with missing value.
int find_candidate_split(int type, int attribute); // find way to split a attribute
int calc_gini_index(int S, int S1, int labelYes1, int labelYes2); //calculate the GINI index after a split.


int main(){
    initMaps("../adult/adult.names");
    for(const auto& category: categoryHash){
        cout << "category:" << category.first << endl;
        for (const auto& entry : category.second) {
            cout << "  " << entry.first << " -> " << entry.second << endl;
        }
    }
    return 0;
}

void initMaps(string path){
    ifstream file(path);
        if (!file.is_open()) {
            cerr << "Error: Could not open the file!" << endl;
            return;
        }

        string line;
        // Read the file line by line
        while (getline(file, line)) {
            // Iterate through all categorical features
            for (const string &category : categoricalFeatures) {
                if (line.rfind(category + ":", 0) == 0) { 
                    string values_str = line.substr(category.length() + 2); // Skip "category:" and the following space
                    // Split the remaining string by commas to get the individual values
                    stringstream ss(values_str);
                    string value;
                    int id = 0; // Used to assign a unique integer to each category value
                    while (getline(ss, value, ',')) {
                        // Trim leading and trailing spaces
                        value.erase(0, value.find_first_not_of(' ')); 
                        value.erase(value.find_last_not_of(' ') + 1); 

                        // Map the category value to a unique integer
                        categoryHash[category][value] = id++;
                    }
                    break; // Exit the loop once the matching category is found and processed
                }
            }
        }
        
        file.close();
}