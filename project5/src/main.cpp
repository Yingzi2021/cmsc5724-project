#include<iostream>
#include<string>
#include<vector>
#include<map>
#include <png.h>
#include <cmath>
#include <iomanip> // For controlling output precision
using namespace std;

//for eigenvalues and eigenvector calculation
const double EPS = 1e-12; // Convergence threshold
const int MAX_ITER = 500; // Maximum number of iterations

int load_data(const string& path, vector<vector<double>>& dataset); //load the original png from `path`
int save_to_png(const string& path, const vector<vector<double>>& dataset); //save the grayscale figure to png format
int move_to_center(vector<vector<double>>& dataset); //move the dataset to its geometric center
vector<vector<double>> calc_covariance_matrix(const vector<vector<double>>& dataset); //calculate covariance matrix A for dataset 
vector<double> normalize(const vector<double>& vec);// Normalize a vector to make it a unit vector
map<double, vector<double>, greater<double>> calc_eigenvalue_and_vector(vector<vector<double>>& A); // calculate eigenvalues and eigenvectors with Jacobi method
vector<vector<double>> PCA(const int k, vector<vector<double>>& dataset); //return k unit eigen vectors in a non ascending order 
double dot_product(const vector<double>&a, const vector<double>&b); //do vector dot product
void vect_add(vector<double>& A, const vector<double>& B); //add two vectors
void scalar_multiply(double f, vector<double>& vec); //Scalar multiplication
//do dimesionality reduction and save the result. 
void dim_reduction(const vector<vector<double>>& dataset, const vector<vector<double>>& subspace_base, const string& output_path); 

int main(){
    int k = 0;
    vector<vector<double>> dataset, subspace;
    string input_path = "../original.png", output_path = "processed.png";

    if(load_data(input_path, dataset)){
        cerr << "Error: Fail to load data from " << input_path << endl;
        exit(1);    
    }

    save_to_png("out.png", dataset);

    cout << "input target dimension k:"<< endl;    
    cin >> k;

/*
            x
0   x               x
            x

*/
    // vector<vector<double>>tmp = {
    //     {1,0},
    //     {5,0},
    //     {3,1},
    //     {3,-1}
    // };

    subspace = PCA(k, dataset);

    dim_reduction(dataset, subspace, output_path);

    return 0;
}

int load_data(const string& path, vector<vector<double>>& dataset) {
    // Open the file in binary mode
    FILE* fp = fopen(path.c_str(), "rb");
    if (!fp) {
        cerr << "Failed to open file: " << path << endl;
        return 1;
    }

    // Read the PNG file signature (first 8 bytes)
    png_byte header[8];
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8)) {
        cerr << "File is not recognized as a PNG file." << endl;
        fclose(fp);
        return 1;
    }

    // Initialize libpng structures
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        cerr << "Failed to create png read struct." << endl;
        fclose(fp);
        return 1;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        cerr << "Failed to create png info struct." << endl;
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        fclose(fp);
        return 1;
    }

    // Set error handling
    if (setjmp(png_jmpbuf(png_ptr))) {
        cerr << "Error during PNG processing." << endl;
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        fclose(fp);
        return 1;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8); // Indicate that the first 8 bytes have already been read

    png_read_info(png_ptr, info_ptr);

    // Retrieve image information
    int width = png_get_image_width(png_ptr, info_ptr);
    int height = png_get_image_height(png_ptr, info_ptr);
    png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    png_byte color_type = png_get_color_type(png_ptr, info_ptr);

    // Verify that the image is 8-bit grayscale
    if (color_type != PNG_COLOR_TYPE_GRAY || bit_depth != 8) {
        if (color_type == PNG_COLOR_TYPE_RGB) {
            png_set_rgb_to_gray(png_ptr, PNG_ERROR_ACTION_NONE, 0.2126, 0.7152);//convert to grayscale
        }else{
            cerr << "This program only supports 8-bit grayscale images." << endl;
            png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
            fclose(fp);
            return 1;
        }
    }

    // Allocate memory for reading rows
    png_bytep* row_pointers = new png_bytep[height];
    for (int y = 0; y < height; ++y) {
        row_pointers[y] = new png_byte[width];
    }

    // Read the image data into row_pointers
    png_read_image(png_ptr, row_pointers);

    // Transfer pixel data into the dataset vector
    dataset.resize(height, vector<double>(width));
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            dataset[y][x] = static_cast<int>(row_pointers[y][x]);
        }
    }

    // Clean up memory
    for (int y = 0; y < height; ++y) {
        delete[] row_pointers[y];
    }
    delete[] row_pointers;

    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    fclose(fp);

    return 0; // Success
}

