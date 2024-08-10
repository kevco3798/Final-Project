#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <unordered_map>
#include <numeric>
#include <cmath>

struct SensorData {
    float moisture;
    float temperature;
    float precipitation;
    float point[2]; // Coordinates x and y for the k-d tree 
};

//future usage would #include an api for denvers weather system but I didnt have the time to implement 
//function to generate simulated sensor data
std::vector<SensorData> generate_sensor_data(int num_days) {
    std::vector<SensorData> sensor_data;
    std::default_random_engine generator;

    //avg and stddev. for Colorado weather systems 
    std::normal_distribution<float> moisture_dist(0.30, 0.10);
    std::normal_distribution<float> temperature_dist(20.0, 5.0);
    std::normal_distribution<float> precipitation_dist(1.5, 1.0);

    for (int i = 0; i < num_days; ++i) 
    {
        //moisture must be between 0 and 1, 
        //temperature is within realistic range
        float moisture      = std::min(1.0f, std::max(0.0f, moisture_dist(generator)));
        float temperature   = std::min(40.0f, std::max(-10.0f, temperature_dist(generator)));
        float precipitation = std::max(0.0f, precipitation_dist(generator));

        SensorData data = {moisture, temperature, precipitation};
        sensor_data.push_back(data);
    }
    return sensor_data;
}

//basic tree node for BST from the hws 
struct TreeNode {
    int day;
    SensorData data;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int d, SensorData sd) : day(d), data(sd), left(nullptr), right(nullptr) {}
};

//BST -- lots of stuff here optimized with help from stackoverflow and Khan academy 
class BST {
public:
    BST() : root(nullptr) {}
    void insert(int day, SensorData data) { root = insertRec(root, day, data); }
    SensorData* search(int day) { return searchRec(root, day); }

private:
    TreeNode* root;

    TreeNode* insertRec(TreeNode* node, int day, SensorData data) 
    {
        if (node == nullptr) return new TreeNode(day, data);
        if (day < node->day) node->left = insertRec(node->left, day, data);
        else if (day > node->day) node->right = insertRec(node->right, day, data);
        return node;
    }

    SensorData* searchRec(TreeNode* node, int day) 
    {
        if (node == nullptr || node->day == day) return (node == nullptr) ? nullptr : &node->data;
        if (day < node->day) return searchRec(node->left, day);
        return searchRec(node->right, day);
    }
};
//help from zybooks -- https://learn.zybooks.com/zybook/COLORADOCSPB2270Summer2024/chapter/13/section/1?content_resource_id=61871850
class MaxHeap {
public:
    void insert(SensorData data) 
    {
        heap.push_back(data);
        heapifyUp(heap.size() - 1);
    }
    SensorData extractMax() 
    {
        if (heap.size() == 0) return {0, 0, 0};
        SensorData max = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        heapifyDown(0);
        return max;
    }

private:
    std::vector<SensorData> heap;

    void heapifyUp(int index) 
    {
        while (index > 0 && heap[parent(index)].moisture < heap[index].moisture) 
        {
            std::swap(heap[parent(index)], heap[index]);
            index = parent(index);
        }
    }

    void heapifyDown(int index) 
    {
        int left = leftChild(index);
        int right = rightChild(index);
        int largest = index;

        if (left < heap.size() && heap[left].moisture > heap[largest].moisture) largest = left;
        if (right < heap.size() && heap[right].moisture > heap[largest].moisture) largest = right;

        if (largest != index) 
        {
            std::swap(heap[index], heap[largest]);
            heapifyDown(largest);
        }
    }

    int parent(int index) { return (index - 1) / 2; }
    int leftChild(int index) { return 2 * index + 1; }
    int rightChild(int index) { return 2 * index + 2; }
};

//still messing around with this so not adding it 
// class Graph {
// public:
//     void addEdge(int u, int v) { adj[u].push_back(v); }
//     void display() 
//     {
//         for (auto& pair : adj) 
//         {
//             std::cout << "Zone " << pair.first << " is connected to zones: ";
//             for (int v : pair.second) std::cout << v << " ";
//             std::cout << std::endl;
//         }
//     }

//     // Implement flow simulation or water retention analysis here
//     void simulateWaterFlow(int startZone) 
//     {
//         std::cout << "Simulating water flow from zone " << startZone << "...\n";
//         // Implement the flow simulation logic
//         // For simplicity, we simulate by traversing the graph from the start zone
//         std::unordered_map<int, bool> visited;
//         simulateFlowRec(startZone, visited);
//     }

// private:
//     std::unordered_map<int, std::vector<int>> adj;

//     void simulateFlowRec(int zone, std::unordered_map<int, bool>& visited) 
//     {
//         visited[zone] = true;
//         std::cout << "Watering zone " << zone << std::endl;
//         for (int neighbor : adj[zone]) 
//         {
//             if (!visited[neighbor]) 
//             {
//                 simulateFlowRec(neighbor, visited);
//             }
//         }
//     }
// };

