#include "analyzer.h"
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <sstream>

using namespace std;

// Students may use ANY data structure internally
namespace {
    unordered_map<string, long long> zoneCountsMap;
    unordered_map<string, long long> slotCountsMap;
}

void TripAnalyzer::ingestFile(const std::string& csvPath) {
    ifstream file(csvPath);
    if (!file) {
        return; // cannot open file
    }

    string line;
    getline(file, line); // skip header

    while (getline(file, line)) {
        stringstream ssline(line);
        string cell;
        vector<string> cols;

        while (getline(ssline, cell, ',')) {
            cols.push_back(cell);
        }

        string pickupZoneID;
        string dateAndTime;

        if (cols.size() == 6) {
            pickupZoneID = cols[1];
            dateAndTime = cols[3];
        }
        else if (cols.size() == 3) {
            pickupZoneID = cols[1];
            dateAndTime = cols[2];
        }
        else {
            continue; // malformed row
        }

        if (dateAndTime.size() < 13) {
            continue;
        }

        int hour;
        try {
            hour = stoi(dateAndTime.substr(11, 2));
        }
        catch (...) {
            continue;
        }

        if (hour < 0 || hour > 23) {
            continue;
        }

        zoneCountsMap[pickupZoneID]++;

        string slotKey = pickupZoneID + "|" + to_string(hour);
        slotCountsMap[slotKey]++;
    }
}

std::vector<ZoneCount> TripAnalyzer::topZones(int k) const {
    vector<ZoneCount> result;

    for (const auto& entry : zoneCountsMap) {
        result.push_back({ entry.first, entry.second });
    }

    sort(result.begin(), result.end(),
        [](const ZoneCount& a, const ZoneCount& b) {
            if (a.count != b.count)
                return a.count > b.count;
            return a.zone < b.zone;
        });

    if (result.size() > static_cast<size_t>(k)) {
        result.resize(k);
    }

    return result;
}

std::vector<SlotCount> TripAnalyzer::topBusySlots(int k) const {
    vector<SlotCount> result;

    for (const auto& entry : slotCountsMap) {
        const string& key = entry.first;
        long long count = entry.second;

        size_t pos = key.find('|');
        if (pos == string::npos) {
            continue;
        }

        string zone = key.substr(0, pos);
        int hour = stoi(key.substr(pos + 1));

        result.push_back({ zone, hour, count });
    }

    sort(result.begin(), result.end(),
        [](const SlotCount& a, const SlotCount& b) {
            if (a.count != b.count)
                return a.count > b.count;
            if (a.zone != b.zone)
                return a.zone < b.zone;
            return a.hour < b.hour;
        });

    if (result.size() > static_cast<size_t>(k)) {
        result.resize(k);
    }

    return result;
}
