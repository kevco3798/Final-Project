C++ Irrigation System

Project Overview
This project uses C++ to determine an irrigation system designed to optimize water usage for various crops based on simulated sensor data (in the future I would add an API for the weather systems associated with the target city). The system utilizes several data structures, including a Binary Search Tree (BST), Max Heap, and the one not from our homework: the k-d Tree.  The k-d tree was used to manage, query, and simulate sensor data in surrounding/neighboring zones as if there were different zones to a garden bed or farm field. 

Key Data Structures
SensorData Struct that represents the moisture, temperature, and precipitation for a day, along with the k-d Tree coordinates
BST : Stores daily sensor data, enabling quick search operations by day.
Methods: insert(int day, SensorData data), SensorData* search(int day)
Max Heap : Organizes sensor data based on moisture levels, enabling efficient retrieval of the highest moisture data.
Methods: insert(SensorData data), SensorData extractMax()
k-d Tree : A binary tree for organizing points in a k-dimensional space, here used for spatial queries based on x and y coordinates 

Menu Options
1) Simulate Sensor Data to generate and stores sensor data for a specified number of days. The data is inserted into the BST, Max Heap, and k-d Tree 
2) View Sensor Data Displays the sensor data for each day that was just simulated 
3) Calculates and displays an irrigation schedule based on the crop type and sensor data 
4) Customize Crops allows users to configure custom moisture and temperature thresholds for different crops if they want unique traits 
5) Query k-d Tree to Nearest Sensor: Queries the k-d Tree to find the sensor data nearest to the given x and y coordinates, simulating a garden bed's varying moisture levels 
6) Exit: self explanatory 

Still have a lot to do to make this project complete but the foundation is there and Im happy with the k-d tree being a different data structure that we didnt use in our homeworks.  Lots to be done for this to be useful for next years garden! 