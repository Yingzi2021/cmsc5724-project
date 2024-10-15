#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>  
#include <random>    

using namespace std;

struct Point {
    vector<double> features;
    int label;  // 1 or -1
};

void loadData(const string& filePath, vector<Point>& data, int& dimension, int& numSamples, double& R);//load the dataset from a file
void splitData(const vector<Point>& data, vector<Point>& trainData, vector<Point>& testData, double trainRatio);//shuffle and split the dataset into training and test sets
double dotProduct(const vector<double>& w, const vector<double>& x);
double calculateNorm(const vector<double>& w); // margin = min (w * p)/|w|
double calculateFinalMargin(const vector<Point>& data, const vector<double>& w);
void marginPerceptron(vector<Point>& data, vector<double>& w, double gamma_guess, double Lambda, double R);//modify, add Lambda
double test(const vector<Point>& testData, const vector<double>& w);
double calculateRadius(const vector<Point>& data) {
    double max_radius = 0.0;
    
    for (const auto& p : data) {
        double norm = calculateNorm(p.features);
        max_radius = max(max_radius, norm);
    }

    return max_radius;
}

int main() {
    // Variables to store dataset information
    int dimension = 0, numSamples = 0;
    double R = 0.0, lambda = 2.0; //influence approximation precision(gamma_guess >= gamma_opt / lambda^2)

    vector<string> filePaths = {"../dataset/2d-r16-n10000", "../dataset/4d-r24-n10000", "../dataset/8d-r12-n10000"};  
    for(string filePath : filePaths){
        // Load dataset from file
        vector<Point> data, trainData, testData;
        loadData(filePath, data, dimension, numSamples, R);
        splitData(data, trainData, testData, 0.5);
        vector<double> w(dimension, 0.0);
        double gamma_guess = R;

        // Run Margin Perceptron
        marginPerceptron(trainData, w, gamma_guess, lambda, R);

        // Output the final weight vector
        cout << "Final weight vector: ";
        for (double wi : w) {
            cout << wi << " ";
        }
        cout << endl;
        // Test the model on test data and output accuracy
        double accuracy = test(testData, w);
        cout << "Test accuracy: " << accuracy * 100 << "%" << endl << endl;
    }

    return 0;
}

void loadData(const string& filePath, vector<Point>& data, int& dimension, int& numSamples, double& R) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Error: Could not open the file!" << endl;
        return;
    }

    string line, token;

    // Read the first line to get d, n, and R
    getline(file, line);
    stringstream ss(line);
    getline(ss, token, ','); dimension = stoi(token);
    getline(ss, token, ','); numSamples = stoi(token);
    getline(ss, token, ','); R = stod(token);

    // Read each sample and its label
    while (getline(file, line)) {
        stringstream ss(line);
        Point p;
        // Read features
        for (int i = 0; i < dimension; ++i) {
            getline(ss, token, ',');
            p.features.push_back(stod(token));
        }

        // Read label
        ss >> p.label;
        data.push_back(p);
    }

    file.close();
    cout << "Dataset loaded: " << numSamples << " samples, dimension: " << dimension << ", R: " << R << endl;
}

void splitData(const vector<Point>& data, vector<Point>& trainData, vector<Point>& testData, double trainRatio) {
    // Create a copy of the data and shuffle it
    vector<Point> shuffledData = data;
    random_device rd;
    mt19937 g(rd());  // Mersenne Twister random number generator
    shuffle(shuffledData.begin(), shuffledData.end(), g);

    // Split data into training and testing sets
    size_t trainSize = static_cast<size_t>(trainRatio * shuffledData.size());
    trainData.assign(shuffledData.begin(), shuffledData.begin() + trainSize);
    testData.assign(shuffledData.begin() + trainSize, shuffledData.end());
}

double dotProduct(const vector<double>& w, const vector<double>& x) {
    double result = 0.0;
    for (size_t i = 0; i < w.size(); ++i) {
        result += w[i] * x[i];
    }
    return result;
}

// Calculate L2 norm of the weight vector w
double calculateNorm(const vector<double>& w) {
    double sum = 0.0;
    for (double wi : w) {
        sum += wi * wi;
    }
    return sqrt(sum);
}

//calculate the margin for a set of points and the final weight vector w
double calculateFinalMargin(const vector<Point>& data, const vector<double>& w) {
    double norm_w = calculateNorm(w);
    double margin = numeric_limits<double>::max();

    for (const auto& p : data) {
        double dist = (dotProduct(w, p.features) * p.label) / norm_w;  // Calculate the margin
        if (dist < margin) {
            margin = dist;
        }
    }

    return margin;
}

// Margin Perceptron Algorithm
void marginPerceptron(vector<Point>& data, vector<double>& w, double gamma_guess, double Lambda, double R) {
    int iteration = 0, max_iterations = (12 * R * R)/ (gamma_guess*gamma_guess);
    bool violations_found;

    do {
        violations_found = false;
        // run margin perceptron with parameter gamma_guess
        for (const auto& p : data) {
            double wx = dotProduct(w, p.features);
            // Check for violations based on distance to the plane and label
            if (abs(wx) < gamma_guess / Lambda || (p.label == 1 && wx < 0) || (p.label == -1 && wx > 0)) {
                // Update weight vector w based on the violation
                if (p.label == 1) {
                    for (size_t i = 0; i < w.size(); ++i) {
                        w[i] += p.features[i];  // w = w + p
                    }
                } else {
                    for (size_t i = 0; i < w.size(); ++i) {
                        w[i] -= p.features[i];  // w = w - p
                    }
                }
                violations_found = true;
                break;
            }
        }
        iteration++;

        // Forced termination condition
        if (iteration >= max_iterations) {
            cout << "Forced termination after " << iteration << " iterations." << endl;
            gamma_guess /= Lambda;  // Set new gamma_guess
            iteration = 0;  // Restart
            max_iterations = (12 * R * R)/ (gamma_guess*gamma_guess);
        }

    } while (violations_found);

    cout << "Margin Perceptron converged after " << iteration << " iterations." << endl;

    double final_margin = calculateFinalMargin(data, w);
    cout << "Final margin after convergence: " << final_margin << endl;
}

//test the accuracy of the trained perceptron model
double test(const vector<Point>& testData, const vector<double>& w) {
    int correctPredictions = 0;

    for (const auto& p : testData) {
        double wx = dotProduct(w, p.features);
        int predictedLabel;
        if(wx > 0){
            predictedLabel = 1;
        }else if(wx < 0){
            predictedLabel = -1;
        }else{
            continue;
        }
        if (predictedLabel == p.label) {
            correctPredictions++;
        }
    }

    return static_cast<double>(correctPredictions) / testData.size();  // Return accuracy
}
