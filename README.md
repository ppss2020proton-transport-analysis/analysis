# analysis
The code allows optics\_PPSS\_2020 folder with currently used optics. 
You may download them by this link: 
http://ppss.ifj.edu.pl/~mtrzebin/PPSS\_2020\_tracks/optics\_PPSS\_2020/ 

Also, you have to put initial .root file in the code directory. Download link:
http://ppss.ifj.edu.pl/~mtrzebin/PPSS\_2020\_tracks/pythia8\_13TeV\_protons\_100k.root

Compile & run the whole project by the next terminal command: 
g++ ver1\_modified.cpp magnet.cpp shift.cpp distributions\_difference.cpp \`root-config --libs --cflags\` -o ver1\_modified; ./ver1\_modified
