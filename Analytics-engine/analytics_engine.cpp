#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <ctime>
#include "config.h"


using namespace std;


struct StorageEvent {
    string file_id;
    string event_type;
    long timestamp;
    int size_mb;
};




unordered_map<string, int> access_count;
unordered_map<string, int> storage_usage;


void processEvent(const StorageEvent& event) {
    if (event.event_type == "UPLOAD") {
        storage_usage[event.file_id] = event.size_mb;
        access_count[event.file_id] = 1;
    } else if (event.event_type == "READ") {
        access_count[event.file_id]++;
    } else if (event.event_type == "DELETE") {
        storage_usage.erase(event.file_id);
        access_count.erase(event.file_id);
    }
}

void printAnalytics() {
    cout << "\n=== Storage Analytics ===\n";
    for (auto& entry : access_count) {
        string file_id = entry.first;
        int accesses = entry.second;
        string status = accesses >= 3 ? "HOT" : "COLD";

        cout << "File: " << file_id
             << " | Accesses: " << accesses
             << " | Status: " << status << endl;
    }
}
void printRecommendations() {
    cout << "\n=== Optimization Recommendations ===\n";

    for (auto& entry : storage_usage) {
        string file_id = entry.first;
        int size_mb = entry.second;
        int accesses = access_count[file_id];

        if (accesses < 3) {
            double hot_cost = (size_mb / 1024.0) * 0.1;
            double cold_cost = (size_mb / 1024.0) * 0.02;

            cout << "File: " << file_id
                 << " | Action: ARCHIVE"
                 << " | Monthly Savings: $"
                 << (hot_cost - cold_cost) << endl;
        }
    }
}



int main() {
    StorageEvent event;
    loadConfig("E:/Real time Cloud Storage Optimizer/config/config.txt");
 


    while (cin >> event.file_id
               >> event.event_type
               >> event.timestamp
               >> event.size_mb) {
        processEvent(event);
        printAnalytics();
        printRecommendations();
        }
        
    
    

    return 0;
    }