// KDTree Node
//wiki and https://www.geeksforgeeks.org/search-and-insertion-in-k-dimensional-tree/
//used KD tree to conceptualize a garden bed with different levels/coordinates of varying moisture saturation 
struct KDTreeNode {
    SensorData data;
    float point[2]; //x and y coordinates
    KDTreeNode* left;
    KDTreeNode* right;

    KDTreeNode(SensorData d, float x, float y) : data(d), left(nullptr), right(nullptr) 
    {
        point[0] = x;
        point[1] = y;
    }
};

class KDTree {
public:
    KDTree() : root(nullptr) {}

    void insert(SensorData data, float x, float y) 
    {
        root = insertRec(root, data, x, y, 0);
    }

    SensorData* nearestNeighbor(float x, float y) 
    {
        return nearestRec(root, x, y, 0);
    }

private:
    KDTreeNode* root;

    KDTreeNode* insertRec(KDTreeNode* node, SensorData data, float x, float y, int depth) 
    {
        if (node == nullptr) return new KDTreeNode(data, x, y);

        int cd = depth % 2; // cd- current dimension (0 for x, 1 for y)
        if (x < node->point[cd]) node->left = insertRec(node->left, data, x, y, depth + 1);
        else node->right = insertRec(node->right, data, x, y, depth + 1);

        return node;
    }

    SensorData* nearestRec(KDTreeNode* node, float x, float y, int depth) 
    {
        if (node == nullptr) return nullptr;

        int cd = depth % 2;
        KDTreeNode* next_branch = nullptr;
        KDTreeNode* other_branch = nullptr;

        if (x < node->point[cd]) 
        {
            next_branch = node->left;
            other_branch = node->right;
        } else {
            next_branch = node->right;
            other_branch = node->left;
        }

        SensorData* best = nearestRec(next_branch, x, y, depth + 1);
        if (best == nullptr) best = &node->data;

        float dist_best = distance(x, y, best->point[0], best->point[1]);
        float dist_node = distance(x, y, node->point[0], node->point[1]);

        if (dist_node < dist_best) best = &node->data;

        float dist_plane = std::abs(x - node->point[cd]);
        if (dist_plane < dist_best) 
        {
            SensorData* possible_best = nearestRec(other_branch, x, y, depth + 1);
            if (possible_best != nullptr) 
            {
                float dist_possible_best = distance(x, y, possible_best->point[0], possible_best->point[1]);
                if (dist_possible_best < dist_best) best = possible_best;
            }
        }

        return best;
    }

    float distance(float x1, float y1, float x2, float y2) {
        return std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
    }
};

//helper function to validate integer input within a specific range
//stack + google help here
int getValidatedIntInput(int min, int max) {
    int input;
    while (true) 
    {
        std::cin >> input;
        if (std::cin.fail() || input < min || input > max) 
        {
            std::cin.clear(); // Clear the error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
            std::cout << "Invalid -- must enter a number between " << min << " and " << max << ": ";
        } else {
            return input;
        }
    }
}
// helper function to validate correct crop input (small list til the project gets bigger - 4/5 crops)
std::string getValidatedCropInput() {
    std::string crop;
    while (true) 
    {
        std::cin >> crop;
        if (crop == "peppers" || crop == "tomatoes" || crop == "cucumbers" || crop == "peaches" || crop == "cherries") 
        {
            return crop;
        } else {
            std::cout << "Invalid crop -- Enter 1 of the following: peppers, tomatoes, cucumbers, peaches, cherries: ";
        }
    }
}
//set the irrigation schedule based on certain crops and their moisture/temp threshold according to csu crop guide 
//https://growgive.extension.colostate.edu/wp-content/uploads/sites/63/2021/01/Colorado-Vegetable-Guide-2.1.pdf
std::vector<std::string> determine_irrigation_schedule(const std::vector<SensorData>& data, const std::string& crop) {
    std::vector<std::string> schedule;
    float moisture_threshold;
    float temperature_threshold;

    //thresholds based on crop type
    //important*** -- temp is in C not F 
    if (crop == "peppers") {
        moisture_threshold = 0.5; 
        temperature_threshold = 25.0;  
    } else if (crop == "tomatoes") {
        moisture_threshold = 0.5; 
        temperature_threshold = 24.0;  
    } else if (crop == "cucumbers") {
        moisture_threshold = 0.6;  
        temperature_threshold = 22.0;  
    } else if (crop == "peaches" || crop == "cherries") {
        moisture_threshold = 0.4;  
        temperature_threshold = 20.0;  
    } else {
        moisture_threshold = 0.5;  
        temperature_threshold = 22.0;  
    }

    //change irrigation schedule based on previous days moisture levels
    for (size_t i = 0; i < data.size(); ++i) 
    {
        float avg_moisture = data[i].moisture;
        if (i > 0) avg_moisture = (avg_moisture + data[i-1].moisture) / 2.0;
        if (i > 1) avg_moisture = (avg_moisture + data[i-2].moisture) / 3.0;

        if (avg_moisture < moisture_threshold && data[i].temperature > temperature_threshold) 
        {
            schedule.push_back("Day " + std::to_string(i + 1) + ": Irrigation needed for " + crop + ".");
        } else {
            schedule.push_back("Day " + std::to_string(i + 1) + ": No irrigation needed for " + crop + ".");
        }
    }
    return schedule;
}

