#!/bin/python3
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

def create_heatmap_cache_miss_rate(data, NUM_RUNS):

    # Extract data corresponding to the grid
    algorithms = data['Algorithm'].to_numpy()
    dimensions = data['Dimension'].to_numpy()

    # Extract data corresponding to the heat intensity
    cache_miss_rates = data['Cache-Miss-Rate'].to_numpy()

    # Get unique dimensions
    unique_dimensions = np.unique(dimensions)

    # Get unique algorithms.
    # Using pandas over np to preserve the algorithm order (np.unique() sorts in alphabetical order)
    ordered_algorithms = pd.Categorical(data['Algorithm'], categories=data['Algorithm'].unique(), ordered=True)
    unique_algorithms = ordered_algorithms.categories

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

    # Flip the heatmap on the horizontal axis to have dimenisons grow upwards
    heatmap_array = np.flip(heatmap_array, 0)

    # Set the style and font figure size
    sns.set_theme(style="whitegrid", font_scale=1.1)

    # Need to be lists of strings or sns wont plot
    xtick_labels = list(unique_algorithms)
    # Flip the dimension labels to grow upwards
    ytick_labels = list(map(str, np.flip(unique_dimensions)))

    # Create the heatmap
    plt.figure(figsize=(12, 8))
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


def create_bar_cpi(data, NUM_RUNS):

    # Set the style and font figure size
    sns.set_theme(style="whitegrid", font_scale=1.1)

    # Create the bar plot, grouping by Dimension with color for each Algorithm
    plt.figure(figsize=(12, 8))
    sns.barplot(
        x="Dimension",
        y="Cycles per Instruction (CPI)",
        hue="Algorithm",
        data=data,
        palette="YlGnBu",
    )

    # Add title and labels
    plt.title("Cycles per Instruction (CPI) by Algorithm and Dimension")
    plt.xlabel("Dimension")
    plt.ylabel("Cycles per Instruction (CPI)")

    # Rotate x-axis labels if necessary
    plt.xticks(rotation=0)

    # Adjust the legend to be outside the plot if you need more space
    plt.legend(title="Algorithm", bbox_to_anchor=(1.05, 1), loc='upper left')

    plt.tight_layout()
    # Save
    plt.savefig("cpi_bar.png")


def create_graph_execution_time(data, NUM_RUNS):

    # Extract and convert time to log base 10
    data["Log Execution Time"] = np.log10(data["Average Execution Time (seconds)"])

    # Set the style and font figure size
    sns.set_theme(style="whitegrid", font_scale=1.1)

    # Create plot canvas
    plt.figure(figsize=(12, 8))

    # Custom markers for each algorithm
    markers = {
        "BLAS": "o",
        "NAIVE": "+",
        "SINGLETHREAD": "s",
        "MULTITHREAD": "D",
        "MULTITHREAD_3AVX": "*",
        "MULTITHREAD_9AVX": "^"
    }

    # Plot each algorithm individually with custom line style and marker
    for algorithm, marker in markers.items():
        sns.lineplot(
            data=data[data["Algorithm"] == algorithm],
            x="Dimension",
            y="Log Execution Time",
            label=algorithm,
            linestyle="dashed",
            marker=marker,
            linewidth=3
        )

    # Customize plot layout
    plt.title("Average Execution Time by Dimension and Algorithm")
    plt.xlabel("Dimension")
    plt.ylabel("Average Execution Time (seconds, log 10 scale)")
    plt.legend(title="Algorithm", loc="upper left", fontsize="medium", title_fontsize="large")

    plt.tight_layout()
    plt.savefig("execution_time_plot.png")

def main():

    # The number of runs used for each datapoint (row) in the data during benchmark
    NUM_RUNS = 50;

    # Load data
    data = pd.read_csv('benchmark_results.csv')

    # Create and save heatmap
    create_heatmap_cache_miss_rate(data, NUM_RUNS)

    # Create and save bar diagram for CPI
    create_bar_cpi(data, NUM_RUNS)

    # Create and save graph plot for execution time
    create_graph_execution_time(data, NUM_RUNS)

# Execute main if this file is called as a script
if __name__ == "__main__":
    main()

