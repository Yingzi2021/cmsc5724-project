#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<cmath>
#include<limits.h>

#define K_CENTER 0
#define K_MEANS 1

using namespace std;

//data structure
class point{
public:
    int x;  // x coordinate
    int y;  // y coordinate
    int cluster; //belong to which cluster, initial is 0.
    double centroid_distance; // the "Mcdonald" distance

    point(){
      x = 0, y = 0, cluster = -1;  
      centroid_distance = -1;
    }    
    
    //calculate the distance between 2 points.
    double eu_distance(point A, point B){
        return pow((A.x - B.x),2) + pow((A.y - B.y),2);
    }

    //assign a point to some cluuster
    void do_clustering(int cluster_idx){
        this->cluster = cluster_idx;
    }
};

class centriods{
public:
    vector<point>centriod_set;  // a centriod set(points)
    int num;                    // size of the centriod set
    
    centriods(){
        this->num = 0;
    }
    
    // init the centriod set with an arbitrary point from the dataset.
    void init(vector<point>&dataset){//k-center
        dataset[0].centroid_distance = 0;
        dataset[0].cluster = 1;
        this->centriod_set.push_back(dataset[0]);
        this->num = 1;
    }
    //k-means init
    void init(vector<point>&dataset, int k){
        for(int i = 0; i < k; i++){
            dataset[i].centroid_distance = 0;
            dataset[i].cluster = i+1;
            this->centriod_set.push_back(dataset[i]);
        }
        this->num = k;
    }

    // add a point to the set
    void add(point& p){
        p.cluster = ++(this->num);
        p.centroid_distance = 0;
        this->centriod_set.push_back(p);
    }

    //k-means, compare two centroid set and see if they are equal.
    bool is_equal(centriods New){
        int size = this->centriod_set.size();
        for(int i = 0; i < size; i++){
            if((this->centriod_set[i].x != New.centriod_set[i].x) || 
            (this->centriod_set[i].y != New.centriod_set[i].y) ||
            (this->centriod_set[i].cluster != New.centriod_set[i].cluster)
            ){
                return false;
            }
        } 
        return true;
    }
};


int load_data(string path, vector<point>&dataset);
int generate_report(const vector<point>& dataset, centriods C, int mode, string path);
// k-center
void calc_centroid_distance(point& p, centriods c);
void k_center(vector<point>&dataset, centriods& C, int centroid_num);
//k-means
struct point calc_geometric_center(int i, const vector<point>& dataset);//calculate geometric center for cluster_i
void k_means(vector<point>&dataset, centriods& C, int centroid_num);

int main(){
    //load dataset
    vector<point>dataset;
    string input_path = "../dataset.txt", output_path = "kmeans-report.txt";
    if(load_data(input_path, dataset)){
        cerr << "Error: Fail to load data" << endl;
        exit(1);
    }
    //perform clusttering
    centriods C;
    //1.k-center
    //k_center(dataset, C, 8);
    //2.k-means
    k_means(dataset, C, 8);

    //generate report
    if(generate_report(dataset, C, K_MEANS, output_path)){
        cerr << "Error: Fail to generate report" << endl;
        exit(1);
    }else{
        cout << "report file " << output_path << " successfully generated." << endl;
    }

    return 0;
}

//helper functions
int load_data(string path, vector<point>&dataset){
    //open file
    ifstream input(path);
    if(!input.is_open()){
        cerr << "Error: Could not open file" << path << endl;
        return 1;
    }
    //read each line
    string line;
    point p;
    while(getline(input, line)){//parse each line
        stringstream ss(line);
        ss >> p.x >> p.y;
        dataset.push_back(p);
    }
    input.close();
    return 0;
}

