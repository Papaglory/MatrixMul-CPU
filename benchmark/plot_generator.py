#!/bin/python3
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

def create_heatmap_cache_miss_rate(data):

    # Extract data corresponding to the grid
    algorithms = data['Algorithm'].to_numpy()
    dimensions = data['Dimension'].to_numpy()

    # Extract data corresponding to the heat intensity
    cache_miss_rates = data['Cache-Miss-Rate'].to_numpy()

    # Get unique algorithms and dimensions
    unique_algorithms = np.unique(algorithms)
    unique_dimensions = np.unique(dimensions)

    # Initialize heatmap with NaN values
    heatmap_array = np.full((len(unique_dimensions), len(unique_algorithms)), np.nan)

    # Create mappings from algorithm and dimension to indices
    algorithm_to_index = {alg: idx for idx, alg in enumerate(unique_algorithms)}
    dimension_to_index = {dim: idx for idx, dim in enumerate(unique_dimensions)}

    # Fill heatmap
    for alg, dim, rate in zip(algorithms, dimensions, cache_miss_rates):
        row_idx = dimension_to_index[dim]
        col_idx = algorithm_to_index[alg]
        heatmap_array[row_idx, col_idx] = rate

    # Set the color
    sns.set_theme(style="whitegrid")

    # Need to be lists of strings or sns wont plot
    xtick_labels = list(unique_algorithms)
    ytick_labels = list(map(str, unique_dimensions))

    # Create the heatmap
    plt.figure(figsize=(8, 6))
    sns.heatmap(
        heatmap_array,
        annot=True,
        fmt=".5f",
        cmap="YlGnBu",
        xticklabels=xtick_labels,
        yticklabels=ytick_labels
    )

    # Add title and labels
    plt.title("Cache-Miss Rate Heatmap")
    plt.ylabel("Dimension")
    plt.xlabel("Algorithm")

    plt.tight_layout()
    # Save
    plt.savefig("cache_miss_heatmap.png")

def create_bar_cpi(data):

    # Extract data for x-axis
    algorithms = data["Algorithm"].to_numpy()
    dimensions = data["Dimension"].to_numpy()

    # Extract data for y-axis
    cpi = data["Cycles per Instruction (CPI)"].to_numpy()

    # Set the color
    sns.set_theme(style="whitegrid")

    # Unwrap algorithm and dimension to a x-axis labels
    x_labels = [f"{alg} (Dim {dim})" for alg, dim in zip(algorithms, dimensions)]

    # Create the bar plot
    plt.figure(figsize=(8, 6))
    sns.barplot(
        x=x_labels,
        y=cpi,
        hue=x_labels,
        palette="YlGnBu",
        dodge=False,
        legend=False
    )

    # Add title and labels
    plt.title("Cycles per Instruction (CPI) by Algorithm and Dimension")
    plt.xlabel("Algorithm (Dimension)")
    plt.ylabel("Cycles per Instruction (CPI)")

    plt.tight_layout()
    # Save
    plt.savefig("cpi_bar.png")


def create_graph_execution_time(data):

    # Extract and convert time to log base 10
    data["Log Execution Time"] = np.log10(data["Average Execution Time (seconds)"])

    # Set the color
    sns.set_theme(style="whitegrid")

    # Create the bar plot
    plt.figure(figsize=(8, 6))

    sns.lineplot(
        data=data,
        x="Dimension",
        y="Average Execution Time (seconds)",
        # Different color for each Algorithm
        hue="Algorithm",
        marker="o",
        palette="YlGnBu"
    )

    # Add title and labels
    plt.title("Average Execution Time by Dimension and Algorithm")
    plt.xlabel("Dimension")
    plt.ylabel("Average Execution Time (seconds)")

    plt.tight_layout()
    plt.savefig("execution_time_plot.png")

def main():

    # Load data
    data = pd.read_csv('benchmark_results.csv')

    # Create and save heatmap
    create_heatmap_cache_miss_rate(data)

    # Create and save bar diagram for CPI
    create_bar_cpi(data)

    # Create and save graph plot for execution time
    create_graph_execution_time(data)

# Execute main if this file is called as a script
if __name__ == "__main__":
    main()

