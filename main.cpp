#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <variant>

using namespace std;

using Value = variant<int, string>;

struct Variable {
    string type;
    Value value;
};

class ScopeSimulator {
private:
    vector<map<string, Variable>> scopes;
    
    Variable* findVariable(const string& name) {
        for (int i = scopes.size() - 1; i >= 0; i--) {
            auto it = scopes[i].find(name);
            if (it != scopes[i].end()) {
                return &(it->second);
            }
        }
        return nullptr;
    }
    
public:
    ScopeSimulator() {
        scopes.push_back(map<string, Variable>());
    }
    
    void indent() {
        scopes.push_back(map<string, Variable>());
    }
    
    bool dedent() {
        if (scopes.size() <= 1) {
            return false;
        }
        scopes.pop_back();
        return true;
    }
    
    bool declare(const string& type, const string& name, const string& valueStr) {
        if (scopes.back().find(name) != scopes.back().end()) {
            return false;
        }
        
        Variable var;
        var.type = type;
        
        if (type == "int") {
            try {
                var.value = stoi(valueStr);
            } catch (...) {
                return false;
            }
        } else if (type == "string") {
            if (valueStr.size() < 2 || valueStr.front() != '"' || valueStr.back() != '"') {
                return false;
            }
            var.value = valueStr.substr(1, valueStr.size() - 2);
        } else {
            return false;
        }
        
        scopes.back()[name] = var;
        return true;
    }
    
    bool add(const string& result, const string& value1, const string& value2) {
        Variable* resVar = findVariable(result);
        Variable* val1Var = findVariable(value1);
        Variable* val2Var = findVariable(value2);
        
        if (!resVar || !val1Var || !val2Var) {
            return false;
        }
        
        if (resVar->type != val1Var->type || resVar->type != val2Var->type) {
            return false;
        }
        
        if (resVar->type == "int") {
            int v1 = get<int>(val1Var->value);
            int v2 = get<int>(val2Var->value);
            resVar->value = v1 + v2;
        } else {
            string v1 = get<string>(val1Var->value);
            string v2 = get<string>(val2Var->value);
            resVar->value = v1 + v2;
        }
        
        return true;
    }
    
    bool selfAdd(const string& name, const string& valueStr) {
        Variable* var = findVariable(name);
        if (!var) {
            return false;
        }
        
        if (var->type == "int") {
            try {
                int addValue = stoi(valueStr);
                var->value = get<int>(var->value) + addValue;
            } catch (...) {
                return false;
            }
        } else if (var->type == "string") {
            if (valueStr.size() < 2 || valueStr.front() != '"' || valueStr.back() != '"') {
                return false;
            }
            string addValue = valueStr.substr(1, valueStr.size() - 2);
            var->value = get<string>(var->value) + addValue;
        } else {
            return false;
        }
        
        return true;
    }
    
    bool print(const string& name) {
        Variable* var = findVariable(name);
        if (!var) {
            return false;
        }
        
        cout << name << ":";
        if (var->type == "int") {
            cout << get<int>(var->value) << "\n";
        } else {
            cout << "\"" << get<string>(var->value) << "\"\n";
        }
        
        return true;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int n;
    cin >> n;
    cin.ignore();
    
    ScopeSimulator sim;
    
    for (int i = 0; i < n; i++) {
        string line;
        getline(cin, line);
        
        if (line.empty()) {
            i--;
            continue;
        }
        
        size_t pos = line.find(' ');
        string command = (pos == string::npos) ? line : line.substr(0, pos);
        
        bool success = false;
        
        if (command == "Indent") {
            sim.indent();
            success = true;
        } else if (command == "Dedent") {
            success = sim.dedent();
        } else if (command == "Declare") {
            size_t pos1 = line.find(' ');
            size_t pos2 = line.find(' ', pos1 + 1);
            size_t pos3 = line.find(' ', pos2 + 1);
            
            if (pos1 != string::npos && pos2 != string::npos && pos3 != string::npos) {
                string type = line.substr(pos1 + 1, pos2 - pos1 - 1);
                string name = line.substr(pos2 + 1, pos3 - pos2 - 1);
                string value = line.substr(pos3 + 1);
                success = sim.declare(type, name, value);
            }
        } else if (command == "Add") {
            size_t pos1 = line.find(' ');
            size_t pos2 = line.find(' ', pos1 + 1);
            size_t pos3 = line.find(' ', pos2 + 1);
            
            if (pos1 != string::npos && pos2 != string::npos && pos3 != string::npos) {
                string result = line.substr(pos1 + 1, pos2 - pos1 - 1);
                string value1 = line.substr(pos2 + 1, pos3 - pos2 - 1);
                string value2 = line.substr(pos3 + 1);
                success = sim.add(result, value1, value2);
            }
        } else if (command == "SelfAdd") {
            size_t pos1 = line.find(' ');
            size_t pos2 = line.find(' ', pos1 + 1);
            
            if (pos1 != string::npos && pos2 != string::npos) {
                string name = line.substr(pos1 + 1, pos2 - pos1 - 1);
                string value = line.substr(pos2 + 1);
                success = sim.selfAdd(name, value);
            }
        } else if (command == "Print") {
            size_t pos1 = line.find(' ');
            if (pos1 != string::npos) {
                string name = line.substr(pos1 + 1);
                success = sim.print(name);
            }
        }
        
        if (!success) {
            cout << "Invalid operation\n";
        }
    }
    
    return 0;
}
