#!/bin/python3
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import math
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

def create_bar_cpi(data, NUM_RUNS, png_name):

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
    plt.savefig(png_name)


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

# Two sided welchs t test function
def welchs_t_test_two_sided(xbar, ybar, s1, s2, n, m, delta0=0):

    # Calculate the numerator of the t-statistic
    num = xbar - ybar - delta0

    # Calculate the denominator of the t-statistic
    den = math.sqrt(s1**2 / n + s2**2 / m)

    # Calculate the t-statistic
    t_stat = num / den

    # Calculate degrees of freedom
    df_num = (s1**2 / n + s2**2 / m)**2
    df_den = ((s1**2 / n)**2) / (n - 1) + ((s2**2 / m)**2) / (m - 1)
    df = df_num / df_den

    # Calculate the two-tailed p-value
    p_value = 2 * stats.t.sf(abs(t_stat), df)

    return t_stat, p_value

def perform_runtime_ttest_two_sided(
    data,
    group_col,            # Column name for the grouping variable ('Block Size')
    group_values,          # Two values of the grouping variable to compare ([64, 128])
    factor_col,            # Column name of the factor you iterate over ('Dimension')
    metric_col,            # Column name for the metric ('Average Execution Time (seconds)')
    variance_col,          # Column name for the variance of the metric ('Execution Time Variance')
    num_runs,              # Number of runs for each group (NUM_RUNS)
    png_file,              # Filename for the plot
    results_name,          # Filename for the CSV results
    alpha=0.05,            # Significance level
    reject_label_template="Reject H0: {better_group} is better",  # Template for rejection label
    fail_reject_label="Fail to Reject H0",
    plot_title="Two-Sided Log-scaled P-values for {metric} across {factor}",
    plot_xlabel="{factor}",
    plot_ylabel="-log10(P-value)"
):
    # Extract data subsets for the two groups
    data_g1 = data[data[group_col] == group_values[0]]
    data_g2 = data[data[group_col] == group_values[1]]

    # Get all factor values
    factors = data[factor_col].unique()

    # Calculate Bonferroni correction for alpha
    num_tests = len(factors)
    adjusted_alpha = alpha / num_tests

    results = []

    for f in factors:
        # Filter data for the current factor level
        data_g1_filtered = data_g1.loc[data_g1[factor_col] == f]
        data_g2_filtered = data_g2.loc[data_g2[factor_col] == f]

        # Ensure there is data for both groups for this factor
        if data_g1_filtered.empty or data_g2_filtered.empty:
            continue

        # Extract the means and variances
        xbar = data_g1_filtered[metric_col].iloc[0]
        ybar = data_g2_filtered[metric_col].iloc[0]
        s1_squared = data_g1_filtered[variance_col].iloc[0]
        s2_squared = data_g2_filtered[variance_col].iloc[0]
        s1 = np.sqrt(s1_squared)
        s2 = np.sqrt(s2_squared)
        n = num_runs
        m = num_runs

        # Perform two-sided Welchs t-test
        t_stat, p_value = welchs_t_test_two_sided(xbar, ybar, s1, s2, n, m)

        # Determine decision
        if p_value < adjusted_alpha:
            # Identify which group is better (lower metric means better)
            better_group_value = group_values[0] if xbar < ybar else group_values[1]
            decision = reject_label_template.format(better_group=better_group_value)
        else:
            decision = fail_reject_label

        # Store results
        results.append({
            factor_col: f,
            f"{group_values[0]} Mean": xbar,
            f"{group_values[1]} Mean": ybar,
            't-statistic': t_stat,
            'p-value': p_value,
            'Adjusted Alpha': adjusted_alpha,
            'Decision': decision
        })

    # Convert results to DataFrame
    results_df = pd.DataFrame(results)

    # Display the results
    print(results_df)

    # Save the results to a CSV file
    results_df.to_csv(results_name, index=False)

    # Plotting p-values across factors using seaborn
    plt.figure(figsize=(12, 8))
    sns.barplot(data=results_df, x=factor_col, y=-np.log10(results_df['p-value']), hue='Decision', dodge=False)
    plt.axhline(y=-np.log10(adjusted_alpha), color='red', linestyle='--', label=f'Adjusted Alpha = {adjusted_alpha:.5f}')
    plt.title(plot_title.format(metric=metric_col, factor=factor_col))
    plt.xlabel(plot_xlabel.format(metric=metric_col, factor=factor_col))
    plt.ylabel(plot_ylabel.format(metric=metric_col, factor=factor_col))
    plt.legend()
    plt.xticks(rotation=90)
    plt.tight_layout()
    plt.savefig(png_file)

