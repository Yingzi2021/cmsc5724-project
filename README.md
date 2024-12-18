# CMSC5724 projects

## Project 1: Decision Tree(√)

### Goal

Implement Hung's algorithm for decision tree classification

### Dataset

We will use the Adult dataset whose description is available [here](http://archive.ics.uci.edu/ml/datasets/Adult). The training set (adult.data) and evaluation set (adult.test) can be downloaded [here](https://archive.ics.uci.edu/static/public/2/adult.zip).

### Preprocessing

Remove all the records containing '?' (i.e., missing values). Also, remove the attribute "native-country".

## Project 2: Margin Perceptron(√)

### Goal

Implement the margin perceptron algorithm.

### Dataset

Your implementation should work on any dataset in the following format:
The first line contains three numbers n, d, and r, where n is the number of points, d is the dimensionality of the instance space, and r is the radius.
The i-th line (where i goes from 2 to n + 1) gives the (i - 1)-th point in the dataset as:
x1,x2,...,xd,label
where the first d values are the coordinates of the point, and label = 1 or -1.
We have prepared three datasets for you

## Project 3: Bayes Classifier, K-Center, K-Means

This project has two parts.

### PART I(√)

#### Goal

Implement the Bayes Classifier.

#### Dataset, Preprocessing

Same as Project #1.

### PART II

#### Goal

Implement the k-means algorithm using the k-center algorithm for center initialization.

#### Dataset

Download [here](https://www.cse.cuhk.edu.hk/~taoyf/course/cmsc5724/data/8gau.txthttps://www.cse.cuhk.edu.hk/~taoyf/course/cmsc5724/data/8gau.txt) (obtained from the data collection [here](http://cs.joensuu.fi/sipu/datasets/)). Each line has the following format:

x y

which represent the x- and y-coordinates of a point.

#### Task

Partition the dataset into 8 clusters.
