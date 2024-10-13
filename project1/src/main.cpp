#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

using namespace std;

// Data structures for storing features and category mappings
vector<string> features;
vector<bool> isCategorical;
map<string, unordered_map<string, int>> categoryHash;

// Paths to data files
string namesPath = "../adult/adult.names";
string trainDataPath = "../adult/adult.data";
string testDataPath = "../adult/adult.test";

// Structure to hold a data record
struct DataRecord {
    vector<double> attributes;
    string label; // The class label
};

// Structure for a node in the decision tree
struct TreeNode {
    bool isLeaf;
    string label; // If it's a leaf, the class label
    int splitAttribute; // Attribute index to split on
    double splitValue; // The value to split at (for continuous attributes) or category value
    TreeNode* left; // Left child node
    TreeNode* right; // Right child node
    TreeNode() : isLeaf(false), splitAttribute(-1), splitValue(0.0), left(nullptr), right(nullptr) {}
};

// Function declarations
void initFeaturesAndCategories();
string getProcessedFilename(const string& originalPath);
void preprocessing(const string& inputPath, const string& outputPath);
void loadData(const string& path, vector<DataRecord>& dataSet, bool isTestData = false);
double calc_gini_index(const vector<DataRecord>& group, int total_instances);
double find_best_split(const vector<DataRecord>& dataSet, int attribute_index, double& best_split_value, vector<DataRecord>& best_left, vector<DataRecord>& best_right);
TreeNode* buildDecisionTree(const vector<DataRecord>& dataSet, int max_depth, int min_size, int depth);
string predict(TreeNode* node, const DataRecord& record);
void printTree(TreeNode* node, string indent = "");
void saveTreeToFile(TreeNode* node, ofstream& outFile, string indent = "");
void generateClassificationReport(const vector<DataRecord>& testData, TreeNode* root, const string& reportFilePath);

// parameters for stopping criteria
const int MAX_DEPTH = 10;      // Maximum depth of the tree
const int MIN_SIZE = 10;       // Minimum number of samples required to split

int main() {
    initFeaturesAndCategories();

    // Preprocess the training data
    string processedTrainDataPath = getProcessedFilename(trainDataPath);
    preprocessing(trainDataPath, processedTrainDataPath);

    // Load training data
    vector<DataRecord> trainingData;
    loadData(processedTrainDataPath, trainingData);
    cout << "Number of training records loaded: " << trainingData.size() << endl;

    // Build decision tree using the Hunt algorithm with stopping criteria
    TreeNode* root = buildDecisionTree(trainingData, MAX_DEPTH, MIN_SIZE, 1);

    // Save the decision tree to a file
    ofstream treeFile("decision_tree.txt");
    if (treeFile.is_open()) {
        saveTreeToFile(root, treeFile);
        treeFile.close();
        cout << "Decision tree saved to 'decision_tree.txt'" << endl;
    } else {
        cerr << "Error: Could not open 'decision_tree.txt' for writing." << endl;
    }

    // Preprocess and load test data
    string processedTestDataPath = getProcessedFilename(testDataPath);
    preprocessing(testDataPath, processedTestDataPath);

    vector<DataRecord> testData;
    loadData(processedTestDataPath, testData, true); // 'true' indicates this is test data
    cout << "Number of test records loaded: " << testData.size() << endl;

    // Evaluate the decision tree on the test data
    int correctPredictions = 0;
    for (const DataRecord& record : testData) {
        string predictedLabel = predict(root, record);
        if (predictedLabel == record.label) {
            correctPredictions++;
        }
    }

    double accuracy = (double)correctPredictions / testData.size();
    cout << "Accuracy on test data: " << accuracy * 100 << "%" << endl;

    // Generate classification report
    generateClassificationReport(testData, root, "classification_report.txt");
    cout << "Classification report saved to 'classification_report.txt'" << endl;

    return 0;
}

void initFeaturesAndCategories() {
    // Initialize features and their types (categorical or continuous)
    features = {
        "age", "workclass", "fnlwgt", "education", "education-num",
        "marital-status", "occupation", "relationship", "race", "sex",
        "capital-gain", "capital-loss", "hours-per-week"
    };
    isCategorical = {
        false, true, false, true, false,
        true, true, true, true, true,
        false, false, false
    };
}

string getProcessedFilename(const string& originalPath) {
    size_t dotPos = originalPath.find_last_of('.');
    if (dotPos == string::npos) {
        return originalPath + "_processed";
    }
    return originalPath.substr(0, dotPos) + "_processed" + originalPath.substr(dotPos);
}