def main():

    # The number of runs used for each datapoint (row) in the data during benchmark
    NUM_RUNS = 50;

    # ----- 3AVX: FIND OPTIMAL BLOCK SIZE----
    block_size_data_3avx = pd.read_csv('data/MULTITHREAD_3AVX_block_size_results.csv')

    # Create block size and execution time heatmap
    create_heatmap_with(block_size_data_3avx['Block Size'], block_size_data_3avx['Dimension'], block_size_data_3avx['Average Execution Time (seconds)'],
                        "Average Execution Time (seconds)", "Block Size", "Dimension", "plots/MULTITHREAD_3AVX_block_size_heatmap.png")

    # Perform welchs test for 3AVX
    #perform_runtime_ttest_two_sided(block_size_data_3avx, NUM_RUNS, "plots/MULTITHREAD_3AVX_block_size_welch.png", "data/MULTITHREAD_3AVX_runtime_t_test_two_sided_results.csv", alpha=0.05)
    perform_runtime_ttest_two_sided(
        data=block_size_data_3avx,
        group_col='Block Size',
        group_values=[64, 128],
        factor_col='Dimension',
        metric_col='Average Execution Time (seconds)',
        variance_col='Execution Time Variance',
        num_runs=NUM_RUNS,
        png_file="plots/MULTITHREAD_3AVX_block_size_welch.png",
        results_name="data/MULTITHREAD_3AVX_runtime_t_test_two_sided_results.csv",
        alpha=0.05,
        reject_label_template="Reject H0: {better_group} block size is better",
        fail_reject_label="Fail to Reject H0",
        plot_title="Two-Sided Log-scaled P-values for {metric} across {factor}",
        plot_xlabel="{factor}",
        plot_ylabel="-log10(P-value)"
    )

    # ----- 9AVX: FIND OPTIMAL BLOCK SIZE----
    block_size_data_9avx = pd.read_csv('data/MULTITHREAD_9AVX_block_size_results.csv')

    # Create block size and execution time heatmap
    create_heatmap_with(block_size_data_9avx['Block Size'], block_size_data_9avx['Dimension'], block_size_data_9avx['Average Execution Time (seconds)'],
                        "Average Execution Time (seconds)", "Block Size", "Dimension", "plots/MULTITHREAD_9AVX_block_size_heatmap.png")

    # Perform welchs test for 9AVX
    perform_runtime_ttest_two_sided(
        data=block_size_data_9avx,
        group_col='Block Size',
        group_values=[64, 128],
        factor_col='Dimension',
        metric_col='Average Execution Time (seconds)',
        variance_col='Execution Time Variance',
        num_runs=NUM_RUNS,
        png_file="plots/MULTITHREAD_9AVX_block_size_welch.png",
        results_name="data/MULTITHREAD_9AVX_runtime_t_test_two_sided_results.csv",
        alpha=0.05,
        reject_label_template="Reject H0: {better_group} block size is better",
        fail_reject_label="Fail to Reject H0",
        plot_title="Two-Sided Log-scaled P-values for {metric} across {factor}",
        plot_xlabel="{factor}",
        plot_ylabel="-log10(P-value)"
    )

    # ----- ALL ALGORITHMS: CREATE PLOTS FOR BENCHMARK RESULTS
    # Load benchmark data
    data = pd.read_csv('data/benchmark_results.csv')

    # Perform test between competing algorithms
    perform_runtime_ttest_two_sided(
        data=data,
        group_col='Algorithm',
        group_values=['MULTITHREAD_3AVX', 'MULTITHREAD_9AVX'],
        factor_col='Dimension',
        metric_col='Average Execution Time (seconds)',
        variance_col='Execution Time Variance',
        num_runs=NUM_RUNS,
        png_file="plots/competing_algorithms_welch.png",
        results_name="data/competing_algorithms_runtime_t_test_two_sided_results.csv",
        alpha=0.05,
        reject_label_template="Reject H0: {better_group} algorithm is better",
        fail_reject_label="Fail to Reject H0",
        plot_title="Two-Sided Log-scaled P-values for {metric} across {factor}",
        plot_xlabel="{factor}",
        plot_ylabel="-log10(P-value)"
    )

    # Create cache-miss-rate heatmap
    create_heatmap_with(data['Algorithm'], data['Dimension'], data['Cache-Miss-Rate'],
                        "Cache-Miss-Rate", "Algorithm", "Dimension", "plots/cache_miss_rate_heatmap.png")

    create_heatmap_with(data['Algorithm'], data['Dimension'], data['Cache-Miss-Rate Variance'],
                        "Cache-Miss-Rate", "Algorithm", "Dimension", "plots/cache_miss_rate_variance_heatmap.png")

    # Create average execution time (seconds) heatmap
    create_heatmap_with(data['Algorithm'], data['Dimension'], data['Average Execution Time (seconds)'],
                        "Average Execution Time (seconds)", "Algorithm", "Dimension", "plots/execution_time_heatmap.png")

    # Create and save bar diagram for CPI
    create_bar_cpi(data, NUM_RUNS, "plots/cpi_bar.png")

    # Create and save graph plot for execution time
    create_graph_execution_time(data, NUM_RUNS, "plots/execution_time_plot.png")

    # Filter such that we have selected algorithms
    excluded_algos = ["NAIVE", "SINGLETHREAD", "MULTITHREAD", "BLAS"]
    filtered_data = data.copy()
    for algo in excluded_algos:
        filtered_data = filtered_data[filtered_data['Algorithm'] != algo]

    # Create average execution time (seconds) heatmap for competing algorithms
    create_heatmap_with(filtered_data['Algorithm'], filtered_data['Dimension'], filtered_data['Average Execution Time (seconds)'],
                        "Average Execution Time (seconds)", "Algorithm", "Dimension", "plots/competing_algorithms_execution_time_heatmap.png")

# Execute main if this file is called as a script
if __name__ == "__main__":
    main()

