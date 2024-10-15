import math

# convert adult dataset to the structure that is easy for margin perceptron to process.
# input: adult_processed.data (string features already processed to int)
# output: adult.txt(convert label and strip unnecessary ' ')

def process_dataset(input_file, output_file):
    data = []
    max_norm = 0.0

    with open(input_file, 'r') as f:
        for line in f:
            # Strip whitespace and split by comma
            tokens = [token.strip() for token in line.strip().split(',')]
            if not tokens:
                continue  # Skip empty lines

            # Extract features and label
            *features_str, label_str = tokens

            # Convert features to floats
            features = [float(feature) for feature in features_str]

            # Convert label to 1 or -1
            if label_str == '>50K':
                label = 1
            elif label_str == '<=50K':
                label = -1
            else:
                print(f"Unknown label '{label_str}' encountered. Skipping line.")
                continue  # Skip lines with unknown labels

            # Calculate norm of the feature vector
            norm = math.sqrt(sum(f ** 2 for f in features))
            if norm > max_norm:
                max_norm = norm

            # Append processed data
            data.append((features, label))

    # Calculate dimension and number of samples
    if data:
        dimension = len(data[0][0])
        num_samples = len(data)
        R = max_norm
    else:
        print("No data processed.")
        return

    # Write processed data to output file
    with open(output_file, 'w') as f:
        # Write the first line: dimension,num_samples,R
        f.write(f"{dimension},{num_samples},{R}\n")

        # Write each data point
        for features, label in data:
            features_str = ','.join(map(str, features))
            f.write(f"{features_str},{label}\n")

    print(f"Processed dataset saved to '{output_file}'.")

# Example usage
input_file = 'adult_processed.data'    # Replace with your input file path
output_file = '/adult.txt'  # Replace with your desired output file path
process_dataset(input_file, output_file)
