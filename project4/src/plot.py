import matplotlib.pyplot as plt
import pandas as pd

def plot_dbscan_cluster(input_path, output_path):
    # Step 1: Load the data from the file
    data = pd.read_csv(input_path, delim_whitespace=True, header=None, names=["x", "y", "cluster_id"])

    # Step 2: Prepare the data for visualization
    noise_color = 'gray'  # Color for noise points
    unique_clusters = data['cluster_id'].unique()
    colors = plt.cm.get_cmap('tab10', len(unique_clusters) - 1)

    # Step 3: Plot the data
    plt.figure(figsize=(8, 6))
    for cluster_id in unique_clusters:
        cluster_data = data[data['cluster_id'] == cluster_id]
        if cluster_id == -2:  # Noise points
            plt.scatter(cluster_data['x'], cluster_data['y'], label='Noise', c=noise_color, alpha=0.6)
        else:  # Other clusters
            plt.scatter(cluster_data['x'], cluster_data['y'], label=f'Cluster {cluster_id}', alpha=0.6)

    plt.xlabel("X")
    plt.ylabel("Y")
    plt.title("DBSCAN Clustering Visualization")
    plt.legend()
    plt.grid(True)
    
    plt.savefig(output_path, format="png")
    plt.show()

input_path = "report.txt"
output_path = "cluser.png"
plot_dbscan_cluster(input_path, output_path)