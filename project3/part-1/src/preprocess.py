import os
import pandas as pd

# This function remove all the records containing '?' (i.e., missing values). Also, remove the attribute "native-country".
# parameter: 
#   filepath->string, the file to be precessed
# return: 
#   None
def preprocess(input_file, output_file):
    if os.path.isfile(output_file):
        os.remove(output_file)

    titles = ['age','workclass','fnlwgt','education','education-num','marital-status','occupation','relationship','race','sex','capital-gain','capital-loss','hours-per-week','native-country','class']

    with open(output_file,"w+") as processed,\
        open(input_file,"r+") as original:

        for ele in titles:
            processed.write(ele)
            if(ele != 'class'):
                processed.write(",")
            else:
                processed.write('\n')

        for line in original: 
            if line == "|1x3 Cross validator\n" or line == "\n":
                continue
            tmp = "" 
            for ele in line:
                if ele == '?':
                    tmp = ""
                    break
                if ele != ' ' and ele != '.':
                    tmp += ele
            processed.write(tmp)

    df = pd.read_csv(output_file)
    df = df.drop("native-country", axis=1)

    df.to_csv(output_file,index=False, header=False)

# preprocess, remove records with '?' and remove attribute "native-country"
training_set = "../adult/adult.data"
training_set_preprocessed = "train.data"

test_set = "../adult/adult.test"
test_set_preprocessed = "test.data"

preprocess(training_set, training_set_preprocessed)
preprocess(test_set, test_set_preprocessed)
