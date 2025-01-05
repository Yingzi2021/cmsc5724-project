#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<unordered_set>
#include<set>
#include<cmath>

#define UNCLASSIFIED -1
#define NOISE -2

using namespace std;

//data structure
//2-D point
class point{
public:
    double x, y; 
    int cluster;
    point(): x(0), y(0), cluster(UNCLASSIFIED){}
    point(double a, double b): x(a), y(b), cluster(UNCLASSIFIED){}
    
    bool operator==(const point& other) const{
        return ((this->x == other.x) && (this->y == other.y));
    }
};

//functions
int load_data(const string& path, vector<point>&dataset); //load data from file
double calc_distance(const point& a, const point& b); //calculate distance between 2 points
vector<point>range_query(const vector<point>&dataset, const double Eps, const point& p); //for point p, find and return the points within Eps
void set_union(vector<point>&x, const vector<point>&y); //x = x V y, return 1 if succeed.
void expand_cluster(vector<point>&dataset, vector<point>&neighbor_pts, const int cluster_id, const int minPts, const double Eps); //expand point p and connect all "core" neighbors
void DBSCAN(vector<point>&dataset, const int minPts, const double Eps); //DBSCAN algorithm
int generate_report(const vector<point>&dataset, const string& path); //generate report from clustered dataset.

int main(){
    //input DBSCAN parameters
    int minPts = 0;
    double Eps = 0;
    cout << "input parameter minPts and Eps: " << endl;
    cin >> minPts >> Eps;
    
    //load dataset from file
    string input_path = "../dataset.txt", output_path = "report.txt";
    vector<point>dataset;
    if(!load_data(input_path, dataset)){
        cerr << "Error: Fail to load data from path "<< input_path << endl;
        exit(1);
    }

    /*
    x x x
    x x x

        x

        x x x
    */
    //vector<point>tmp = {point(0,0), point(0.5,0),point(1,0),point(0,0.5),point(0.5,0.5),point(1,0.5),point(1,1.5),point(1,2.5),point(1.5,2.5),point(2,2.5)};

    //do clustering
    DBSCAN(dataset, minPts, Eps);

    //generate report
    if(!generate_report(dataset, output_path)){
        cerr << "Error: Fail to generate report "<< output_path << endl;
        exit(1);
    }

    return 0;
}

int load_data(const string& path, vector<point>&dataset){
    ifstream input(path);
    if(!input.is_open()){
        cerr << "Error: Fail to open "<< path << endl;
        exit(1);
    }
    string line;
    point p;
    while(getline(input, line)){
        stringstream ss(line);
        if(ss >> p.x >> p.y){
            dataset.push_back(p);
        }else{
            cerr << "Error: Fail to extract coordinates from "<< line << endl;
            return 0;
        }
    }
    return 1;
}

double calc_distance(const point& a, const point& b){
    double x_diff = a.x - b.x;
    double y_diff = a.y - b.y;
    return sqrt(pow(x_diff, 2) + pow(y_diff, 2));
}

vector<point>range_query(const vector<point>&dataset, const double Eps, const point& p){
    vector<point>result;
    for(int i = 0; i < dataset.size(); i++){
        if(calc_distance(p, dataset[i]) <= Eps){
            result.push_back(dataset[i]);
        }
    }
    return result;
}

void set_union(vector<point>&x, const vector<point>&y){
    for(point p : y){
        bool flag = true;
        for(point q : x){
            if(p == q){
                flag = false;
                break;
            }
        }
        if(flag){//distinct
            x.push_back(p);//append
        }
    }
}

//待优化：neighbor_pts中存对应点在dataset中的idx，这样就可以省去fill部分。
void expand_cluster(vector<point>&dataset, vector<point>&neighbor_pts, const int cluster_id, const int minPts, const double Eps){
    for(int i = 0; i < neighbor_pts.size(); i++){
        if(neighbor_pts[i].cluster == UNCLASSIFIED){
            vector<point>neighbors = range_query(dataset, Eps, neighbor_pts[i]);
            if(neighbors.size() >= minPts){ //neighbor_pts[idx] is a core point, expand
                neighbor_pts[i].cluster = cluster_id;
                set_union(neighbor_pts, neighbors);
            }else{
                neighbor_pts[i].cluster = NOISE;
            }
        }
    }
    //fill the change to dataset
    for(point p : neighbor_pts){
        for(int i = 0; i < dataset.size(); i++){
            if(p.x == dataset[i].x && p.y == dataset[i].y){
                dataset[i].cluster = p.cluster;
            }
        }
    }
}

void DBSCAN(vector<point>&dataset, const int minPts, const double Eps){
    int cluster_id = 0;
    for(int i = 0; i < dataset.size(); i++){
        if(dataset[i].cluster == UNCLASSIFIED){
            vector<point>neighbor = range_query(dataset, Eps, dataset[i]);
            if(neighbor.size() >= minPts){//core point
                cluster_id++;
                expand_cluster(dataset, neighbor, cluster_id, minPts, Eps);
            }else{//non-core point
                dataset[i].cluster = NOISE;
            }
        }
    }
}

int generate_report(const vector<point>&dataset, const string& path){
    ofstream output(path);
    if(!output.is_open()){
        cerr << "Error: open "<< path << " fail" << endl;
        exit(1);
    }
    for(point p : dataset){
        output << p.x << " " << p.y << " " << p.cluster << endl;
    }
    return 1;
}