void preprocessing(const string& inputPath, const string& outputPath) {
    ifstream file(inputPath);
    if (!file.is_open()) {
        cerr << "Error: Could not open the file " << inputPath << "!" << endl;
        return;
    }
    ofstream outFile(outputPath);
    if (!outFile.is_open()) {
        cerr << "Error: Could not open the output file " << outputPath << "!" << endl;
        file.close();
        return;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue; // skip empty lines

        // Skip lines that start with '|' (comments in 'adult.test')
        if (line[0] == '|') continue;

        stringstream ss(line);
        string token;
        vector<string> record;
        bool missingValue = false;
        while (getline(ss, token, ',')) {
            token.erase(0, token.find_first_not_of(' '));
            token.erase(token.find_last_not_of(' ') + 1);
            // Remove records with missing values
            if (token == "?") {
                missingValue = true;
                break;
            }
            record.push_back(token);
        }
        if (missingValue) {
            continue;
        }

        if (record.size() == 0) continue; // skip if the record is empty

        // Remove the "native-country" attribute (second last attribute)
        if (record.size() >= 2) {
            record.erase(record.end() - 2);
        }

        // Convert categorical features to integer values
        for (size_t i = 0; i < record.size(); ++i) {
            if (i < features.size() && isCategorical[i]) {
                string featureName = features[i];
                string categoricalValue = record[i];
                // Build the mapping if not already present
                if (categoryHash[featureName].find(categoricalValue) == categoryHash[featureName].end()) {
                    int newId = categoryHash[featureName].size();
                    categoryHash[featureName][categoricalValue] = newId;
                }
                record[i] = to_string(categoryHash[featureName][categoricalValue]);
            }
        }

        // Write the processed record to the output file
        for (size_t i = 0; i < record.size(); ++i) {
            outFile << record[i];
            if (i < record.size() - 1) {
                outFile << ", ";
            }
        }
        outFile << "\n";
    }
    outFile.close();
    file.close();
}

void loadData(const string& path, vector<DataRecord>& dataSet, bool isTestData) {
    ifstream file(path);
    if (!file.is_open()) {
        cerr << "Error: Could not open the file " << path << "!" << endl;
        return;
    }

    string line;
    int lineNumber = 0;
    while (getline(file, line)) {
        lineNumber++;
        if (line.empty()) continue; // skip empty lines
        stringstream ss(line);
        string token;
        vector<string> tokens;
        while (getline(ss, token, ',')) {
            token.erase(0, token.find_first_not_of(' '));
            token.erase(token.find_last_not_of(' ') + 1);
            tokens.push_back(token);
        }
        // The last token is the label
        if (tokens.empty()) continue;
        string label = tokens.back();
        tokens.pop_back();

        // For test data, labels may have a period at the end, remove it
        if (isTestData && !label.empty() && label.back() == '.') {
            label.pop_back();
        }

        if (tokens.size() != features.size()) {
            cerr << "Error at line " << lineNumber << ": Number of tokens (" << tokens.size() << ") does not match number of features (" << features.size() << ")" << endl;
            continue;
        }
        vector<double> attributes;
        for (const string& s : tokens) {
            double val = stod(s);
            attributes.push_back(val);
        }
        dataSet.push_back({attributes, label});
    }
    file.close();
}

double calc_gini_index(const vector<DataRecord>& group, int total_instances) {
    unordered_map<string, int> label_counts;
    for (const DataRecord& record : group) {
        label_counts[record.label]++;
    }
    double gini = 1.0;
    for (const auto& pair : label_counts) {
        double proportion = (double)pair.second / group.size();
        gini -= proportion * proportion;
    }
    return gini * ((double)group.size() / total_instances);
}

