#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <limits>
#include <unistd.h>

using namespace std;

struct Employee
{
        string name;
        int experience;
        int salary;
};

struct Node
{
    Employee data;
    Node* left;
    Node* right;
};

struct KDTree
{
    Node* root;
    int k;
};

Node* newNode(Employee data)
{
    Node* node = new Node;
    node->data = data;
    node->left = node->right = nullptr;
    return node;
}

Node* buildKDTree(vector<Employee>& employees, int depth, int k)
{
        if (employees.empty())
        {
                return nullptr;
        }

        int axis = depth % k;

        if (axis == 0)
        {
                sort(employees.begin(), employees.end(), [](const Employee& a, const Employee& b) {
                return a.experience < b.experience;
        });
    } else {
        sort(employees.begin(), employees.end(), [](const Employee& a, const Employee& b) {
            return a.salary < b.salary;
        });
    }

    int medianIndex = employees.size() / 2;
    Node* node = newNode(employees[medianIndex]);

    vector<Employee> leftEmployees(employees.begin(), employees.begin() + medianIndex);
    vector<Employee> rightEmployees(employees.begin() + medianIndex + 1, employees.end());

    node->left = buildKDTree(leftEmployees, depth + 1, k);
    node->right = buildKDTree(rightEmployees, depth + 1, k);

    return node;
}

Node* insert(Node* root, Employee data, int depth, int k)
{
        if (root == nullptr)
        {
                return newNode(data);
        }

        int axis = depth % k;

        if (axis == 0)
        {
                if (data.experience < root->data.experience)
                {
                        root->left = insert(root->left, data, depth + 1, k);
                }
                else
                {
                        root->right = insert(root->right, data, depth + 1, k);
                }
        }
        else
        {
                if (data.salary < root->data.salary)
                {
                        root->left = insert(root->left, data, depth + 1, k);
                }
                else
                {
                        root->right = insert(root->right, data, depth + 1, k);
                }
        }

        return root;
}

Node* minValueNode(Node* node, int depth, int k)
{
        int axis = depth % k;

        if (axis == 0 && node->left != nullptr)
        {
                return minValueNode(node->left, depth + 1, k);
        }
        else if (axis == 1 && node->left != nullptr)
        {
                return minValueNode(node->left, depth + 1, k);
        }

        return node;
}

void rangeSearch(Node* root, Employee low, Employee high, vector<Employee>& result, int depth, int k)
{
        if (root == nullptr)
        {
                return;
        }

        if (low.experience <= root->data.experience && root->data.experience <= high.experience &&
        low.salary <= root->data.salary && root->data.salary <= high.salary)
        {
                result.push_back(root->data);
        }

        int axis = depth % k;

        if (axis == 0)
        {
                if (low.experience <= root->data.experience)
                {
                        rangeSearch(root->left, low, high, result, depth + 1, k);
                }
                if (high.experience >= root->data.experience)
                {
                        rangeSearch(root->right, low, high, result, depth + 1, k);
                }
        }
        else
        {
                if (low.salary <= root->data.salary)
                {
                        rangeSearch(root->left, low, high, result, depth + 1, k);
                }
                if (high.salary >= root->data.salary)
                {
                        rangeSearch(root->right, low, high, result, depth + 1, k);
                }
        }
}

void findNearestNeighbor(Node* root, Employee target, Employee& nearest, double& minDistance, int depth, int k)
{
        if (root == nullptr)
        {
                return;
        }

        double distance = 0;
        for (int i = 0; i < k; ++i)
        {
                if (i == 0)
                {
                        distance += pow(root->data.experience - target.experience, 2);
                }
                else
                {
                        distance += pow(root->data.salary - target.salary, 2);
                }
        }

        if (distance < minDistance)
        {
                nearest = root->data;
                minDistance = distance;
        }

        int axis = depth % k;

        if (axis == 0)
        {
                if (target.experience < root->data.experience)
                {
                        findNearestNeighbor(root->left, target, nearest, minDistance, depth + 1, k);
                }
                else
                {
                        findNearestNeighbor(root->right, target, nearest, minDistance, depth + 1, k);
                }
        }
        else
        {
                if (target.salary < root->data.salary)
                {
                        findNearestNeighbor(root->left, target, nearest, minDistance, depth + 1, k);
                }
                else
                {
                        findNearestNeighbor(root->right, target, nearest, minDistance, depth + 1, k);
                }
        }
}

