import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from time import sleep

def plot_diffs(filename, shift_axis):
    df = pd.read_csv(filename)
    
    colors = ["cyan", "green", "violet", "blue", "black", "yellow", "grey", "orange"]
    magnets = df["Magnet"].unique()
    
    magnets_to_colors = dict(zip(magnets, colors))

    df_copy = df.copy(deep=True)
    df_copy.drop(df_copy.iloc[:, 0:6], inplace=True, axis=1)
    
    indicies = (df_copy != 0).any()
    y_vals = indicies.index[indicies].tolist()

    y_vals_names = [i for i in y_vals if "Mean" in i]
    y_errs_names = [i for i in y_vals if "RMS" in i]
    
    for i in range(len(y_vals_names)):
        plt.figure(figsize=(16,9))
        for magnet in magnets:
            magnet_df = df.loc[(df["Magnet"] == magnet)]
            plt.errorbar(x=magnet_df[shift_axis],
                         y=magnet_df[y_vals_names[i]], 
                         yerr=magnet_df[y_errs_names[i]], fmt='*', 
                         ecolor='r', color=magnets_to_colors[magnet])
            plt.xlabel(shift_axis)
            plt.ylabel(y_vals_names[i])
        
        plt.legend(labels=magnets, loc="upper center")
        plt.savefig(shift_axis + "_" + y_vals_names[i] + ".png", dpi=300)
        plt.show()

plot_diffs("changes_x_shifting.csv", "x_shift")
plot_diffs("changes_y_shifting.csv", "y_shift")
