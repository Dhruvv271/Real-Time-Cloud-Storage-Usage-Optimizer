#include <string>
#pragma once
extern int HOT_THRESHOLD;
extern double HOT_STORAGE_COST;
extern double COLD_STORAGE_COST;
extern int PRINT_EVERY_N_EVENTS;

void loadConfig(const std::string& path);