double calculateTotalSalary(Node* root, int& totalSalary, int& numEmployees)
{
        if (root == nullptr)
        {
                return 0.0;
        }

        double leftSum = calculateTotalSalary(root->left, totalSalary, numEmployees);
        double rightSum = calculateTotalSalary(root->right, totalSalary, numEmployees);

        totalSalary += root->data.salary;
        numEmployees++;

        return (leftSum + rightSum + root->data.salary);
}

void inOrderTraversal(Node* root, vector<int>& salaries)
{
        if (root == nullptr)
        {
                return;
        }
        inOrderTraversal(root->left, salaries);
        salaries.push_back(root->data.salary);
        inOrderTraversal(root->right, salaries);
}

double calculateAverageSalary(Node* root, int numEmployees)
{
        vector<int> salaries;
        inOrderTraversal(root, salaries);

        if (numEmployees % 2 == 1)
        {
                return static_cast<double>(salaries[numEmployees / 2]);
        }
        else
        {
                return static_cast<double>((salaries[numEmployees / 2 - 1] + salaries[numEmployees / 2]) / 2);
        }
}

double calculateStandardDeviation(Node* root, double mean, int numEmployees)
{
        vector<int> salaries;
        inOrderTraversal(root, salaries);

        double sumOfSquares = 0.0;
        for (int salary : salaries)
        {
                sumOfSquares += pow(static_cast<double>(salary) - mean, 2);
        }

        return sqrt(sumOfSquares / numEmployees);
}

void printKDTree(Node* root, int depth)
{
        if (root == nullptr)
        {
                return;
        }

        printKDTree(root->left, depth + 1);

        cout << "Depth " << depth << ": ";
        cout << "Name: " << root->data.name << ", Experience: " << root->data.experience << ", Salary: " << root->data.salary << endl;

        printKDTree(root->right, depth + 1);
}

Node* deleteNode(Node* root, string name, int depth, int k)
{
        if (root == nullptr)
        {
                return root;
        }

        int axis = depth % k;

        if (name < root->data.name)
        {
                root->left = deleteNode(root->left, name, depth + 1, k);
        }
        else if (name > root->data.name)
        {
                root->right = deleteNode(root->right, name, depth + 1, k);
        }
        else
        {
                if (root->left == nullptr)
                {
                        Node* temp = root->right;
                        delete root;
                        return temp;
                }
                else if (root->right == nullptr)
                {
                        Node* temp = root->left;
                        delete root;
                        return temp;
                }

                Node* temp = minValueNode(root->right, depth, k);
                root->data = temp->data;
                root->right = deleteNode(root->right, temp->data.name, depth + 1, k);
        }
        return root;
}

void saveDataToFile(const vector<Employee>& employees, const string& filename)
{
    ofstream outFile(filename);
    if (outFile.is_open()) {
        for (const Employee& emp : employees) {
            outFile << emp.name << "," << emp.experience << "," << emp.salary << endl;
        }
        outFile.close();
        cout << "Employee data saved to " << filename << endl;
    } else {
        cout << "Unable to open the file for writing." << endl;
    }
}

vector<Employee> loadDataFromFile(const string& filename) {
    vector<Employee> employees;
    ifstream inFile(filename);
    if (inFile.is_open()) {
        string line;
        while (getline(inFile, line)) {
            size_t pos1 = line.find(",");
            size_t pos2 = line.find(",", pos1 + 1);
            if (pos1 != string::npos && pos2 != string::npos) {
                string name = line.substr(0, pos1);
                int experience = stoi(line.substr(pos1 + 1, pos2 - pos1 - 1));
                int salary = stoi(line.substr(pos2 + 1));
                employees.push_back({name, experience, salary});
            }
        }
        inFile.close();
        cout << "Employee data loaded from " << filename << endl;
    } else {
        cout << "Unable to open the file for reading. Using an empty employee list." << endl;
    }
    return employees;
}

