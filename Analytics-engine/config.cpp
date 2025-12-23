#include "config.h"
#include <fstream>
#include <string>

int HOT_THRESHOLD;
double HOT_STORAGE_COST;
double COLD_STORAGE_COST;
int PRINT_EVERY_N_EVENTS;

void loadConfig(const std::string& path) {
    std::ifstream file(path);
    std::string line;

    while (getline(file, line)) {
        auto pos = line.find('=');
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        if (key == "HOT_THRESHOLD") HOT_THRESHOLD = stoi(value);
        else if (key == "HOT_STORAGE_COST") HOT_STORAGE_COST = stod(value);
        else if (key == "COLD_STORAGE_COST") COLD_STORAGE_COST = stod(value);
        else if (key == "PRINT_EVERY_N_EVENTS") PRINT_EVERY_N_EVENTS = stoi(value);
    }
}
