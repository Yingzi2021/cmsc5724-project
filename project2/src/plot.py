import matplotlib.pyplot as plt
import re

# Read data from the file
with open('./result.txt', 'r') as file:
    data = file.read()

# Split the data into datasets based on the "Dataset loaded" delimiter
datasets = data.strip().split('Dataset loaded:')
datasets = [ds for ds in datasets if ds.strip()]  # Remove any empty strings

all_data = []

for ds in datasets:
    lines = ds.strip().split('\n')
    header = lines[0].strip()
    # Extract dimension for labeling
    dimension_match = re.search(r'dimension:\s*(\d+)', header)
    if dimension_match:
        dimension = dimension_match.group(1)
    else:
        dimension = 'Unknown'
    
    lambdas = []
    margins = []
    current_lambda = None
    current_margin = None
    
    # Iterate over the lines to extract lambda and margin values
    for line in lines[1:]:
        lambda_match = re.match(r'lambda\s*=\s*([\d.]+)', line)
        margin_match = re.match(r'Final margin after convergence:\s*([\d.]+)', line)
        if lambda_match:
            current_lambda = float(lambda_match.group(1))
            lambdas.append(current_lambda)
        elif margin_match:
            current_margin = float(margin_match.group(1))
            margins.append(current_margin)
    
    # Ensure the number of lambdas and margins match
    if len(lambdas) != len(margins):
        print(f"Warning: Mismatched data in dataset with dimension {dimension}.")
        continue
    
    dataset_label = f'Dimension {dimension}'
    all_data.append({
        'label': dataset_label,
        'lambdas': lambdas,
        'margins': margins
    })

# Plot the data
plt.figure(figsize=(20, 12))

for dataset in all_data:
    lambdas = dataset['lambdas']
    margins = dataset['margins']
    label = dataset['label']
    
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
    
    # Draw a horizontal line to the y-axis
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

# Save the figure as 'fig.png'
plt.savefig('fig.png', dpi=500)
plt.show()