int main()
{
        vector<Employee> employees;
        KDTree kdTree;
        kdTree.root = nullptr;
        kdTree.k = 2;

        int choice;
        int totalSalary = 0;
        int numEmployees = 0;
        double averageSalary = 0.0;
        double medianSalary = 0.0;
        double standardDeviation = 0.0;

        do
        {
                system("cls");
                cout << "\nCOMPANY EMPLOYEE DETAILS:\n\n\n";
                cout << "1. Search by Experience and Salary Range\n";
                cout << "2. Insert an Employee\n";
                cout << "3. Delete an Employee\n";
                cout << "4. Find Nearest Neighbor\n";
                cout << "5. Print KD-Tree Structure\n";
                cout << "6. Calculate Total Salary\n";
                cout << "7. Calculate Average Salary\n";
                cout << "8. Calculate Standard Deviation of Salaries\n";
                cout << "9. Save Employee Data to File\n";
                cout << "10. Load Employee Data from File\n";
                cout << "11. Exit\n\n\n";
                cout << "Enter your choice> ";
                cin >> choice;
                usleep(3000);
                system("cls");
                if (choice == 1)
                {
                        Employee low, high;
                        cout << "Enter low experience and salary: ";
                        cin >> low.experience >> low.salary;
                        cout << "Enter high experience and salary: ";
                        cin >> high.experience >> high.salary;

                         vector<Employee> result;
                        rangeSearch(kdTree.root, low, high, result, 0, kdTree.k);

                        cout << "Employees with experience between " << low.experience << " and " << high.experience<< " and salary between " << low.salary << " and " << high.salary << " (inclusive):" << endl;
                        for (const Employee& emp : result) {
                        cout << "Name: " << emp.name << ", Experience: " << emp.experience << ", Salary: " << emp.salary << endl;
                        }
                        usleep(3000);
                        system("cls");
                } 
                else if (choice == 2) {
                    Employee emp;
                    cout << "Enter employee name: ";
                    cin.ignore();
                    getline(cin, emp.name);
                    cout << "Enter employee experience: ";
                    cin >> emp.experience;
                    cout << "Enter employee salary: ";
                    cin >> emp.salary;

                    employees.push_back(emp);
                    kdTree.root = insert(kdTree.root, emp, 0, kdTree.k);
                    cout << "Employee added successfully." << endl;
                    usleep(4000);
                    system("cls");
                } else if (choice == 3) {
                string nameToDelete;
                cout << "Enter the name of the employee to delete: ";
                cin.ignore();
                getline(cin, nameToDelete);

                auto empIt = remove_if(employees.begin(), employees.end(), [&nameToDelete](const Employee& emp) {
                return emp.name == nameToDelete;
                
            });

            if (empIt != employees.end()) {
                employees.erase(empIt, employees.end());
                kdTree.root = deleteNode(kdTree.root, nameToDelete, 0, kdTree.k);
                cout << "Employee deleted successfully." << endl;
            } else {
                cout << "Employee not found." << endl;
            }
            usleep(4000);
            system("cls");
        } else if (choice == 4) {
            Employee target;
            cout << "Enter target employee name: ";
            cin.ignore();
            getline(cin, target.name);
            cout << "Enter target employee experience: ";
            cin >> target.experience;
            cout << "Enter target employee salary: ";
            cin >> target.salary;

            Employee nearest;
            double minDistance = numeric_limits<double>::max();
            findNearestNeighbor(kdTree.root, target, nearest, minDistance, 0, kdTree.k);

            cout << "Nearest neighbor to " << target.name << " (Experience: " << target.experience << ", Salary: " << target.salary << "):" << endl;
            cout << "Name: " << nearest.name << ", Experience: " << nearest.experience << ", Salary: " << nearest.salary << endl;
            usleep(4000);
            system("cls");
        } else if (choice == 5) {
            cout << "KD-Tree Structure:" << endl;
            printKDTree(kdTree.root, 0);
            usleep(4000);
            system("cls");
        } else if (choice == 6) {
            averageSalary = calculateTotalSalary(kdTree.root, totalSalary, numEmployees);
            cout << "Total salary of all employees: " << averageSalary << endl;
            usleep(4000);
            system("cls");
        } else if (choice == 7) {
            medianSalary = calculateAverageSalary(kdTree.root, numEmployees);
            cout << "Average salary of all employees: " << medianSalary << endl;
            usleep(4000);
            system("cls");
        } else if (choice == 8) {
            standardDeviation = calculateStandardDeviation(kdTree.root, averageSalary, numEmployees);
            cout << "Standard deviation of salaries: " << standardDeviation << endl;
            usleep(4000);
            system("cls");
        } else if (choice == 9) {
            string filename;
            cout << "Enter filename to save employee data: ";
            cin.ignore();
            getline(cin, filename);
            saveDataToFile(employees, filename);
            usleep(4000);
            system("cls");
        } else if (choice == 10) {
            string filename;
            cout << "Enter filename to load employee data: ";
            cin.ignore();
            getline(cin, filename);
            employees = loadDataFromFile(filename);
            kdTree.root = buildKDTree(employees, 0, kdTree.k);
            usleep(4000);
            system("cls");
        } else if (choice == 11) {
            cout << "Exiting the program. Goodbye!" << endl;
        } else {
            cout << "Invalid choice. Please enter a valid option." << endl;
        }

    } while (choice != 11);

    return 0;
}