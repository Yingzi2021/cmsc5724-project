import matplotlib.pyplot as plt
import re

def parse_data(filename):
    # Read data from the file
    with open(filename, 'r') as file:
        data = file.read()

    # Split the data into datasets based on the "Dataset loaded" delimiter
    datasets = data.strip().split('Dataset loaded:')
    datasets = [ds for ds in datasets if ds.strip()]  # Remove any empty strings

    all_data = []

    for ds in datasets:
        lines = ds.strip().split('\n')
        header = lines[0].strip()
        # Extract dimension and R for labeling and default gamma_guess
        dimension_match = re.search(r'dimension:\s*(\d+)', header)
        R_match = re.search(r'R:\s*([\d.]+)', header)
        if dimension_match:
            dimension = dimension_match.group(1)
        else:
            dimension = 'Unknown'
        if R_match:
            R = float(R_match.group(1))
        else:
            R = None  # Default to None if R is not found

        dataset_label = f'Dimension {dimension}'

        data_points = []

        current_lambda = None
        last_gamma_guess = None  # Store the gamma_guess before convergence
        data_point = None

        for line in lines[1:]:
            lambda_match = re.match(r'lambda\s*=\s*([\d.]+)', line)
            gamma_guess_match = re.match(r'Reducing gamma_guess to ([\d.]+) and restarting.', line)
            convergence_match = re.match(r'Margin Perceptron converged after', line)
            margin_match = re.match(r'Final margin after convergence:\s*([\d.]+)', line)
            if lambda_match:
                current_lambda = float(lambda_match.group(1))
                last_gamma_guess = None  # Reset for new lambda
                data_point = {'lambda': current_lambda, 'gamma_guess': None, 'final_margin': None}
            elif gamma_guess_match:
                last_gamma_guess = float(gamma_guess_match.group(1))
            elif convergence_match:
                # Set the gamma_guess just before convergence
                if last_gamma_guess is not None:
                    data_point['gamma_guess'] = last_gamma_guess
                else:
                    # If no gamma_guess before convergence, set to R
                    data_point['gamma_guess'] = R
            elif margin_match:
                final_margin = float(margin_match.group(1))
                data_point['final_margin'] = final_margin
                # Append data_point to dataset's data
                data_points.append(data_point)
                # Reset data_point for the next lambda
                data_point = None

        all_data.append({
            'label': dataset_label,
            'R': R,
            'data_points': data_points
        })

    return all_data

def plot_margins(all_data):
    plt.figure(figsize=(20, 12))
    for dataset in all_data:
        label = dataset['label']
        data_points = dataset['data_points']
        lambdas = [dp['lambda'] for dp in data_points]
        margins = [dp['final_margin'] for dp in data_points]
        # Sort the data
        sorted_data = sorted(zip(lambdas, margins), reverse=False)
        lambdas, margins = zip(*sorted_data)
        # Plot the line
        line, = plt.plot(lambdas, margins, marker='o', markersize=4, label=label)
        # Get the color of the current line
        line_color = line.get_color()
        # Find the index of the maximum margin
        max_index = margins.index(max(margins))
        max_lambda = lambdas[max_index]
        max_margin = margins[max_index]
        # Plot the star at the highest point
        plt.plot(max_lambda, max_margin, marker='*', color='red', markersize=12)
        # Draw a horizontal line at the maximum margin
        plt.axhline(y=max_margin, color=line_color, linestyle='--', linewidth=1.5)
        # Annotate the point with the x and y values
        plt.text(max_lambda, max_margin, f'({max_lambda}, {max_margin})', fontsize=10, verticalalignment='bottom')

    plt.xlabel('Lambda')
    plt.ylabel('Final Margin After Convergence')
    plt.title('Lambda vs Final Margin for Multiple Datasets')
    plt.legend()
    plt.grid(True)
    # Invert the x-axis to show lambda from large to small
    plt.gca().invert_xaxis()
    plt.savefig('1.png', dpi=500)
    plt.show()

def plot_dataset_details(all_data):
    num_datasets = len(all_data)
    plt.figure(figsize=(15, 5))
    for idx, dataset in enumerate(all_data):
        label = dataset['label']
        R = dataset['R']
        data_points = dataset['data_points']
        lambdas = [dp['lambda'] for dp in data_points]
        margins = [dp['final_margin'] for dp in data_points]
        gamma_guesses = [dp['gamma_guess'] for dp in data_points]
        gamma_guess_over_lambda = [gg / lmbd if lmbd != 0 else None for gg, lmbd in zip(gamma_guesses, lambdas)]
        # Sort the data
        sorted_data = sorted(zip(lambdas, margins, gamma_guess_over_lambda), reverse=True)
        lambdas, margins, gamma_guess_over_lambda = zip(*sorted_data)
        # Create subplot
        plt.subplot(1, num_datasets, idx+1)
        plt.plot(lambdas, margins, marker='o', label='Final Margin')
        plt.plot(lambdas, gamma_guess_over_lambda, marker='x', label=r'$\gamma_{\text{guess}} / \lambda$')
        plt.xlabel('Lambda')
        plt.ylabel('Value')
        plt.title(f'Lambda vs Value for {label}')
        plt.legend()
        plt.grid(True)
        plt.gca().invert_xaxis()
    plt.tight_layout()
    plt.savefig('2.png', dpi=500)
    plt.show()

# Main execution
if __name__ == "__main__":
    # Parse the data from the file
    all_data = parse_data('./appendix_result.txt')

    # Plot the margins vs lambda for all datasets in one figure
    plot_margins(all_data)

    # Plot the detailed graphs for each dataset
    plot_dataset_details(all_data)
