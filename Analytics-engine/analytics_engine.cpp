#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <ctime>
#include <thread>
#include <atomic>
#include <sstream>
#include "config.h"
#include "thread_safe_queue.h"
#include "logger.h"
#include "httplib.h"
#include "json.hpp"
using json = nlohmann::json;


using namespace std;

// --------------------
// Data structures
// --------------------
struct StorageEvent {
    string file_id;
    string event_type;
    long timestamp;
    int size_mb;
};

ThreadSafeQueue<StorageEvent> eventQueue;
atomic<bool> shuttingDown(false);

unordered_map<string, int> access_count;    // file_id -> number of accesses
unordered_map<string, int> storage_usage;   // file_id -> size in MB
mutex dataMutex;                             // protects access_count & storage_usage

// --------------------
// Event processing
// --------------------
void processEvent(const StorageEvent& event) {
    lock_guard<mutex> lock(dataMutex);

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
    lock_guard<mutex> lock(dataMutex);
    cout << "\n=== Storage Analytics ===\n";
    for (auto& entry : access_count) {
        string file_id = entry.first;
        int accesses = entry.second;
        string status = accesses >= HOT_THRESHOLD ? "HOT" : "COLD";
        cout << "File: " << file_id
             << " | Accesses: " << accesses
             << " | Status: " << status << endl;
    }
}

void printRecommendations() {
    lock_guard<mutex> lock(dataMutex);
    cout << "\n=== Optimization Recommendations ===\n";
    for (auto& entry : storage_usage) {
        string file_id = entry.first;
        int size_mb = entry.second;
        int accesses = access_count[file_id];
        if (accesses < HOT_THRESHOLD) {
            double hot_cost = (size_mb / 1024.0) * HOT_STORAGE_COST;
            double cold_cost = (size_mb / 1024.0) * COLD_STORAGE_COST;
            cout << "File: " << file_id
                 << " | Action: ARCHIVE"
                 << " | Monthly Savings: $"
                 << (hot_cost - cold_cost) << endl;
        }
    }
}

// --------------------
// Worker threads
// --------------------
void readerThread() {
    StorageEvent event;
    while (cin >> event.file_id >> event.event_type >> event.timestamp >> event.size_mb) {
        eventQueue.push(event);
    }
    shuttingDown = true;
    eventQueue.close();
}

void workerThread() {
    int count = 0;
    StorageEvent event;
    while (eventQueue.pop(event)) {
        processEvent(event);
        count++;
        if (count % PRINT_EVERY_N_EVENTS == 0) {
            printAnalytics();
            printRecommendations();
        }
    }
    Logger::instance().log(INFO, "Worker thread exiting — no more events.");
}

// --------------------
// Main function
// --------------------
int main() {
    // Load configuration
    try {
        loadConfig("../config/config.txt");
    } catch (const std::exception& ex) {
        Logger::instance().log(LOG_ERROR, ex.what());
        return 1;
    }

    Logger::instance().setLevel(INFO);
    Logger::instance().enableFile("analytics.log");
    Logger::instance().log(INFO, "Analytics Engine started");

    // Start worker threads
    thread reader(readerThread);
    thread worker(workerThread);

    // --------------------
    // HTTP API Server
    // --------------------
    httplib::Server svr;

    // Simple health check
    svr.Get("/hello", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content("Server is running!", "text/plain");
    });

    // Return analytics summary
   svr.Get("/analytics", [](const httplib::Request&, httplib::Response& res) {
    lock_guard<mutex> lock(dataMutex);
    json j;
    for (auto& entry : access_count) {
        string file_id = entry.first;
        int accesses = entry.second;
        string status = accesses >= HOT_THRESHOLD ? "HOT" : "COLD";
        j[file_id] = { {"accesses", accesses}, {"status", status} };
    }
    res.set_header("Access-Control-Allow-Origin", "*"); // allow frontend requests
    res.set_content(j.dump(), "application/json");
});
    // Return optimization recommendations
    svr.Get("/recommendations", [](const httplib::Request&, httplib::Response& res) {
    lock_guard<mutex> lock(dataMutex);
    json j = json::array();
    for (auto& entry : storage_usage) {
        string file_id = entry.first;
        int accesses = access_count[file_id];
        if (accesses < HOT_THRESHOLD) {
            j.push_back({{"file_id", file_id}, {"action", "ARCHIVE"}});
        }
    }
    res.set_content(j.dump(), "application/json");
});


    
    
    svr.Get("/shutdown", [&](const httplib::Request&, httplib::Response& res){
    res.set_content("Shutting down", "text/plain");
    shuttingDown = true;
    eventQueue.close();
    svr.stop();
});


  
// In your main() after creating svr:
svr.Post("/event", [](const httplib::Request& req, httplib::Response& res) {
      res.set_header("Access-Control-Allow-Origin", "*"); 
    try {
        // 1. Check Content-Type
        if (req.get_header_value("Content-Type") != "application/json") {
            res.status = 400;
            res.set_content(R"({"error":"Content-Type must be application/json"})", "application/json");
            return;
        }

        // 2. Parse JSON
        auto j = json::parse(req.body);

        // 3. Validate required fields
        if (!j.contains("file_id") || !j.contains("event_type") ||
            !j.contains("timestamp") || !j.contains("size_mb")) {
            res.status = 400;
            res.set_content(R"({"error":"Missing required fields"})", "application/json");
            return;
        }

        // 4. Fill StorageEvent and push to queue
        StorageEvent event;
        event.file_id = j.at("file_id").get<std::string>();
        event.event_type = j.at("event_type").get<std::string>();
        event.timestamp = j.at("timestamp").get<long>();
        event.size_mb = j.at("size_mb").get<int>();

        eventQueue.push(event);

        Logger::instance().log(INFO, "Received event: " + event.file_id + " / " + event.event_type);

        // 5. Respond OK
        res.set_content(R"({"status":"ok"})", "application/json");
    } 
    catch (const json::parse_error& e) {
        // Invalid JSON
        res.status = 400;
        res.set_content(R"({"error":"Invalid JSON"})", "application/json");
    }
    catch (const std::exception& e) {
        // Other errors
        res.status = 500;
        res.set_content(std::string("{\"error\":\"") + e.what() + "\"}", "application/json");
    }
});
// Allow all origins (for development)
svr.Options(".*", [](const httplib::Request&, httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
    res.status = 200;
});






    cout << "HTTP server running at http://localhost:8080\n";
    svr.listen("0.0.0.0", 8080);

    // Wait for threads to finish
    reader.join();
    worker.join();

    Logger::instance().log(INFO, "Analytics Engine shutting down.");
    return 0;
}
