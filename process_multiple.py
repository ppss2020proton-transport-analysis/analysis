import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from time import sleep
from os import listdir
from os.path import isfile, join

def plot_diffs(filename):
    df = pd.read_csv(filename)

    num_of_changes = len(df["Magnet"].unique())

    df_copy = df.copy(deep=True)
    df_copy.drop(df_copy.iloc[:, 0:7], inplace=True, axis=1)
    
    # Get columns names with non-zero values
    indicies = (df_copy != 0).any()
    y_names_all = indicies.index[indicies].tolist()

    # Separate those columns names by Mean and RMS
    y_vals_names = [i for i in y_names_all if "Mean" in i]
    y_errs_names = [i for i in y_names_all if "RMS" in i]

    for y_val_name in y_vals_names:
        df_vals = df.loc[np.isnan(df[y_val_name]) == False]

        plt.hist(df_vals[y_val_name], bins=15)
        plt.xlabel(y_val_name)
        plt.ylabel("Probability density")
        plt.savefig("./pics_csv_data/" + filename.split(".")[0] + "_" + y_val_name + ".png")
        plt.show()

plot_diffs("RBEND_variants.csv")