int generate_report(const vector<point>& dataset, centriods C, int mode, string path){
    ofstream output(path);
    if(!output.is_open()){
        cerr << "Error: Could not open file " << path << endl;
        return 1;
    }

    if(mode == K_CENTER){
        for(int i = 0; i < dataset.size(); i++){
            output << dataset[i].x << " " << dataset[i].y << " " << dataset[i].cluster << " " << dataset[i].centroid_distance << endl;
        }    
    }else if(mode == K_MEANS){
        for(struct point p : C.centriod_set){//print centroids(not in the dataset)
            output << p.x << " " << p.y << " " << p.cluster << " " << p.centroid_distance << endl;
        }    
        for(int i = 0; i < dataset.size(); i++){
            output << dataset[i].x << " " << dataset[i].y << " " << dataset[i].cluster << " " << -1 << endl;
        }
    }else{
        cerr << "Error: Invalid mode " << path << endl;
        return 1;
    }
    
    output.close();
    return 0;
}

// k-center
// calculate the dC(o) for a point p
void calc_centroid_distance(point& p, centriods c){
    double centroid_distance = MAXFLOAT;
    int cluster_idx = -1;
    for(point centroid : c.centriod_set){
        double tmp = p.eu_distance(centroid, p);
        if(tmp < centroid_distance){
            centroid_distance = tmp;
            cluster_idx = centroid.cluster;
        }
    }
    p.centroid_distance = centroid_distance;
    p.do_clustering(cluster_idx); // (re)cluster p
}

// k-center
void k_center(vector<point>&dataset, centriods& C, int centroid_num){
    C.init(dataset); //init C

    for(int i = 1; i <= centroid_num; i++){
        // find p with largest dC(p) ("cost" defined by this problem)
        int max_idx = 0;
        double max_dist = -1;
        for(int j = 0; j < dataset.size(); j++){
            calc_centroid_distance(dataset[j], C);
            if(dataset[j].centroid_distance > max_dist){
                max_dist = dataset[j].centroid_distance;
                max_idx = j;
            }
        }
        // add dataset[max_idx] to C
        if(i != centroid_num){
            C.add(dataset[max_idx]);
            cout << max_dist << endl;
        }   
    }
}

struct point calc_geometric_center(int i, const vector<point>& dataset){
    struct point new_centroid;
    int ans_x = 0, ans_y = 0, num = 0;
    for(struct point p: dataset){
        if(p.cluster == i){
            ans_x += p.x;
            ans_y += p.y;
            num++;
        }
    }
    new_centroid.x = ans_x / num;
    new_centroid.y = ans_y / num;
    new_centroid.cluster = i;
    new_centroid.centroid_distance = 0;
    return new_centroid;
}

//k-means
void k_means(vector<point>&dataset, centriods& C, int centroid_num){
    int threshold = 0;
    //init
    C.init(dataset, centroid_num); //choose k centrorids arbitrarily
    centriods old_set;
    do{
        old_set.centriod_set.clear();
        old_set.num = 0;

        threshold++;
        if(threshold > INT_MAX){
            cerr << "Error: might be infinite loop" << endl;
            exit(1);
        }
        //assign C to C_old
        for(struct point p : C.centriod_set){
            old_set.add(p);
        }
        C.centriod_set.clear();
        C.num = 0;

        //partition the dataset with respect to centroid set C_old
        for(int i = 0; i < dataset.size(); i++){
            double min_dist = MAXFLOAT;
            int idx = 0;
            for(int j = 0; j < centroid_num; j++){
                double tmp = dataset[i].eu_distance(dataset[i], old_set.centriod_set[j]);
                if(tmp < min_dist){
                    min_dist = tmp;
                    idx = old_set.centriod_set[j].cluster;
                }
            }
            dataset[i].centroid_distance = min_dist;
            dataset[i].do_clustering(idx);
        }

        //calculate the geometric center and form the new centroid set
        for(int i = 0; i < centroid_num; i++){
            struct point c_i = calc_geometric_center(i + 1, dataset);
            C.add(c_i);
        }

    }while(!C.is_equal(old_set));
}