int save_to_png(const string& path, const vector<vector<double>>& dataset) {
    int height = dataset.size();
    int width = dataset[0].size();

    // Open the file for writing
    FILE* fp = fopen(path.c_str(), "wb");
    if (!fp) {
        cerr << "Failed to open file for writing: " << path << endl;
        return 1;
    }

    // Initialize libpng write structure
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        cerr << "Failed to create png write struct." << endl;
        fclose(fp);
        return 1;
    }

    // Initialize libpng info structure
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        cerr << "Failed to create png info struct." << endl;
        png_destroy_write_struct(&png_ptr, nullptr);
        fclose(fp);
        return 1;
    }

    // Set error handling
    if (setjmp(png_jmpbuf(png_ptr))) {
        cerr << "Error during PNG creation." << endl;
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        return 1;
    }

    png_init_io(png_ptr, fp);

    // Set the PNG file metadata
    png_set_IHDR(
        png_ptr,
        info_ptr,
        width,
        height,
        8, // Bit depth
        PNG_COLOR_TYPE_GRAY, // Grayscale image
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );

    png_write_info(png_ptr, info_ptr);

    // Prepare the row pointers
    png_bytep* row_pointers = new png_bytep[height];
    for (int y = 0; y < height; ++y) {
        row_pointers[y] = new png_byte[width];
        for (int x = 0; x < width; ++x) {
            row_pointers[y][x] = static_cast<png_byte>(dataset[y][x]);
        }
    }

    // Write the image data to the file
    png_write_image(png_ptr, row_pointers);

    // End the write process
    png_write_end(png_ptr, nullptr);

    // Free memory
    for (int y = 0; y < height; ++y) {
        delete[] row_pointers[y];
    }
    delete[] row_pointers;

    // Clean up
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);

    cout << "Image successfully saved to " << path << endl;
    return 0;
}

int move_to_center(vector<vector<double>>& dataset){
    int dim = dataset[0].size(), n = dataset.size();
    if(dim == 0 || n == 0){
        cerr << "Error: invalid dataset size with " << dim << "dim and " << n << "size." << endl;
        return 1;
    }
    vector<int>geo_center(dim, 0);
    //calculate geometric center
    for(int i = 0; i < dim; i++){
        for(int j = 0; j < n; j++){
            geo_center[i] += dataset[j][i];
        }
        geo_center[i] /= n;
    }
    //move the dataset to geometric center
    for(int i = 0; i < n; i++){
        for(int j = 0; j < dim; j++){
            dataset[i][j] -= geo_center[j];
        }
    }
    return 0;
}

vector<vector<double>> calc_covariance_matrix(const vector<vector<double>>& dataset){
    int dim = dataset[0].size(), n = dataset.size();
    if(dim == 0 || n == 0){
        cerr << "Error: invalid dimesion or dataset size"<< endl;
        exit(1);
    }
    vector<vector<double>>res(dim, vector<double>(dim, 0));
    for(int i = 0; i < dim; i++){
        for(int j = 0; j < dim; j++){
            //calculate res[i][j]
            for(int k = 0; k < n; k++){
                res[i][j] += (dataset[k][i]*dataset[k][j]);
            }
        }
    }
    return res;
}

vector<double> normalize(const vector<double>& vec) {
    double norm = 0.0;
    for (double v : vec) {
        norm += v * v;
    }
    norm = sqrt(norm);
    vector<double> unitVec(vec.size());
    for (size_t i = 0; i < vec.size(); ++i) {
        unitVec[i] = vec[i] / norm;
    }
    return unitVec;
}

