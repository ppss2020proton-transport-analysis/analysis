# analysis
The code allows optics_PPSS_2020 folder with currently used optics. 
You may download them by this link: 
http://ppss.ifj.edu.pl/~mtrzebin/PPSS_2020_tracks/optics_PPSS_2020/ 

Also, you have to put initial .root file in the code directory. Download link:
http://ppss.ifj.edu.pl/~mtrzebin/PPSS_2020_tracks/pythia8_13TeV_protons_100k.root

Compile & run the whole project by the next terminal command: 
g++ ver1_modified.cpp magnet.cpp shift.cpp distributions_difference.cpp \`root-config --libs --cflags\` -o ver1_modified; ./ver1_modified
