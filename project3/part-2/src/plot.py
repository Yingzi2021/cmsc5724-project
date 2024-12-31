import matplotlib.pyplot as plt
import pandas as pd


def plot_clustering_result(file_path):
    # 从文件中读取数据
    data = []
    with open(file_path, "r") as file:
        for line in file:
            x, y, cluster, distance = map(float, line.split())
            data.append((int(x), int(y), int(cluster), distance))

    # 转换为DataFrame
    df = pd.DataFrame(data, columns=["x", "y", "cluster", "distance"])

    # 自定义离散颜色（15种颜色）
    custom_colors = [
        "#1f77b4",  # 蓝色
        "#ff7f0e",  # 橙色
        "#2ca02c",  # 绿色
        "#d62728",  # 红色
        "#9467bd",  # 紫色
        "#8c564b",  # 棕色
        "#e377c2",  # 粉色
        "#7f7f7f",  # 灰色
        "#bcbd22",  # 黄绿色
        "#17becf",  # 青色
        "#ff9896",  # 浅红色
        "#98df8a",  # 浅绿色
        "#c5b0d5",  # 浅紫色
        "#c49c94",  # 浅棕色
        "#aec7e8"   # 浅蓝色
    ]

    # 颜色映射（按 cluster 大小顺序排序）
    clusters = sorted(df["cluster"].unique())  # 按 cluster 排序
    num_clusters = len(clusters)
    color_map = {cluster: custom_colors[i % len(custom_colors)] for i, cluster in enumerate(clusters)}

    # 绘图
    plt.figure(figsize=(10, 8))
    for cluster in clusters:
        subset = df[df["cluster"] == cluster]
        centroids = subset[subset["distance"] == 0]
        others = subset[subset["distance"] != 0]

        # 绘制非centroid点
        plt.scatter(others["x"], others["y"], label=f"Cluster {cluster}", s=50, color=color_map[cluster])
        # 绘制centroid点
        plt.scatter(centroids["x"], centroids["y"], s=200, color=color_map[cluster], marker="*", edgecolors="black", linewidth=1.5)

    plt.xlabel("X")
    plt.ylabel("Y")
    plt.title(f"{file_path.split('-')[0]} clustering result - {num_clusters}")
    plt.legend()
    plt.grid(True)

    # 保存为PNG文件
    output_file = f"./result/{file_path.split('-')[0]}/{file_path.split('-')[0]}_clustering_{num_clusters}_clusters.png"
    plt.savefig(output_file, format="png")
    plt.show()


plot_clustering_result("kmeans-report.txt")