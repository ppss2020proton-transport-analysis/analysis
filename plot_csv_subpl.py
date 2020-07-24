import matplotlib.pyplot as plt
import numpy as np
import math as m
import pandas as pd 

def csv(file,x_argument,x_label,x_scale):
	df=pd.read_csv(file)

	v_k=["VKICKER(1)","VKICKER(2)","VKICKER(3)","VKICKER(4)","VKICKER(5)"]
	h_k=["HKICKER(1)","HKICKER(2)","HKICKER(3)","HKICKER(4)","HKICKER(5)"]
	dip=["RBEND(1)","RBEND(2)","RBEND(3)","RBEND(4)","RBEND(5)","RBEND(6)","RBEND(7)"]
	quad=["QUADRUPOLE(1)","QUADRUPOLE(2)","QUADRUPOLE(3)","QUADRUPOLE(4)","QUADRUPOLE(5)","QUADRUPOLE(6)","QUADRUPOLE(7)"]

	mag_table=[v_k,h_k,dip,quad]
	arg=x_argument

	color_iter=0

	colors = ["cyan", "green", "violet", "blue", "black", "orange", "grey", "yellow"]
	markers = ["o","v","s","P","D","d","*","X"]

	xlabel=x_label
	ylabel_sh="Difference in position [mm]"
	ylabel_ang="Difference in angle [urad]"



	for j in mag_table:
		fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2,constrained_layout=True,figsize=(16,9))
		
		color_iter=0
		for i in j:
			df_cut=df.loc[df["Magnet"] == i] 
			
			ax1.errorbar(x_scale*df_cut[arg], 1000*df_cut["Mean(d_x)"], yerr=1000*df_cut["RMS(d_x)"],fmt=markers[color_iter],capsize=5,ecolor=colors[color_iter],color=colors[color_iter])
			ax1.grid(True)
			ax1.set_title("Mean(d_x)")
			ax1.set_xlabel(xlabel)
			ax1.set_ylabel(ylabel_sh)

			#plt.title()
			ax1.legend(j,loc="best")
			ax2.errorbar(x_scale*df_cut[arg], 1000*df_cut["Mean(d_y)"], yerr=1000*df_cut["RMS(d_y)"],fmt=markers[color_iter],capsize=5,ecolor=colors[color_iter],color=colors[color_iter])
			ax2.grid(True)
			ax2.set_title("Mean(d_y)")
			ax2.set_xlabel(xlabel)
			ax2.set_ylabel(ylabel_sh)



			ax3.errorbar(x_scale*df_cut[arg], df_cut["Mean(d_sx)"], yerr=df_cut["RMS(d_sx)"],fmt=markers[color_iter],capsize=5,ecolor=colors[color_iter],color=colors[color_iter])
			ax3.grid(True)
			ax3.set_title("Mean(d_sx)")
			ax3.set_xlabel(xlabel)
			ax3.set_ylabel(ylabel_ang)


			ax4.errorbar(x_scale*df_cut[arg], df_cut["Mean(d_sy)"], yerr=df_cut["RMS(d_sy)"],fmt=markers[color_iter],capsize=5,ecolor=colors[color_iter],color=colors[color_iter])
			ax4.grid(True)		
			ax4.set_title("Mean(d_sy)")
			ax4.set_xlabel(xlabel)
			ax4.set_ylabel(ylabel_ang)



			color_iter+=1

		

		fig.suptitle(arg+ " " +j[0][:-3]+"s", fontsize=16)
		
		plt.savefig(x_argument+"_"+j[0],dpi=150)
		plt.clf()

csv("z_shift.csv","z_shift[m]","Shift in z [mm]",1000)
csv("str_shift.csv","Strength_ratio","Magnet strength [%]",100)