// print the irrigation schedule after simulation + determine are both ran 
void determineAndPrintIrrigationSchedule(const std::vector<SensorData>& sensorData) {
    std::string crop;
    std::cout << "Enter crop type (peppers, tomatoes, cucumbers, peaches, cherries): ";
    crop = getValidatedCropInput();
    std::vector<std::string> schedule = determine_irrigation_schedule(sensorData, crop);
    for (const auto& entry : schedule) 
    {
        std::cout << entry << std::endl;
    }
}

// Function to display the menu
void displayMenu() {
    std::cout << "Irrigation Interface: Select 1-7 \n";
    std::cout << "1) Simulate Sensor Data \n";
    std::cout << "2) View Sensor Data \n";
    std::cout << "3) Show Irrigation Schedule \n";
    std::cout << "4) Customize Crops \n";
    std::cout << "5) Query k-d Tree to Nearest Sensor \n";
    std::cout << "6) Exit \n";

    //graphing not done yet 
    //std::cout << "_) Simulate Water Flow \n";

}

// Function to view sensor data
void viewSensorData(const std::vector<SensorData>& sensorData) {
    for (int i = 0; i < sensorData.size(); ++i) 
    {
        std::cout << "Day " << i + 1 << ": Moisture: " << sensorData[i].moisture
                  << ", Temperature: " << sensorData[i].temperature
                  << ", Precipitation: " << sensorData[i].precipitation << std::endl;
    }
}

//configure crops if the user wants unique customization 
void customizeCrops() {
    std::string crop;
    float moisture_threshold;
    float temperature_threshold;

    std::cout << "Enter crop: ";
    std::cin >> crop;
    std::cout << "Enter moisture threshold (0-1): ";
    std::cin >> moisture_threshold;
    std::cout << "Enter temperature threshold (in Celcius): ";
    std::cin >> temperature_threshold;

    //store this configuration in a map or database for future use
    std::cout << "Configuration saved for crop: " << crop << "\n";
}

//command line interface function to slide into main 
void interface() {
    BST bst;
    MaxHeap maxHeap;
    std::vector<SensorData> sensorData;
    KDTree kdTree; // KD-Tree for spatial queries

    //still need to mess with this 
    //Graph graph;

    int choice;

    while (true) {
        displayMenu();
        choice = getValidatedIntInput(1, 7);

        // Clear the input buffer to remove any leftover characters
        //segmentation fault if you dont have this 
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 1) {
            int num_days;
            std::cout << "Enter number of days: ";
            //limit num of days to 1yr range 
            num_days = getValidatedIntInput(1, 365); 
            sensorData = generate_sensor_data(num_days);
            for (int i = 0; i < num_days; ++i) {
                float x = static_cast<float>(rand() % 100); // Random x-coordinate for this example
                float y = static_cast<float>(rand() % 100); // Random y-coordinate for this example
                bst.insert(i + 1, sensorData[i]);
                maxHeap.insert(sensorData[i]);
                kdTree.insert(sensorData[i], x, y);
            }
            //debugging
            //std::cout << "Sensor data: .\n";
        } else if (choice == 2) {
            if (sensorData.empty()) {
                std::cout << "No sensor data simulated .\n";
            } else {
                viewSensorData(sensorData);
            }
        } else if (choice == 3) {
            if (sensorData.empty()) {
                std::cout << "No sensor data simualted .\n";
            } else {
                determineAndPrintIrrigationSchedule(sensorData);
            }
        } else if (choice == 4) {
            customizeCrops();
        } else if (choice == 5) {
            if (sensorData.empty()) {
                std::cout << "No sensor data available.\n";
            } else {
                float x, y;
                std::cout << "Enter x-coordinate (0-100): ";
                std::cin >> x;
                std::cout << "Enter y-coordinate (0-100): ";
                std::cin >> y;
                SensorData* nearest = kdTree.nearestNeighbor(x, y);
                if (nearest) {
                    std::cout << "Nearest data -> Moisture: " << nearest->moisture 
                              << ", Temperature: " << nearest->temperature 
                              << ", Precipitation: " << nearest->precipitation << std::endl;
                } else {
                    std::cout << "No data found near given coordinates.\n";
                }
            }
        } else if (choice == 6) {
            break;
        }
    }
}

int main() {
    interface();
    return 0;
}