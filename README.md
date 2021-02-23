# data_sub
Different version are for different type of data processing. 
The newest version (v5) subscribes to a topic called "data_pub" which has the floor inclination data on different coordinates. 
It takes the data and subsequently group with its coordinates and orientation and saves it in a csv file in the root directory.

To run it, simply type "rosrun data_sub data_sub_v5" in command line.