map<double, vector<double>, greater<double>> calc_eigenvalue_and_vector(vector<vector<double>>& A) {
    int n = A.size();
    vector<vector<double>> eigenvectors(n, vector<double>(n, 0.0));

    // Initialize eigenvectors as the identity matrix
    for (int i = 0; i < n; ++i) {
        eigenvectors[i][i] = 1.0;
    }

    for (int iter = 0; iter < MAX_ITER; ++iter) {
        // Find the largest off-diagonal element by absolute value
        int p = 0, q = 0;
        double maxVal = 0.0;
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (fabs(A[i][j]) > maxVal) {
                    maxVal = fabs(A[i][j]);
                    p = i;
                    q = j;
                }
            }
        }

        // Check for convergence
        if (maxVal < EPS) break;

        // Compute rotation parameters
        double theta = 0.5 * atan2(2 * A[p][q], A[q][q] - A[p][p]);
        double cosTheta = cos(theta);
        double sinTheta = sin(theta);

        // Update matrix A elements
        vector<vector<double>> B = A; // Temporary matrix for updates
        for (int i = 0; i < n; ++i) {
            if (i != p && i != q) {
                B[i][p] = B[p][i] = cosTheta * A[i][p] - sinTheta * A[i][q];
                B[i][q] = B[q][i] = sinTheta * A[i][p] + cosTheta * A[i][q];
            }
        }
        B[p][p] = cosTheta * cosTheta * A[p][p] + sinTheta * sinTheta * A[q][q] - 2 * sinTheta * cosTheta * A[p][q];
        B[q][q] = sinTheta * sinTheta * A[p][p] + cosTheta * cosTheta * A[q][q] + 2 * sinTheta * cosTheta * A[p][q];
        B[p][q] = B[q][p] = 0.0;

        A = B;

        // Update eigenvectors
        for (int i = 0; i < n; ++i) {
            double tempP = cosTheta * eigenvectors[i][p] - sinTheta * eigenvectors[i][q];
            double tempQ = sinTheta * eigenvectors[i][p] + cosTheta * eigenvectors[i][q];
            eigenvectors[i][p] = tempP;
            eigenvectors[i][q] = tempQ;
        }
    }

    // Create a map for eigenvalues and their corresponding eigenvectors
    map<double, vector<double>,greater<double>> result;
    for (int i = 0; i < n; ++i) {
        vector<double> eigenvector(n);
        for (int j = 0; j < n; ++j) {
            eigenvector[j] = eigenvectors[j][i];
        }
        result[A[i][i]] = normalize(eigenvector); // Normalize eigenvector
    }

    return result;
}

vector<vector<double>> PCA(const int k, vector<vector<double>>& dataset){
    if(k <= 0){
        cerr << "Error: invalid k " << k << endl;
        exit(1); 
    }
    
    //1.move the dattaset to its geometric center
    move_to_center(dataset);

    //2.calculate the covariance matrix for dataset
    vector<vector<double>>A = calc_covariance_matrix(dataset);
    
    //3.calculate the eigenvalue and eigenvector and sort to non-ascending order in termms of eigenvalues
    map<double,vector<double>,greater<double>>res = calc_eigenvalue_and_vector(A);
    
    //4.return the unit eigenvectors with top-k eigenvalues.
    vector<vector<double>>subspace;
    std::map<double,vector<double>,greater<double>>::iterator it;
    int idx = 0;
    for(it = res.begin(); it != res.end(); it++){
        subspace.push_back(it->second);
        idx++;
        if(idx == k){
            break;
        }
    }
    return subspace;
}

double dot_product(const vector<double>&a, const vector<double>&b){
    if(a.size() != b.size()){
        cerr << "Error: attempt to perform dot product to vectors with different dims" << endl;
        exit(1);
    }
    int n = a.size();
    double res = 0;
    for(int i = 0; i < n; i++){
        res += (a[i] * b[i]);
    }
    return res;
}

void vect_add(vector<double>& A, const vector<double>& B){
    if(A.size() != B.size()){
        cerr << "Error: attempt to perform dot product to vectors with different dims" << endl;
        exit(1);
    }
    int n = A.size();
    for(int i = 0; i < n; i++){
        A[i] += B[i];
    }
}

void scalar_multiply(double f, vector<double>& vec){
    int n = vec.size();
    for(int i = 0; i < n; i++){
        vec[i] *= f;
    }
}

void dim_reduction(const vector<vector<double>>& dataset, const vector<vector<double>>& subspace_base, const string& output_path){
    int n = dataset.size(), k = subspace_base.size(), d = dataset[0].size();
    vector<vector<double>>processed(n, vector<double>(k, 0));
    //convert from original d-dim space to k-dim space.(k <= d)
    for(int i = 0; i < n; i++){
        for(int j = 0; j < k; j++){
            double coordinate = dot_product(dataset[i],subspace_base[j]);
            processed[i][j] = coordinate; //every point in the subspace will have k coordinates
        }
    }
    
    //restore from k-dim to original d-dim and save the result to output_path
    vector<vector<double>>restore(n, vector<double>(d, 0));
    for(int i = 0; i < n; i++){
        for(int j = 0; j < k; j++){
            vector<double>tmp = subspace_base[j];
            double coordinate = processed[i][j];
            scalar_multiply(coordinate, tmp);
            vect_add(restore[i], tmp);
        }
    }

    //convert data in "restore" to png
    save_to_png(output_path, restore);
}