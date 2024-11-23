#!/bin/python3
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from scipy import stats # Perform stat analysis

def create_heatmap_with(x_data, y_data, heat_data, title, x_label, y_label, png_name):

    # Convert data to numpy arrays for filling heatmap
    x_data_arr = x_data.to_numpy()
    y_data_arr = y_data.to_numpy()
    heat_data_arr = heat_data.to_numpy()

    # Get unique x_data categories in preserved order (i.e. np.unique() sorts
    # names in alphabetical order)
    ordered_x_data = pd.Categorical(x_data, categories=x_data.unique(), ordered=True)
    unique_x_data = ordered_x_data.categories

    # Get unique y_data categories
    ordered_y_data = pd.Categorical(y_data, categories=y_data.unique(), ordered=True)
    unique_y_data = ordered_y_data.categories

    # Initialize heatmap with NaN values
    heatmap_array = np.full((len(unique_y_data), len(unique_x_data)), np.nan)

    # Create mappings from x_data and y_data to indices
    algorithm_to_index = {alg: idx for idx, alg in enumerate(unique_x_data)}
    dimension_to_index = {dim: idx for idx, dim in enumerate(unique_y_data)}

    # Fill heatmap
    for alg, dim, rate in zip(x_data_arr, y_data_arr, heat_data_arr):
        row_idx = dimension_to_index[dim]
        col_idx = algorithm_to_index[alg]
        heatmap_array[row_idx, col_idx] = rate

    # Flip the heatmap on the horizontal axis to have dimenisons grow upwards
    heatmap_array = np.flip(heatmap_array, 0)

    # Set the style and font figure size
    sns.set_theme(style="whitegrid", font_scale=1.1)

    # Need to be lists of strings or sns wont plot
    xtick_labels = list(unique_x_data)
    # Flip the dimension labels to grow upwards
    ytick_labels = list(map(str, np.flip(unique_y_data)))

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
    plt.title(title)
    plt.xlabel(x_label)
    plt.ylabel(y_label)

    plt.tight_layout()
    # Save
    plt.savefig(png_name)

def create_bar_cpi(data, NUM_RUNS):

    # Preprocessing for calculating the confidence interval
    alpha = 1 - 0.05
    df = NUM_RUNS - 1

    t_star = stats.t.ppf(1 - alpha / 2, df=df)

    # Calculate the standard error (SE) for each data point
    data['CPI SE'] = np.sqrt(data['CPI Variance'] / NUM_RUNS)

    # Calculate the margin of error (ME)
    data['CPI ME'] = t_star * data['CPI SE']

    # Set the style and font figure size
    sns.set_theme(style="whitegrid", font_scale=1.1)

    # Make CPI values log 10 scale
    #data["Log CPI"] = np.log10(data["Cycles per Instruction (CPI)"])

    # Create the bar plot, grouping by Dimension with color for each Algorithm
    plt.figure(figsize=(12, 8))
    ax = sns.barplot(
        x="Dimension",
        y="Cycles per Instruction (CPI)",
        hue="Algorithm",
        data=data,
        palette="YlGnBu",
        # Confidence interval bar settings
        capsize=4.6,
        err_kws={'linewidth': 2.5},
        errorbar=None
    )

    for bar_group, alg_data in zip(ax.containers, data.groupby("Algorithm")):
            _, group_data = alg_data
            for bar, (_, row) in zip(bar_group, group_data.iterrows()):
                # Get the x position and height of the bar
                x = bar.get_x() + bar.get_width() / 2.0
                height = bar.get_height()
                error = row['CPI ME']
                # Add the error bars
                ax.errorbar(x, height, yerr=error, capsize=5, color='black', fmt='none', lw=1.5)


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
    plt.savefig("plots/cpi_bar.png")


def create_graph_execution_time(data, NUM_RUNS, filename):

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
    plt.savefig(filename)

def main():

    # The number of runs used for each datapoint (row) in the data during benchmark
    NUM_RUNS = 50;

    # Load benchmark data
    data = pd.read_csv('data/benchmark_results.csv')

    # Load block size data of state-of-the-art algorithm: MULTITHREAD_9AVX
    block_size_data = pd.read_csv('data/block_size_results.csv')

    # Create cache-miss-rate heatmap
    create_heatmap_with(data['Algorithm'], data['Dimension'], data['Cache-Miss-Rate'],
                        "Cache-Miss-Rate", "Algorithm", "Dimension", "plots/cache_miss_heatmap.png")

    create_heatmap_with(data['Algorithm'], data['Dimension'], data['Cache-Miss-Rate Variance'],
                        "Cache-Miss-Rate", "Algorithm", "Dimension", "plots/cache_miss_heatmap.png")


    # Create average execution time (seconds) heatmap
    create_heatmap_with(data['Algorithm'], data['Dimension'], data['Average Execution Time (seconds)'],
                        "Average Execution Time (seconds)", "Algorithm", "Dimension", "plots/execution_time_heatmap.png")

    # Create block size and execution time heatmap
    create_heatmap_with(block_size_data['Block Size'], block_size_data['Dimension'], block_size_data['Average Execution Time (seconds)'],
                        "Average Execution Time (seconds)", "Block Size", "Dimension", "plots/block_size_heatmap.png")

    # Create and save bar diagram for CPI
    create_bar_cpi(data, NUM_RUNS)

    # Create and save graph plot for execution time
    create_graph_execution_time(data, NUM_RUNS, "plots/execution_time_plot.png")

    # Filter such that we have selected algorithms
    excluded_algos = ["NAIVE", "SINGLETHREAD", "MULTITHREAD", "BLAS"]
    filtered_data = data.copy()
    for algo in excluded_algos:
        filtered_data = filtered_data[filtered_data['Algorithm'] != algo]

    # Create and save graph plot for execution time
    create_graph_execution_time(filtered_data, NUM_RUNS, "plots/execution_time_plot_TODO.png")

# Execute main if this file is called as a script
if __name__ == "__main__":
    main()

