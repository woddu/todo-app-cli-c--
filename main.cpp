#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

using 
    std::cout,
    std::cin,
    std::unitbuf,
    std::string,
    std::vector,
    std::stringstream;

enum class Command {
    Unknown,
    View,
    Add,
    Edit,
    Remove,
    Complete,
    Exit
};

struct TodoItem final {
    int id;
    string title;
    string description;
    bool completed;

    void markComplete() {
        completed = true;
    }
};

void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if_not(s.begin(), s.end(), [](unsigned char ch) {
        return std::isspace(ch);
    }));
}

void rtrim(std::string &s) {
    s.erase(std::find_if_not(s.rbegin(), s.rend(), [](unsigned char ch) {
        return std::isspace(ch);
    }).base(), s.end());
}

void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

Command parseCommand(const string& input) {
    if (input == "view") return Command::View;
    if (input == "add") return Command::Add;
    if (input == "edit") return Command::Edit;
    if (input == "remove") return Command::Remove;
    if (input == "complete") return Command::Complete;
    if (input == "exit") return Command::Exit;
    return Command::Unknown;
}

vector<string> getInput(string& input){
    cout << "> ";
    getline(cin, input);

    vector<string> tokens;
    stringstream ss(input);
    string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

vector<TodoItem> getTodosFromFile(){
    vector<TodoItem> todos;    
    std::ifstream file("E:\\dump\\c++\\todo-app-cli-c++\\todos.csv");
    if (!file.is_open()) {
        cout << "Could not open the file!\n";
        return todos;
    }
    
    string line;
    string completedStr;
    string idStr;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        TodoItem todo;
        if (std::getline(iss, idStr, ',') &&
            std::getline(iss, todo.title, ',') &&
            std::getline(iss, todo.description, ',') &&
            std::getline(iss, completedStr)) {
            todo.id = std::stoi(idStr);
            if (completedStr == "true" || completedStr == "1") {
                todo.markComplete();
            } else {
                todo.completed = false;
            }
            todos.push_back(todo);
        }
    }
    file.close();
    return todos;
}

bool saveTodosToFile(const vector<TodoItem>& todos) {
    std::ofstream file("E:\\dump\\c++\\todo-app-cli-c++\\todos.csv");
    if (!file.is_open()) {
        cout << "Could not open the file for writing!\n";
        return false;
    }

    for (const auto& todo : todos) {
        file << todo.id << ","
             << todo.title << ","
             << todo.description << ","
             << (todo.completed ? "true" : "false") << "\n";
    }
    file.close();
    return true;
}

void printTodos(const vector<TodoItem>& todos){

    if(todos.size() == 0) {
        cout << "No todos found.\n";
        return;
    }

    int longestTitle = 0;
    int longestDesc = 0;
    for (const auto& todo : todos) {
        longestTitle = std::max(longestTitle, static_cast<int>(todo.title.length()));
        longestDesc = std::max(longestDesc, static_cast<int>(todo.description.length()));
    }

    cout << "\nID  " << "Title" << string(longestTitle - (5 - 3), ' ')<< "Description" << string(longestDesc - (11 - 3), ' ')  << "  Completed\n";
    for (const auto& todo : todos) {
        cout << todo.id << string(4 - static_cast<int>(std::to_string(todo.id).length()), ' ') 
        << todo.title << string(longestTitle - static_cast<int>(todo.title.length()) + 3, ' ') 
        << todo.description << string(longestDesc - static_cast<int>(todo.description.length()) + 3, ' ') 
        << "  " << (todo.completed ? "Yes" : "No") << "\n";
    }
}

bool addTodo(vector<TodoItem>& todos, string& newTitle, string& newDescription) {
    cout << "Enter Title: ";
    while (newTitle.empty()) {
        getline(cin, newTitle);
        trim(newTitle);
    }
    cout << "Enter Description: ";
    while (newDescription.empty()) {
        getline(cin, newDescription);
        trim(newDescription);
    }
    TodoItem newTodo{static_cast<int>(todos.empty() ? 1 : todos.back().id + 1), newTitle, newDescription, false};
    todos.push_back(newTodo);
    return saveTodosToFile(todos);
}

bool removeTodo(vector<TodoItem>& todos, int& id) {
    cout << "Enter the ID of the todo to remove: ";
    while (!(cin >> id)) {
        cin.clear(); // Clear the error flag
        cin.ignore(10000, '\n'); // Discard invalid input
    }
    auto it = std::remove_if(todos.begin(), todos.end(), [id](const TodoItem& todo) {
        return todo.id == id;
    });
    if (it != todos.end()) {
        todos.erase(it, todos.end());
        return saveTodosToFile(todos);
    }
    return false;
}

bool editTodo(vector<TodoItem>& todos, int& id, string& newTitle, string& newDescription) {
    cout << "Enter the ID of the todo to edit: ";
    while (!(cin >> id)) {
        cin.clear(); // Clear the error flag
        cin.ignore(10000, '\n'); // Discard invalid input
    }
    cin.ignore();  // Clear the newline character from the input buffer
    cout << "Enter new Title: ";
    while (newTitle.empty()) {
        getline(cin, newTitle);
        trim(newTitle);
    }
    cout << "Enter new Description: ";
    while (newDescription.empty()) {
        getline(cin, newDescription);
        trim(newDescription);
    }
    for (auto& todo : todos) {
        if (todo.id == id) {
            todo.title = newTitle;
            todo.description = newDescription;
            return saveTodosToFile(todos);
        }
    }
    return false;
}

bool completeTodo(vector<TodoItem>& todos, int& id) {
    cout << "Enter the ID of the todo to complete: ";
    while (!(cin >> id)) {
        cin.clear(); // Clear the error flag
        cin.ignore(10000, '\n'); // Discard invalid input
    }
    for (auto& todo : todos) {
        if (todo.id == id) {
            todo.markComplete();
            return saveTodosToFile(todos);
        }
    }
    return false;
}

int main(){
    auto todos = getTodosFromFile();
    cout << unitbuf;
    cout << "Todo App:\n";
    cout << "Enter a command (view, add, remove, complete, exit)\n";
    string input;
    string newTitle, newDescription;
    int selectedTodoId = -1;
    while (true) {        
        vector<string> tokens = getInput(input);

        if (tokens.empty()) {
            continue;
        }

        switch (parseCommand(tokens[0])) {
            case Command::View:
                printTodos(todos);
                break;
            case Command::Add:                
                if (!addTodo(todos, newTitle, newDescription)) {                
                    cout << "Failed to save todos to file.\n";
                    todos.pop_back();
                }
                newTitle.clear();
                newDescription.clear();
                printTodos(todos);
                break;
            case Command::Edit:                
                if (!editTodo(todos, selectedTodoId, newTitle, newDescription)) {
                    cout << "Failed to edit todo.\n";
                }
                newTitle.clear();
                newDescription.clear();
                printTodos(todos);
                break;
            case Command::Remove:                
                if (removeTodo(todos, selectedTodoId)) {
                    cout << "Todo removed successfully.\n";
                } else {
                    cout << "Failed to remove todo.\n";
                }
                break;
            case Command::Complete:                
                if (completeTodo(todos, selectedTodoId)) {
                    cout << "Todo marked as complete.\n";
                } else {
                    cout << "Failed to mark todo as complete.\n";
                }
                break;
            case Command::Exit:
                return 0;
            case Command::Unknown:
            default:
                cout << "Unknown command\n";
                break;
        }
    }
}