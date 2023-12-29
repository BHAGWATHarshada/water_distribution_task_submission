#include <iostream>
#include <fstream>
#include "json.hpp"
#include <iomanip>

using json = nlohmann::json;

using namespace std;
class Schedule
{
public:
    string day;
    int duration;
    Schedule(const string &_day, int _duration) : day(_day), duration(_duration) {}
};

class SupplyProfile
{
private:
    static struct tm validityTimeInfo;

public:
    int stackLevel;
    string houseID;
    string supplyKind;
    string recurrencyKind;
    string transactionID;
    string validityDateTime;
    string scheduleStartTime;
    int scheduleDuration;
    vector<Schedule> schedules;

    SupplyProfile() {}

    static SupplyProfile fromJson(const json &jsonValue)
    {
        SupplyProfile profile;

        try
        {
            if (jsonValue.count("stackLevel") > 0)
                profile.stackLevel = jsonValue.at("stackLevel").get<int>();

            if (jsonValue.count("houseID") > 0)
                profile.houseID = jsonValue.at("houseID").get<std::string>();

            if (jsonValue.count("supplyKind") > 0)
                profile.supplyKind = jsonValue.at("supplyKind").get<std::string>();

            if (jsonValue.count("recurrencyKind") > 0)
                profile.recurrencyKind = jsonValue.at("recurrencyKind").get<std::string>();

            if (jsonValue.count("transactionID") > 0)
                profile.transactionID = jsonValue.at("transactionID").get<std::string>();

            if (jsonValue.count("validityDateTime") > 0)
            {
                std::istringstream dateStream(jsonValue.at("validityDateTime").get<std::string>());
                dateStream >> std::get_time(&validityTimeInfo, "%d.%m.%Y to %d.%m.%Y");
            }

            if (jsonValue.count("scheduleStartTime") > 0)
                profile.scheduleStartTime = jsonValue.at("scheduleStartTime").get<std::string>();

            if (jsonValue.count("scheduleDuration") > 0)
                profile.scheduleDuration = jsonValue.at("scheduleDuration").get<int>();

            if (jsonValue.count("schedules") > 0)
            {
                const json &schedules = jsonValue.at("schedules");
                for (const auto &schedule : schedules)
                {
                    if (schedule.count("day") > 0 && schedule.count("duration") > 0)
                        profile.schedules.emplace_back(schedule.at("day").get<std::string>(), schedule.at("duration").get<int>());
                }
            }
        }
        catch (const std::exception &e)
        {
            cerr << "Error parsing JSON: " << e.what() << endl;
            throw;
        }

        return profile;
    }

    friend class WaterDistributionLibrary;
};

struct tm SupplyProfile::validityTimeInfo = {};

class SupplyStatus
{
public:
    string referenceDateTime;
    string houseID;
    string currentTransactionID;
    string currentSupplyStartTime;

    // Additional fields
    int currentSupplyProfileID;
    int currentScheduleID;
    int limitValue;
    string limitType;

    SupplyStatus() {}
};

class WaterDistributionLibrary
{
public:
    static SupplyProfile parseJsonSupplyProfile(const std::string &jsonString)
    {
        try
        {
            json root = json::parse(jsonString);
            return SupplyProfile::fromJson(root);
        }
        catch (const std::exception &e)
        {
            cerr << "Error parsing JSON: " << e.what() << endl;
            throw;
        }
    }

    static SupplyStatus determineCurrentSupplyStatus(const SupplyProfile &profile)
    {
        struct tm validityTimeInfo = {};
        if (!profile.validityDateTime.empty())
        {
            std::istringstream dateStream(profile.validityDateTime);
            dateStream >> std::get_time(&validityTimeInfo, "%d.%m.%Y to %d.%m.%Y");
        }

        char buffer[100];
        strftime(buffer, sizeof(buffer), "%d.%m.%Y to %d.%m.%Y", &validityTimeInfo);

        SupplyStatus status;

        status.referenceDateTime = buffer;
        status.houseID = profile.houseID;

        status.currentTransactionID = profile.transactionID;
        status.currentSupplyStartTime = profile.scheduleStartTime;

        status.currentSupplyProfileID = 1;
        status.currentScheduleID = 1;
        status.limitValue = 2000;
        status.limitType = "Gallon";

        return status;
    }
};

int main()
{
    // Example
    string jsonProfile = R"(
        {
    "stackLevel": 4,
    "houseID": "123",
    "supplyKind": "Once",
    "transactionID": "ABC123",
    "validityDateTime": "01.12.2022 to 30.12.2022",
    "scheduleStartTime": "08:00",
    "scheduleDuration": 2,
    "schedules": [
        {"day": "Monday", "duration": 2},
        {"day": "Tuesday", "duration": 5},
        {"day": "Wednesday", "duration": 1}
    ],
    "currentTransactionID": "XYZ789",   
    "currentSupplyStartTime": "10:00"   
}

    )";

    SupplyProfile profile = WaterDistributionLibrary::parseJsonSupplyProfile(jsonProfile);

    // Printing details here
    cout << "Stack Level: " << profile.stackLevel << endl;
    cout << "House ID: " << profile.houseID << endl;
    cout << "Supply Kind: " << profile.supplyKind << endl;

    // here determining the current supply status
    SupplyStatus status = WaterDistributionLibrary::determineCurrentSupplyStatus(profile);
    cout << "Current Transaction ID: " << status.currentTransactionID << endl;
    cout << "Current Supply Start Time: " << status.currentSupplyStartTime << endl;

    // Additional fields
    cout << "Current Supply Profile ID: " << status.currentSupplyProfileID << endl;
    cout << "Current Schedule ID: " << status.currentScheduleID << endl;
    cout << "Limit Value: " << status.limitValue << " " << status.limitType << endl;

    return 0;
}