double find_best_split(const vector<DataRecord>& dataSet, int attribute_index, double& best_split_value, vector<DataRecord>& best_left, vector<DataRecord>& best_right) {
    double best_gini = 1.0;
    best_split_value = 0.0;

    int total_instances = dataSet.size();

    // For continuous attributes, sort data and compute Gini index incrementally
    if (!isCategorical[attribute_index]) {
        // Sort the data based on the attribute
        vector<DataRecord> sortedData = dataSet;
        sort(sortedData.begin(), sortedData.end(), [attribute_index](const DataRecord& a, const DataRecord& b) {
            return a.attributes[attribute_index] < b.attributes[attribute_index];
        });

        unordered_map<string, int> left_counts, right_counts;
        int left_size = 0;
        int right_size = total_instances;

        // Initialize right_counts with all instances
        for (const DataRecord& record : sortedData) {
            right_counts[record.label]++;
        }

        // Iterate through sorted data and consider splits between different attribute values
        for (int i = 0; i < total_instances - 1; ++i) {
            const DataRecord& record = sortedData[i];
            // Move record from right to left
            left_counts[record.label]++;
            right_counts[record.label]--;
            left_size++;
            right_size--;

            // If next attribute value is the same, skip to avoid redundant splits
            if (sortedData[i].attributes[attribute_index] == sortedData[i + 1].attributes[attribute_index]) {
                continue;
            }

            // Compute Gini index for this split
            double gini_left = 1.0;
            for (const auto& pair : left_counts) {
                double proportion = (double)pair.second / left_size;
                gini_left -= proportion * proportion;
            }
            gini_left *= ((double)left_size / total_instances);

            double gini_right = 1.0;
            for (const auto& pair : right_counts) {
                if (pair.second == 0) continue;
                double proportion = (double)pair.second / right_size;
                gini_right -= proportion * proportion;
            }
            gini_right *= ((double)right_size / total_instances);

            double gini = gini_left + gini_right;

            // Update best split if current gini is lower
            if (gini < best_gini) {
                best_gini = gini;
                best_split_value = (sortedData[i].attributes[attribute_index] + sortedData[i + 1].attributes[attribute_index]) / 2.0;
            }
        }

        // After finding the best split value, split the data
        for (const DataRecord& record : dataSet) {
            if (record.attributes[attribute_index] <= best_split_value) {
                best_left.push_back(record);
            } else {
                best_right.push_back(record);
            }
        }
    } else {
        // For categorical attributes, split on each category
        unordered_set<double> categories;
        for (const DataRecord& record : dataSet) {
            categories.insert(record.attributes[attribute_index]);
        }

        for (double category : categories) {
            vector<DataRecord> left, right;
            for (const DataRecord& record : dataSet) {
                if (record.attributes[attribute_index] == category) {
                    left.push_back(record);
                } else {
                    right.push_back(record);
                }
            }
            if (left.empty() || right.empty()) continue;

            double gini_left = calc_gini_index(left, total_instances);
            double gini_right = calc_gini_index(right, total_instances);
            double gini = gini_left + gini_right;

            if (gini < best_gini) {
                best_gini = gini;
                best_split_value = category;
                best_left = left;
                best_right = right;
            }
        }
    }

    return best_gini;
}

TreeNode* buildDecisionTree(const vector<DataRecord>& dataSet, int max_depth, int min_size, int depth) {
    // Create root node or leaf node depending on stopping criteria
    unordered_map<string, int> label_counts;
    for (const DataRecord& record : dataSet) {
        label_counts[record.label]++;
    }

    // Majority label
    string majority_label;
    int max_count = 0;
    for (const auto& pair : label_counts) {
        if (pair.second > max_count) {
            max_count = pair.second;
            majority_label = pair.first;
        }
    }

    // Check for stopping criteria
    if (label_counts.size() == 1 || depth >= max_depth || dataSet.size() <= min_size) {
        TreeNode* leaf = new TreeNode();
        leaf->isLeaf = true;
        leaf->label = majority_label;
        return leaf;
    }

    // Try splitting on each attribute
    double best_gini = 1.0;
    int best_attribute = -1;
    double best_split_value = 0.0;
    vector<DataRecord> best_left, best_right;

    for (size_t attribute_index = 0; attribute_index < dataSet[0].attributes.size(); ++attribute_index) {
        vector<DataRecord> left, right;
        double split_value;
        double gini = find_best_split(dataSet, attribute_index, split_value, left, right);
        if (gini < best_gini && !left.empty() && !right.empty()) {
            best_gini = gini;
            best_attribute = attribute_index;
            best_split_value = split_value;
            best_left = left;
            best_right = right;
        }
    }

    if (best_attribute == -1) {
        // Could not find a split, return a leaf node with the majority label
        TreeNode* leaf = new TreeNode();
        leaf->isLeaf = true;
        leaf->label = majority_label;
        return leaf;
    }

    // Create internal node
    TreeNode* node = new TreeNode();
    node->splitAttribute = best_attribute;
    node->splitValue = best_split_value;

    // Recursively build left and right subtrees
    node->left = buildDecisionTree(best_left, max_depth, min_size, depth + 1);
    node->right = buildDecisionTree(best_right, max_depth, min_size, depth + 1);

    return node;
}

