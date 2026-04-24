#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <sstream>

using namespace std;

using Value = variant<int, string>;

struct Variable {
    string type;
    Value value;
};

class ScopeSimulator {
private:
    vector<map<string, Variable>> scopes;
    
    bool isValidVariableName(const string& name) {
        if (name.empty()) return false;
        if (!isalpha(name[0]) && name[0] != '_') return false;
        for (char c : name) {
            if (!isalnum(c) && c != '_') return false;
        }
        return true;
    }
    
    Variable* findVariable(const string& name) {
        for (int i = scopes.size() - 1; i >= 0; i--) {
            auto it = scopes[i].find(name);
            if (it != scopes[i].end()) {
                return &(it->second);
            }
        }
        return nullptr;
    }
    
    vector<string> parseTokens(const string& line) {
        vector<string> tokens;
        size_t i = 0;
        while (i < line.size()) {
            if (line[i] == ' ') {
                i++;
                continue;
            }
            if (line[i] == '"') {
                size_t j = i + 1;
                while (j < line.size() && line[j] != '"') {
                    j++;
                }
                if (j < line.size()) {
                    tokens.push_back(line.substr(i, j - i + 1));
                    i = j + 1;
                } else {
                    i++;
                }
            } else {
                size_t j = i;
                while (j < line.size() && line[j] != ' ') {
                    j++;
                }
                tokens.push_back(line.substr(i, j - i));
                i = j;
            }
        }
        return tokens;
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
        if (!isValidVariableName(name)) {
            return false;
        }
        
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
        if (!isValidVariableName(result) || !isValidVariableName(value1) || !isValidVariableName(value2)) {
            return false;
        }
        
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
        if (!isValidVariableName(name)) {
            return false;
        }
        
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
        if (!isValidVariableName(name)) {
            return false;
        }
        
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
    
    bool processLine(const string& line) {
        vector<string> tokens = parseTokens(line);
        
        if (tokens.empty()) {
            return false;
        }
        
        string command = tokens[0];
        
        if (command == "Indent") {
            indent();
            return true;
        } else if (command == "Dedent") {
            return dedent();
        } else if (command == "Declare") {
            if (tokens.size() < 4) return false;
            return declare(tokens[1], tokens[2], tokens[3]);
        } else if (command == "Add") {
            if (tokens.size() < 4) return false;
            return add(tokens[1], tokens[2], tokens[3]);
        } else if (command == "SelfAdd") {
            if (tokens.size() < 3) return false;
            return selfAdd(tokens[1], tokens[2]);
        } else if (command == "Print") {
            if (tokens.size() < 2) return false;
            return print(tokens[1]);
        }
        
        return false;
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
        
        if (!sim.processLine(line)) {
            cout << "Invalid operation\n";
        }
    }
    
    return 0;
}