string predict(TreeNode* node, const DataRecord& record) {
    if (node->isLeaf) {
        return node->label;
    } else {
        double attribute_value = record.attributes[node->splitAttribute];
        if (isCategorical[node->splitAttribute]) {
            if (attribute_value == node->splitValue) {
                return predict(node->left, record);
            } else {
                return predict(node->right, record);
            }
        } else {
            if (attribute_value <= node->splitValue) {
                return predict(node->left, record);
            } else {
                return predict(node->right, record);
            }
        }
    }
}

// print the decision tree
void printTree(TreeNode* node, string indent) {
    if (node->isLeaf) {
        cout << indent << "Leaf: " << node->label << endl;
    } else {
        cout << indent << "Node: " << features[node->splitAttribute] << " ";
        if (isCategorical[node->splitAttribute]) {
            // Reverse lookup from categoryHash to convert integer back to string
            string featureName = features[node->splitAttribute];
            int categoryValue = static_cast<int>(node->splitValue);

            string categoryString = "";
            // Iterate over the categoryHash to find the string corresponding to the integer value
            for (const auto& entry : categoryHash[featureName]) {
                if (entry.second == categoryValue) {
                    categoryString = entry.first;
                    break;
                }
            }

            cout << "== " << categoryString << endl;
            cout << indent << "--> Left:" << endl;
            printTree(node->left, indent + "  ");
            cout << indent << "--> Right:" << endl;
            printTree(node->right, indent + "  ");
        } else {
            // For continuous attributes, we use the splitValue directly
            cout << "<= " << node->splitValue << endl;
            cout << indent << "--> Left:" << endl;
            printTree(node->left, indent + "  ");
            cout << indent << "--> Right:" << endl;
            printTree(node->right, indent + "  ");
        }
    }
}

// Function to save the decision tree to a file
void saveTreeToFile(TreeNode* node, ofstream& outFile, string indent) {
    if (node->isLeaf) {
        outFile << indent << "Leaf: " << node->label << "\n";
    } else {
        outFile << indent << "Node: " << features[node->splitAttribute] << " ";
        if (isCategorical[node->splitAttribute]) {
            // Find the categorical value corresponding to the split value
            string categoryName;
            for (const auto& pair : categoryHash[features[node->splitAttribute]]) {
                if (pair.second == static_cast<int>(node->splitValue)) {
                    categoryName = pair.first;
                    break;
                }
            }
            outFile << "== " << categoryName << "\n";
            outFile << indent << "--> Left:\n";
            saveTreeToFile(node->left, outFile, indent + "  ");
            outFile << indent << "--> Right:\n";
            saveTreeToFile(node->right, outFile, indent + "  ");
        } else {
            outFile << "<= " << node->splitValue << "\n";
            outFile << indent << "--> Left:\n";
            saveTreeToFile(node->left, outFile, indent + "  ");
            outFile << indent << "--> Right:\n";
            saveTreeToFile(node->right, outFile, indent + "  ");
        }
    }
}

// Function to generate the classification report
void generateClassificationReport(const vector<DataRecord>& testData, TreeNode* root, const string& reportFilePath) {
    ofstream reportFile(reportFilePath);
    if (!reportFile.is_open()) {
        cerr << "Error: Could not open '" << reportFilePath << "' for writing." << endl;
        return;
    }

    reportFile << "Classification Report\n";
    reportFile << "---------------------\n";

    int correctPredictions = 0;
    int recordNumber = 1;
    for (const DataRecord& record : testData) {
        string predictedLabel = predict(root, record);
        bool correct = (predictedLabel == record.label);

        // Write record number
        reportFile << "Record " << recordNumber << ":\n";

        // Write attributes
        reportFile << "Attributes:\n";
        for (size_t i = 0; i < record.attributes.size(); ++i) {
            reportFile << "  " << features[i] << ": ";
            if (isCategorical[i]) {
                // Convert categorical value back to original category name
                string categoryName;
                for (const auto& pair : categoryHash[features[i]]) {
                    if (pair.second == static_cast<int>(record.attributes[i])) {
                        categoryName = pair.first;
                        break;
                    }
                }
                reportFile << categoryName;
            } else {
                reportFile << record.attributes[i];
            }
            reportFile << "\n";
        }

        // Write actual and predicted labels
        reportFile << "Actual Label: " << record.label << "\n";
        reportFile << "Predicted Label: " << predictedLabel << "\n";

        // Indicate whether the prediction was correct
        reportFile << "Classification: " << (correct ? "Correct" : "Incorrect") << "\n";
        reportFile << "---------------------\n";

        if (correct) {
            correctPredictions++;
        }
        recordNumber++;
    }

    double accuracy = (double)correctPredictions / testData.size();
    reportFile << "Overall Accuracy: " << accuracy * 100 << "%\n";

    reportFile.close();
}