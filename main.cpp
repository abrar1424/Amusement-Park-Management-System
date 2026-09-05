#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <map>
#include <set>

using namespace std;
struct User
{
    int id;
    string name;
    string email;
    string password;
    string role;
    int age;
    string phone;
};

struct Zone
{
    int id;
    string name;
    string description;
};

struct Ride
{
    int id;
    string name;
    int zoneId;
    int capacity;
    string status;
    int waitMinutes;
    int minimumAge;
    int intensity;
};

struct Ticket
{
    int id;
    int customerId;
    string visitDate;
    string ticketType;
    double price;
    string paymentStatus;
    string qrCode;
    string bookedAt;
};

struct Payment
{
    int id;
    int ticketId;
    string method;
    double amount;
    string status;
    string paidAt;
};

struct ParkEntry
{
    int id;
    int ticketId;
    string gateName;
    string entryTime;
    string exitTime;
    string status;
};

struct RideUsage
{
    int id;
    int customerId;
    int rideId;
    int ticketId;
    string entryTime;
};

struct Maintenance
{
    int id;
    int rideId;
    int staffId;
    string issueDescription;
    string maintenanceDate;
    string status;
};

struct Feedback
{
    int id;
    int customerId;
    int rideId;
    int rating;
    string type;
    string comments;
    string status;
    string submittedAt;
};

struct EmergencyAlert
{
    int id;
    string title;
    string message;
    string severity;
    bool active;
    string createdAt;
};

struct EmergencyRequest
{
    int id;
    int customerId;
    int zoneId;
    string description;
    string status;
    int assignedStaffId;
    string createdAt;
};

vector<User> usersTable;
vector<Zone> zonesTable;
vector<Ride> ridesTable;
vector<Ticket> ticketsTable;
vector<Payment> paymentsTable;
vector<ParkEntry> parkEntriesTable;
vector<RideUsage> rideUsageTable;
vector<Maintenance> maintenanceTable;
vector<Feedback> feedbackTable;
vector<EmergencyAlert> alertsTable;
vector<EmergencyRequest> emergencyRequestsTable;

User currentUser;
bool isLoggedIn = false;

const string DATA_FILE = "amusement_park_data.txt";

string trim(const string &value)
{
    size_t start = value.find_first_not_of(" \t\r\n");

    if(start == string::npos)
    {
        return "";
    }

    size_t ending = value.find_last_not_of(" \t\r\n");

    return value.substr(start, ending - start + 1);
}

string sanitize(string value)
{
    replace(value.begin(), value.end(), '|', '/');
    replace(value.begin(), value.end(), '\n', ' ');
    replace(value.begin(), value.end(), '\r', ' ');

    return trim(value);
}

vector<string> split(const string &line, char delimiter = '|')
{
    vector<string> parts;
    string item;
    stringstream ss(line);

    while(getline(ss, item, delimiter))
    {
        parts.push_back(item);
    }

    return parts;
}

string currentDateTime()
{
    time_t currentTime = time(nullptr);

    tm *localTime = localtime(&currentTime);

    char buffer[30];

    strftime(
        buffer,
        sizeof(buffer),
        "%Y-%m-%d %H:%M:%S",
        localTime
    );

    return buffer;
}

string currentDate()
{
    time_t currentTime = time(nullptr);

    tm *localTime = localtime(&currentTime);

    char buffer[20];

    strftime(
        buffer,
        sizeof(buffer),
        "%Y-%m-%d",
        localTime
    );

    return buffer;
}

string readLine(const string &prompt)
{
    cout << prompt;

    string value;

    getline(cin, value);

    return sanitize(value);
}

int readInteger(
    const string &prompt,
    int minimum,
    int maximum
)
{
    while(true)
    {
        string input = readLine(prompt);

        try
        {
            size_t usedCharacters = 0;

            int value = stoi(
                input,
                &usedCharacters
            );

            if(
                usedCharacters == input.size() &&
                value >= minimum &&
                value <= maximum
            )
            {
                return value;
            }
        }
        catch(...)
        {
        }

        cout
            << "Invalid input. Enter a number between "
            << minimum
            << " and "
            << maximum
            << ".\n";
    }
}

template <typename T>

int generateNextId(const vector<T> &table)
{
    int highestId = 0;

    for(const auto &record : table)
    {
        highestId = max(
            highestId,
            record.id
        );
    }

    return highestId + 1;
}

User* findUser(int id)
{
    for(auto &user : usersTable)
    {
        if(user.id == id)
        {
            return &user;
        }
    }

    return nullptr;
}

Zone* findZone(int id)
{
    for(auto &zone : zonesTable)
    {
        if(zone.id == id)
        {
            return &zone;
        }
    }

    return nullptr;
}

Ride* findRide(int id)
{
    for(auto &ride : ridesTable)
    {
        if(ride.id == id)
        {
            return &ride;
        }
    }

    return nullptr;
}

Ticket* findTicket(int id)
{
    for(auto &ticket : ticketsTable)
    {
        if(ticket.id == id)
        {
            return &ticket;
        }
    }

    return nullptr;
}

Ticket* findTicketByQrCode(const string &qrCode)
{
    for(auto &ticket : ticketsTable)
    {
        if(ticket.qrCode == qrCode)
        {
            return &ticket;
        }
    }

    return nullptr;
}

string getUserName(int id)
{
    User *user = findUser(id);

    if(user)
    {
        return user->name;
    }

    return "Unknown";
}

string getZoneName(int id)
{
    Zone *zone = findZone(id);

    if(zone)
    {
        return zone->name;
    }

    return "Unknown";
}

string getRideName(int id)
{
    Ride *ride = findRide(id);

    if(ride)
    {
        return ride->name;
    }

    return "Unknown";
}

bool emailAlreadyExists(const string &email)
{
    for(const auto &user : usersTable)
    {
        if(user.email == email)
        {
            return true;
        }
    }

    return false;
}

void initializeSampleData()
{
    usersTable =
    {
        {
            1,
            "System Admin",
            "admin@park.com",
            "admin123",
            "ADMIN",
            35,
            "01710000001"
        },

        {
            2,
            "Demo Customer",
            "customer@park.com",
            "customer123",
            "CUSTOMER",
            24,
            "01810000001"
        },

        {
            3,
            "Ride Operator",
            "staff@park.com",
            "staff123",
            "STAFF",
            30,
            "01910000001"
        }
    };

    zonesTable =
    {
        {
            1,
            "Adventure Zone",
            "High-thrill rides"
        },

        {
            2,
            "Family Zone",
            "Family and children rides"
        },

        {
            3,
            "Water Zone",
            "Water-based attractions"
        }
    };

    ridesTable =
    {
        {
            1,
            "Sky Roller Coaster",
            1,
            24,
            "ACTIVE",
            35,
            12,
            5
        },

        {
            2,
            "Pirate Ship",
            1,
            30,
            "ACTIVE",
            20,
            10,
            4
        },

        {
            3,
            "Magic Carousel",
            2,
            40,
            "ACTIVE",
            10,
            3,
            1
        },

        {
            4,
            "Mini Ferris Wheel",
            2,
            20,
            "UNDER_MAINTENANCE",
            0,
            5,
            2
        },

        {
            5,
            "Splash River",
            3,
            18,
            "ACTIVE",
            25,
            8,
            3
        }
    };

    ticketsTable.clear();

    paymentsTable.clear();

    parkEntriesTable.clear();

    rideUsageTable.clear();

    maintenanceTable.clear();

    feedbackTable.clear();

    alertsTable =
    {
        {
            1,
            "Welcome",
            "The amusement park is open from 9 AM to 8 PM.",
            "LOW",
            true,
            currentDateTime()
        }
    };

    emergencyRequestsTable.clear();
}

void saveData()
{
    ofstream file(DATA_FILE);

    if(!file)
    {
        cout << "Warning: Could not save data file.\n";

        return;
    }

    for(const auto &user : usersTable)
    {
        file
            << "USER|"
            << user.id << "|"
            << user.name << "|"
            << user.email << "|"
            << user.password << "|"
            << user.role << "|"
            << user.age << "|"
            << user.phone
            << endl;
    }

    for(const auto &zone : zonesTable)
    {
        file
            << "ZONE|"
            << zone.id << "|"
            << zone.name << "|"
            << zone.description
            << endl;
    }

    for(const auto &ride : ridesTable)
    {
        file
            << "RIDE|"
            << ride.id << "|"
            << ride.name << "|"
            << ride.zoneId << "|"
            << ride.capacity << "|"
            << ride.status << "|"
            << ride.waitMinutes << "|"
            << ride.minimumAge << "|"
            << ride.intensity
            << endl;
    }

    for(const auto &ticket : ticketsTable)
    {
        file
            << "TICKET|"
            << ticket.id << "|"
            << ticket.customerId << "|"
            << ticket.visitDate << "|"
            << ticket.ticketType << "|"
            << ticket.price << "|"
            << ticket.paymentStatus << "|"
            << ticket.qrCode << "|"
            << ticket.bookedAt
            << endl;
    }

    for(const auto &payment : paymentsTable)
    {
        file
            << "PAYMENT|"
            << payment.id << "|"
            << payment.ticketId << "|"
            << payment.method << "|"
            << payment.amount << "|"
            << payment.status << "|"
            << payment.paidAt
            << endl;
    }

    for(const auto &entry : parkEntriesTable)
    {
        file
            << "ENTRY|"
            << entry.id << "|"
            << entry.ticketId << "|"
            << entry.gateName << "|"
            << entry.entryTime << "|"
            << entry.exitTime << "|"
            << entry.status
            << endl;
    }

    for(const auto &usage : rideUsageTable)
    {
        file
            << "USAGE|"
            << usage.id << "|"
            << usage.customerId << "|"
            << usage.rideId << "|"
            << usage.ticketId << "|"
            << usage.entryTime
            << endl;
    }

    for(const auto &maintenance : maintenanceTable)
    {
        file
            << "MAINTENANCE|"
            << maintenance.id << "|"
            << maintenance.rideId << "|"
            << maintenance.staffId << "|"
            << maintenance.issueDescription << "|"
            << maintenance.maintenanceDate << "|"
            << maintenance.status
            << endl;
    }

    for(const auto &feedback : feedbackTable)
    {
        file
            << "FEEDBACK|"
            << feedback.id << "|"
            << feedback.customerId << "|"
            << feedback.rideId << "|"
            << feedback.rating << "|"
            << feedback.type << "|"
            << feedback.comments << "|"
            << feedback.status << "|"
            << feedback.submittedAt
            << endl;
    }

    for(const auto &alert : alertsTable)
    {
        file
            << "ALERT|"
            << alert.id << "|"
            << alert.title << "|"
            << alert.message << "|"
            << alert.severity << "|"
            << (alert.active ? 1 : 0) << "|"
            << alert.createdAt
            << endl;
    }

    for(const auto &request : emergencyRequestsTable)
    {
        file
            << "EMERGENCY|"
            << request.id << "|"
            << request.customerId << "|"
            << request.zoneId << "|"
            << request.description << "|"
            << request.status << "|"
            << request.assignedStaffId << "|"
            << request.createdAt
            << endl;
    }

    file.close();
}

void loadData()
{
    ifstream file(DATA_FILE);

    if(!file)
    {
        initializeSampleData();

        saveData();

        return;
    }

    usersTable.clear();

    zonesTable.clear();

    ridesTable.clear();

    ticketsTable.clear();

    paymentsTable.clear();

    parkEntriesTable.clear();

    rideUsageTable.clear();

    maintenanceTable.clear();

    feedbackTable.clear();

    alertsTable.clear();

    emergencyRequestsTable.clear();

    string line;

    try
    {
        while(getline(file, line))
        {
            vector<string> parts = split(line);

            if(parts.empty())
            {
                continue;
            }

            if(
                parts[0] == "USER" &&
                parts.size() == 8
            )
            {
                usersTable.push_back(
                {
                    stoi(parts[1]),
                    parts[2],
                    parts[3],
                    parts[4],
                    parts[5],
                    stoi(parts[6]),
                    parts[7]
                }
                );
            }

            else if(
                parts[0] == "ZONE" &&
                parts.size() == 4
            )
            {
                zonesTable.push_back(
                {
                    stoi(parts[1]),
                    parts[2],
                    parts[3]
                }
                );
            }

            else if(
                parts[0] == "RIDE" &&
                parts.size() == 9
            )
            {
                ridesTable.push_back(
                {
                    stoi(parts[1]),
                    parts[2],
                    stoi(parts[3]),
                    stoi(parts[4]),
                    parts[5],
                    stoi(parts[6]),
                    stoi(parts[7]),
                    stoi(parts[8])
                }
                );
            }

            else if(
                parts[0] == "TICKET" &&
                parts.size() == 9
            )
            {
                ticketsTable.push_back(
                {
                    stoi(parts[1]),
                    stoi(parts[2]),
                    parts[3],
                    parts[4],
                    stod(parts[5]),
                    parts[6],
                    parts[7],
                    parts[8]
                }
                );
            }

            else if(
                parts[0] == "PAYMENT" &&
                parts.size() == 7
            )
            {
                paymentsTable.push_back(
                {
                    stoi(parts[1]),
                    stoi(parts[2]),
                    parts[3],
                    stod(parts[4]),
                    parts[5],
                    parts[6]
                }
                );
            }

            else if(
                parts[0] == "ENTRY" &&
                parts.size() == 7
            )
            {
                parkEntriesTable.push_back(
                {
                    stoi(parts[1]),
                    stoi(parts[2]),
                    parts[3],
                    parts[4],
                    parts[5],
                    parts[6]
                }
                );
            }

            else if(
                parts[0] == "USAGE" &&
                parts.size() == 6
            )
            {
                rideUsageTable.push_back(
                {
                    stoi(parts[1]),
                    stoi(parts[2]),
                    stoi(parts[3]),
                    stoi(parts[4]),
                    parts[5]
                }
                );
            }

            else if(
                parts[0] == "MAINTENANCE" &&
                parts.size() == 7
            )
            {
                maintenanceTable.push_back(
                {
                    stoi(parts[1]),
                    stoi(parts[2]),
                    stoi(parts[3]),
                    parts[4],
                    parts[5],
                    parts[6]
                }
                );
            }

            else if(
                parts[0] == "FEEDBACK" &&
                parts.size() == 9
            )
            {
                feedbackTable.push_back(
                {
                    stoi(parts[1]),
                    stoi(parts[2]),
                    stoi(parts[3]),
                    stoi(parts[4]),
                    parts[5],
                    parts[6],
                    parts[7],
                    parts[8]
                }
                );
            }

            else if(
                parts[0] == "ALERT" &&
                parts.size() == 7
            )
            {
                alertsTable.push_back(
                {
                    stoi(parts[1]),
                    parts[2],
                    parts[3],
                    parts[4],
                    stoi(parts[5]) == 1,
                    parts[6]
                }
                );
            }

            else if(
                parts[0] == "EMERGENCY" &&
                parts.size() == 8
            )
            {
                emergencyRequestsTable.push_back(
                {
                    stoi(parts[1]),
                    stoi(parts[2]),
                    stoi(parts[3]),
                    parts[4],
                    parts[5],
                    stoi(parts[6]),
                    parts[7]
                }
                );
            }
        }
    }

    catch(...)
    {
        cout
            << "Saved data is damaged. "
            << "Sample data has been restored.\n";

        initializeSampleData();

        saveData();
    }

    file.close();

    if(
        usersTable.empty() ||
        zonesTable.empty() ||
        ridesTable.empty()
    )
    {
        initializeSampleData();

        saveData();
    }
}

bool login(
    const string &email,
    const string &password
)
{
    for(const auto &user : usersTable)
    {
        if(
            user.email == email &&
            user.password == password
        )
        {
            currentUser = user;

            isLoggedIn = true;

            return true;
        }
    }

    return false;
}

void registerCustomer()
{
    cout
        << "\n========== CUSTOMER REGISTRATION ==========\n";

    User user;

    user.id = generateNextId(usersTable);

    user.name = readLine("Name     : ");

    user.email = readLine("Email    : ");

    if(emailAlreadyExists(user.email))
    {
        cout
            << "This email is already registered.\n";

        return;
    }

    user.password = readLine("Password : ");

    user.age = readInteger(
        "Age      : ",
        1,
        120
    );

    user.phone = readLine("Phone    : ");

    user.role = "CUSTOMER";

    usersTable.push_back(user);

    saveData();

    cout
        << "Registration successful. "
        << "You can now log in.\n";
}

void viewZones()
{
    cout
        << "\n================ ZONES ================\n";

    cout
        << left
        << setw(8) << "ID"
        << setw(25) << "Zone"
        << "Description\n";

    cout
        << "------------------------------------------------------------\n";

    for(const auto &zone : zonesTable)
    {
        cout
            << left
            << setw(8) << zone.id
            << setw(25) << zone.name
            << zone.description
            << endl;
    }
}

void viewRides()
{
    cout
        << "\n============================== RIDES ==============================\n";

    cout
        << left
        << setw(5) << "ID"
        << setw(24) << "Ride"
        << setw(18) << "Zone"
        << setw(10) << "Capacity"
        << setw(22) << "Status"
        << setw(10) << "Wait"
        << setw(8) << "Age"
        << "Intensity\n";

    cout
        << "--------------------------------------------------------------------------------------------------------------\n";

    for(const auto &ride : ridesTable)
    {
        cout
            << left
            << setw(5) << ride.id
            << setw(24) << ride.name
            << setw(18) << getZoneName(ride.zoneId)
            << setw(10) << ride.capacity
            << setw(22) << ride.status
            << setw(10)
            << (to_string(ride.waitMinutes) + " min")
            << setw(8) << ride.minimumAge
            << ride.intensity
            << "/5"
            << endl;
    }
}

void viewActiveAlerts()
{
    cout
        << "\n========== ACTIVE EMERGENCY ALERTS ==========\n";

    bool found = false;

    for(const auto &alert : alertsTable)
    {
        if(!alert.active)
        {
            continue;
        }

        found = true;

        cout
            << "\nAlert ID : "
            << alert.id
            << "\nTitle    : "
            << alert.title
            << "\nSeverity : "
            << alert.severity
            << "\nMessage  : "
            << alert.message
            << "\nCreated  : "
            << alert.createdAt
            << "\n--------------------------------------------\n";
    }

    if(!found)
    {
        cout << "No active emergency alerts.\n";
    }
}


double getTicketPrice(const string &ticketType)
{
    if(ticketType == "CHILD")
    {
        return 500.00;
    }

    if(ticketType == "ADULT")
    {
        return 800.00;
    }

    if(ticketType == "SENIOR")
    {
        return 600.00;
    }

    if(ticketType == "FAMILY")
    {
        return 2200.00;
    }

    return 1500.00;
}


void payTicket(Ticket &ticket)
{
    if(ticket.paymentStatus == "PAID")
    {
        cout << "Ticket is already paid.\n";

        return;
    }

    cout << "\n========== PAYMENT ==========\n";

    cout << "1. bKash\n";

    cout << "2. Nagad\n";

    cout << "3. Credit/Debit Card\n";

    cout << "4. Cash Counter\n";

    int choice = readInteger(
        "Select payment method: ",
        1,
        4
    );

    string paymentMethod;

    if(choice == 1)
    {
        paymentMethod = "bKash";
    }

    else if(choice == 2)
    {
        paymentMethod = "Nagad";
    }

    else if(choice == 3)
    {
        paymentMethod = "Card";
    }

    else
    {
        paymentMethod = "Cash";
    }

    Payment payment;

    payment.id = generateNextId(paymentsTable);

    payment.ticketId = ticket.id;

    payment.method = paymentMethod;

    payment.amount = ticket.price;

    payment.status = "PAID";

    payment.paidAt = currentDateTime();

    paymentsTable.push_back(payment);

    ticket.paymentStatus = "PAID";

    saveData();

    cout << "\n====================================\n";

    cout << "Payment Successful!\n";

    cout
        << "Payment Method : "
        << paymentMethod
        << endl;

    cout
        << "Amount         : "
        << fixed
        << setprecision(2)
        << ticket.price
        << " BDT"
        << endl;

    cout
        << "Ticket ID      : "
        << ticket.id
        << endl;

    cout
        << "QR Code        : "
        << ticket.qrCode
        << endl;

    cout << "====================================\n";
}

void bookTicket()
{
    cout
        << "\n========== ONLINE TICKET BOOKING ==========\n";

    string visitDate =
        readLine("Visit date (YYYY-MM-DD): ");

    cout << "\n1. CHILD  - 500 BDT\n";

    cout << "2. ADULT  - 800 BDT\n";

    cout << "3. SENIOR - 600 BDT\n";

    cout << "4. FAMILY - 2200 BDT\n";

    cout << "5. VIP    - 1500 BDT\n";

    int choice = readInteger(
        "Select ticket type: ",
        1,
        5
    );

    string ticketType;

    if(choice == 1)
    {
        ticketType = "CHILD";
    }

    else if(choice == 2)
    {
        ticketType = "ADULT";
    }

    else if(choice == 3)
    {
        ticketType = "SENIOR";
    }

    else if(choice == 4)
    {
        ticketType = "FAMILY";
    }

    else
    {
        ticketType = "VIP";
    }

    Ticket ticket;

    ticket.id =
        generateNextId(ticketsTable);

    ticket.customerId =
        currentUser.id;

    ticket.visitDate =
        visitDate;

    ticket.ticketType =
        ticketType;

    ticket.price =
        getTicketPrice(ticketType);

    ticket.paymentStatus =
        "PENDING";

    ticket.qrCode =
        "PARK-" +
        to_string(currentUser.id) +
        "-" +
        to_string(ticket.id) +
        "-" +
        to_string(time(nullptr));

    ticket.bookedAt =
        currentDateTime();

    ticketsTable.push_back(ticket);

    cout << "\nTicket booked successfully.\n";

    cout
        << "Ticket ID : "
        << ticket.id
        << endl;

    cout
        << "Price     : "
        << fixed
        << setprecision(2)
        << ticket.price
        << " BDT"
        << endl;

    int payNow = readInteger(
        "Pay now? (1=Yes, 2=No): ",
        1,
        2
    );

    if(payNow == 1)
    {
        Ticket *savedTicket =
            findTicket(ticket.id);

        if(savedTicket)
        {
            payTicket(*savedTicket);
        }
    }

    else
    {
        saveData();
    }
}


void viewMyTickets()
{
    cout
        << "\n================ MY TICKETS ================\n";

    bool found = false;

    for(const auto &ticket : ticketsTable)
    {
        if(ticket.customerId != currentUser.id)
        {
            continue;
        }

        found = true;

        cout
            << "\nTicket ID      : "
            << ticket.id
            << "\nVisit Date     : "
            << ticket.visitDate
            << "\nTicket Type    : "
            << ticket.ticketType
            << "\nPrice          : "
            << fixed
            << setprecision(2)
            << ticket.price
            << " BDT"
            << "\nPayment Status : "
            << ticket.paymentStatus
            << "\nQR Code        : "
            << ticket.qrCode
            << "\nBooked At      : "
            << ticket.bookedAt
            << "\n--------------------------------------------\n";
    }

    if(!found)
    {
        cout << "No tickets found.\n";
    }
}

void payPendingTicket()
{
    viewMyTickets();

    int ticketId = readInteger(
        "Enter ticket ID to pay: ",
        1,
        1000000
    );

    Ticket *ticket =
        findTicket(ticketId);

    if(
        !ticket ||
        ticket->customerId != currentUser.id
    )
    {
        cout << "Ticket not found.\n";

        return;
    }

    payTicket(*ticket);
}

void rideRecommendation()
{
    cout
        << "\n========== RIDE RECOMMENDATION ==========\n";

    int preferredIntensity =
        readInteger(
            "Preferred intensity (1-5): ",
            1,
            5
        );

    int maximumWait =
        readInteger(
            "Maximum waiting time in minutes: ",
            0,
            300
        );

    struct Recommendation
    {
        double score;
        Ride ride;
    };

    vector<Recommendation> recommendations;

    for(const auto &ride : ridesTable)
    {
        if(ride.status != "ACTIVE")
        {
            continue;
        }

        if(currentUser.age < ride.minimumAge)
        {
            continue;
        }

        if(ride.waitMinutes > maximumWait)
        {
            continue;
        }

        double waitScore;

        if(maximumWait == 0)
        {
            if(ride.waitMinutes == 0)
            {
                waitScore = 35.0;
            }

            else
            {
                waitScore = 0.0;
            }
        }

        else
        {
            waitScore =
                max(
                    0.0,
                    35.0 *
                    (
                        1.0 -
                        (double)ride.waitMinutes /
                        maximumWait
                    )
                );
        }

        double intensityScore =
            max(
                0.0,
                25.0 -
                6.25 *
                abs(
                    ride.intensity -
                    preferredIntensity
                )
            );

        double totalRating = 0.0;

        int ratingCount = 0;

        for(const auto &feedback : feedbackTable)
        {
            if(
                feedback.rideId == ride.id &&
                feedback.type == "FEEDBACK"
            )
            {
                totalRating += feedback.rating;

                ratingCount++;
            }
        }

        double averageRating;

        if(ratingCount > 0)
        {
            averageRating =
                totalRating /
                ratingCount;
        }

        else
        {
            averageRating = 4.0;
        }

        double ratingScore =
            averageRating /
            5.0 *
            30.0;

        int totalUsage = 0;

        for(const auto &usage : rideUsageTable)
        {
            if(usage.rideId == ride.id)
            {
                totalUsage++;
            }
        }

        double popularityScore =
            min(
                10.0,
                totalUsage * 1.5
            );

        double finalScore =
            waitScore +
            intensityScore +
            ratingScore +
            popularityScore;

        recommendations.push_back(
            {
                finalScore,
                ride
            }
        );
    }

    sort(
        recommendations.begin(),
        recommendations.end(),

        [](
            const Recommendation &first,
            const Recommendation &second
        )
        {
            return first.score > second.score;
        }
    );

    if(recommendations.empty())
    {
        cout
            << "No suitable active ride found.\n";

        return;
    }

    cout << "\nRecommended rides:\n";

    int maximumResults =
        min(
            3,
            (int)recommendations.size()
        );

    for(
        int index = 0;
        index < maximumResults;
        index++
    )
    {
        cout
            << index + 1
            << ". "
            << recommendations[index].ride.name
            << " | Wait: "
            << recommendations[index].ride.waitMinutes
            << " minutes"
            << " | Score: "
            << fixed
            << setprecision(2)
            << recommendations[index].score
            << "/100"
            << endl;
    }
}


void submitFeedback()
{
    viewRides();

    int rideId =
        readInteger(
            "Ride ID: ",
            1,
            1000000
        );

    if(!findRide(rideId))
    {
        cout << "Ride not found.\n";

        return;
    }

    cout << "\n1. Feedback\n";

    cout << "2. Complaint\n";

    cout << "3. Safety Report\n";

    int typeChoice =
        readInteger(
            "Select type: ",
            1,
            3
        );

    string feedbackType;

    if(typeChoice == 1)
    {
        feedbackType = "FEEDBACK";
    }

    else if(typeChoice == 2)
    {
        feedbackType = "COMPLAINT";
    }

    else
    {
        feedbackType = "SAFETY_REPORT";
    }

    Feedback feedback;

    feedback.id =
        generateNextId(feedbackTable);

    feedback.customerId =
        currentUser.id;

    feedback.rideId =
        rideId;

    feedback.rating =
        readInteger(
            "Rating (1-5): ",
            1,
            5
        );

    feedback.type =
        feedbackType;

    feedback.comments =
        readLine("Comments: ");

    feedback.status =
        "PENDING";

    feedback.submittedAt =
        currentDateTime();

    feedbackTable.push_back(feedback);

    saveData();

    cout
        << "Feedback submitted successfully.\n";
}


void submitEmergencyRequest()
{
    viewZones();

    int zoneId =
        readInteger(
            "Current zone ID: ",
            1,
            1000000
        );

    if(!findZone(zoneId))
    {
        cout << "Zone not found.\n";

        return;
    }

    EmergencyRequest request;

    request.id =
        generateNextId(emergencyRequestsTable);

    request.customerId =
        currentUser.id;

    request.zoneId =
        zoneId;

    request.description =
        readLine("Describe the emergency: ");

    request.status =
        "PENDING";

    request.assignedStaffId =
        0;

    request.createdAt =
        currentDateTime();

    emergencyRequestsTable.push_back(request);

    saveData();

    cout
        << "Emergency request submitted successfully.\n";

    cout
        << "Request ID: "
        << request.id
        << endl;
}

void updateRideOperation()
{
    viewRides();

    int rideId =
        readInteger(
            "Ride ID: ",
            1,
            1000000
        );

    Ride *ride =
        findRide(rideId);

    if(!ride)
    {
        cout << "Ride not found.\n";

        return;
    }

    cout << "\n1. ACTIVE\n";

    cout << "2. CLOSED\n";

    cout << "3. UNDER_MAINTENANCE\n";

    int statusChoice =
        readInteger(
            "New status: ",
            1,
            3
        );

    if(statusChoice == 1)
    {
        ride->status = "ACTIVE";
    }

    else if(statusChoice == 2)
    {
        ride->status = "CLOSED";
    }

    else
    {
        ride->status =
            "UNDER_MAINTENANCE";
    }

    ride->waitMinutes =
        readInteger(
            "Current waiting time: ",
            0,
            300
        );

    saveData();

    cout << "Ride status updated.\n";
}


void verifyParkEntry()
{
    string qrCode =
        readLine("Enter/scan QR code: ");

    Ticket *ticket =
        findTicketByQrCode(qrCode);

    if(!ticket)
    {
        cout
            << "Invalid QR code. Entry denied.\n";

        return;
    }

    if(ticket->paymentStatus != "PAID")
    {
        cout
            << "Ticket is not paid. Entry denied.\n";

        return;
    }

    for(const auto &entry : parkEntriesTable)
    {
        if(
            entry.ticketId == ticket->id &&
            entry.status == "ENTERED"
        )
        {
            cout
                << "This visitor is already inside the park.\n";

            return;
        }
    }

    ParkEntry entry;

    entry.id =
        generateNextId(parkEntriesTable);

    entry.ticketId =
        ticket->id;

    entry.gateName =
        readLine("Gate name: ");

    entry.entryTime =
        currentDateTime();

    entry.exitTime =
        "";

    entry.status =
        "ENTERED";

    parkEntriesTable.push_back(entry);

    saveData();

    cout
        << "\nEntry verified successfully.\n";

    cout
        << "Customer  : "
        << getUserName(ticket->customerId)
        << endl;

    cout
        << "Ticket ID : "
        << ticket->id
        << endl;
}

// ============================================================
// RECORD VISITOR EXIT
// ============================================================

void recordVisitorExit()
{
    string qrCode =
        readLine("Enter/scan QR code: ");

    Ticket *ticket =
        findTicketByQrCode(qrCode);

    if(!ticket)
    {
        cout << "Invalid QR code.\n";

        return;
    }

    for(
        auto iterator = parkEntriesTable.rbegin();
        iterator != parkEntriesTable.rend();
        iterator++
    )
    {
        if(
            iterator->ticketId == ticket->id &&
            iterator->status == "ENTERED"
        )
        {
            iterator->status = "EXITED";

            iterator->exitTime =
                currentDateTime();

            saveData();

            cout
                << "Visitor exit recorded successfully.\n";

            return;
        }
    }

    cout
        << "No active park entry found for this ticket.\n";
}

void recordRideEntry()
{
    string qrCode =
        readLine("Enter/scan ticket QR code: ");

    Ticket *ticket =
        findTicketByQrCode(qrCode);

    if(
        !ticket ||
        ticket->paymentStatus != "PAID"
    )
    {
        cout
            << "Valid paid ticket not found.\n";

        return;
    }

    User *customer =
        findUser(ticket->customerId);

    viewRides();

    int rideId =
        readInteger(
            "Ride ID: ",
            1,
            1000000
        );

    Ride *ride =
        findRide(rideId);

    if(!ride)
    {
        cout << "Ride not found.\n";

        return;
    }

    if(ride->status != "ACTIVE")
    {
        cout << "Ride is not active.\n";

        return;
    }

    if(
        customer &&
        customer->age < ride->minimumAge
    )
    {
        cout
            << "Visitor does not meet "
            << "the minimum age requirement.\n";

        return;
    }

    RideUsage usage;

    usage.id =
        generateNextId(rideUsageTable);

    usage.customerId =
        ticket->customerId;

    usage.rideId =
        rideId;

    usage.ticketId =
        ticket->id;

    usage.entryTime =
        currentDateTime();

    rideUsageTable.push_back(usage);

    saveData();

    cout
        << "Ride entry recorded successfully.\n";
}

void createMaintenanceRequest()
{
    viewRides();

    int rideId =
        readInteger(
            "Ride ID: ",
            1,
            1000000
        );

    Ride *ride =
        findRide(rideId);

    if(!ride)
    {
        cout << "Ride not found.\n";

        return;
    }

    Maintenance maintenance;

    maintenance.id =
        generateNextId(maintenanceTable);

    maintenance.rideId =
        rideId;

    maintenance.staffId =
        currentUser.id;

    maintenance.issueDescription =
        readLine("Issue description: ");

    maintenance.maintenanceDate =
        currentDate();

    maintenance.status =
        "REQUESTED";

    maintenanceTable.push_back(maintenance);

    ride->status =
        "UNDER_MAINTENANCE";

    saveData();

    cout
        << "Maintenance request created successfully.\n";
}

void viewMaintenance()
{
    cout
        << "\n================ MAINTENANCE RECORDS ================\n";

    if(maintenanceTable.empty())
    {
        cout << "No maintenance records.\n";

        return;
    }

    for(const auto &maintenance : maintenanceTable)
    {
        cout
            << "\nMaintenance ID : "
            << maintenance.id
            << "\nRide           : "
            << getRideName(maintenance.rideId)
            << "\nStaff          : "
            << getUserName(maintenance.staffId)
            << "\nIssue          : "
            << maintenance.issueDescription
            << "\nDate           : "
            << maintenance.maintenanceDate
            << "\nStatus         : "
            << maintenance.status
            << "\n--------------------------------------------\n";
    }
}

void updateMaintenanceStatus()
{
    viewMaintenance();

    if(maintenanceTable.empty())
    {
        return;
    }

    int maintenanceId =
        readInteger(
            "Maintenance ID: ",
            1,
            1000000
        );

    for(auto &maintenance : maintenanceTable)
    {
        if(maintenance.id != maintenanceId)
        {
            continue;
        }

        cout << "\n1. ASSIGNED\n";

        cout << "2. IN_PROGRESS\n";

        cout << "3. COMPLETED\n";

        cout << "4. CANCELLED\n";

        int choice =
            readInteger(
                "New status: ",
                1,
                4
            );

        if(choice == 1)
        {
            maintenance.status = "ASSIGNED";
        }

        else if(choice == 2)
        {
            maintenance.status = "IN_PROGRESS";
        }

        else if(choice == 3)
        {
            maintenance.status = "COMPLETED";

            Ride *ride =
                findRide(maintenance.rideId);

            if(ride)
            {
                ride->status = "ACTIVE";
            }
        }

        else
        {
            maintenance.status = "CANCELLED";
        }

        saveData();

        cout
            << "Maintenance status updated.\n";

        return;
    }

    cout
        << "Maintenance record not found.\n";
}


void viewEmergencyRequests()
{
    cout
        << "\n================ EMERGENCY REQUESTS ================\n";

    if(emergencyRequestsTable.empty())
    {
        cout << "No emergency requests.\n";

        return;
    }

    for(const auto &request : emergencyRequestsTable)
    {
        cout
            << "\nRequest ID  : "
            << request.id
            << "\nCustomer    : "
            << getUserName(request.customerId)
            << "\nZone        : "
            << getZoneName(request.zoneId)
            << "\nDescription : "
            << request.description
            << "\nStatus      : "
            << request.status
            << "\nAssigned    : ";

        if(request.assignedStaffId == 0)
        {
            cout << "Not assigned";
        }

        else
        {
            cout
                << getUserName(
                    request.assignedStaffId
                );
        }

        cout
            << "\nCreated     : "
            << request.createdAt
            << "\n--------------------------------------------\n";
    }
}

void updateEmergencyRequest()
{
    viewEmergencyRequests();

    if(emergencyRequestsTable.empty())
    {
        return;
    }

    int requestId =
        readInteger(
            "Emergency request ID: ",
            1,
            1000000
        );

    for(auto &request : emergencyRequestsTable)
    {
        if(request.id != requestId)
        {
            continue;
        }

        cout << "\n1. ACKNOWLEDGED\n";

        cout << "2. IN_PROGRESS\n";

        cout << "3. RESOLVED\n";

        int choice =
            readInteger(
                "New status: ",
                1,
                3
            );

        if(choice == 1)
        {
            request.status = "ACKNOWLEDGED";
        }

        else if(choice == 2)
        {
            request.status = "IN_PROGRESS";
        }

        else
        {
            request.status = "RESOLVED";
        }

        request.assignedStaffId =
            currentUser.id;

        saveData();

        cout
            << "Emergency request updated.\n";

        return;
    }

    cout << "Request not found.\n";
}


void addRide()
{
    viewZones();

    Ride ride;

    ride.id =
        generateNextId(ridesTable);

    ride.name =
        readLine("Ride name: ");

    ride.zoneId =
        readInteger(
            "Zone ID: ",
            1,
            1000000
        );

    if(!findZone(ride.zoneId))
    {
        cout << "Zone not found.\n";

        return;
    }

    ride.capacity =
        readInteger(
            "Capacity: ",
            1,
            10000
        );

    ride.status = "ACTIVE";

    ride.waitMinutes =
        readInteger(
            "Initial waiting time: ",
            0,
            300
        );

    ride.minimumAge =
        readInteger(
            "Minimum age: ",
            1,
            120
        );

    ride.intensity =
        readInteger(
            "Intensity (1-5): ",
            1,
            5
        );

    ridesTable.push_back(ride);

    saveData();

    cout << "Ride added successfully.\n";
}

void updateRide()
{
    viewRides();

    int rideId =
        readInteger(
            "Ride ID to update: ",
            1,
            1000000
        );

    Ride *ride =
        findRide(rideId);

    if(!ride)
    {
        cout << "Ride not found.\n";

        return;
    }

    ride->name =
        readLine("New ride name: ");

    viewZones();

    int zoneId =
        readInteger(
            "New zone ID: ",
            1,
            1000000
        );

    if(findZone(zoneId))
    {
        ride->zoneId = zoneId;
    }

    ride->capacity =
        readInteger(
            "New capacity: ",
            1,
            10000
        );

    ride->waitMinutes =
        readInteger(
            "New waiting time: ",
            0,
            300
        );

    ride->minimumAge =
        readInteger(
            "New minimum age: ",
            1,
            120
        );

    ride->intensity =
        readInteger(
            "New intensity (1-5): ",
            1,
            5
        );

    saveData();

    cout << "Ride updated successfully.\n";
}

void deleteRide()
{
    viewRides();

    int rideId =
        readInteger(
            "Ride ID to delete: ",
            1,
            1000000
        );

    for(
        auto iterator = ridesTable.begin();
        iterator != ridesTable.end();
        iterator++
    )
    {
        if(iterator->id == rideId)
        {
            ridesTable.erase(iterator);

            saveData();

            cout
                << "Ride deleted successfully.\n";

            return;
        }
    }

    cout << "Ride not found.\n";
}

void addStaff()
{
    User staff;

    staff.id =
        generateNextId(usersTable);

    staff.name =
        readLine("Staff name: ");

    staff.email =
        readLine("Staff email: ");

    if(emailAlreadyExists(staff.email))
    {
        cout << "Email already exists.\n";

        return;
    }

    staff.password =
        readLine("Temporary password: ");

    staff.age =
        readInteger(
            "Age: ",
            18,
            120
        );

    staff.phone =
        readLine("Phone: ");

    staff.role =
        "STAFF";

    usersTable.push_back(staff);

    saveData();

    cout << "Staff account created.\n";
}


void viewAllTickets()
{
    cout
        << "\n================ ALL TICKETS ================\n";

    if(ticketsTable.empty())
    {
        cout << "No tickets found.\n";

        return;
    }

    for(const auto &ticket : ticketsTable)
    {
        cout
            << "\nTicket ID  : "
            << ticket.id
            << "\nCustomer   : "
            << getUserName(ticket.customerId)
            << "\nVisit Date : "
            << ticket.visitDate
            << "\nType       : "
            << ticket.ticketType
            << "\nPrice      : "
            << fixed
            << setprecision(2)
            << ticket.price
            << " BDT"
            << "\nPayment    : "
            << ticket.paymentStatus
            << "\nQR Code    : "
            << ticket.qrCode
            << "\n--------------------------------------------\n";
    }
}


void revenueReport()
{
    cout
        << "\n================ REVENUE REPORT ================\n";

    map<string, pair<int, double>> dailyRevenue;

    double totalRevenue = 0.0;

    int totalPaidTickets = 0;

    for(const auto &ticket : ticketsTable)
    {
        if(ticket.paymentStatus == "PAID")
        {
            dailyRevenue[ticket.visitDate].first++;

            dailyRevenue[ticket.visitDate].second +=
                ticket.price;

            totalRevenue += ticket.price;

            totalPaidTickets++;
        }
    }

    cout
        << left
        << setw(15) << "Visit Date"
        << setw(18) << "Paid Tickets"
        << "Revenue (BDT)"
        << endl;

    cout
        << "--------------------------------------------------\n";

    for(const auto &record : dailyRevenue)
    {
        cout
            << left
            << setw(15) << record.first
            << setw(18) << record.second.first
            << fixed
            << setprecision(2)
            << record.second.second
            << endl;
    }

    cout
        << "\nTotal Paid Tickets : "
        << totalPaidTickets
        << endl;

    cout
        << "Total Revenue      : "
        << fixed
        << setprecision(2)
        << totalRevenue
        << " BDT"
        << endl;
}


void popularRideReport()
{
    cout
        << "\n================ MOST POPULAR RIDE REPORT ================\n";

    map<int, int> rideCounts;

    map<int, set<int>> uniqueCustomers;

    for(const auto &usage : rideUsageTable)
    {
        rideCounts[usage.rideId]++;

        uniqueCustomers[usage.rideId].insert(
            usage.customerId
        );
    }

    if(rideCounts.empty())
    {
        cout
            << "No ride usage data found.\n";

        return;
    }

    int mostPopularRideId = 0;

    int highestUsage = -1;

    cout
        << left
        << setw(28) << "Ride"
        << setw(18) << "Total Entries"
        << "Unique Riders"
        << endl;

    cout
        << "-----------------------------------------------------------------\n";

    for(const auto &ride : ridesTable)
    {
        int usageCount =
            rideCounts[ride.id];

        cout
            << left
            << setw(28) << ride.name
            << setw(18) << usageCount
            << uniqueCustomers[ride.id].size()
            << endl;

        if(usageCount > highestUsage)
        {
            highestUsage = usageCount;

            mostPopularRideId = ride.id;
        }
    }

    cout
        << "\nMost Popular Ride : "
        << getRideName(mostPopularRideId)
        << endl;

    cout
        << "Total Entries     : "
        << highestUsage
        << endl;
}


void dailyVisitorReport()
{
    cout
        << "\n================ DAILY VISITOR REPORT ================\n";

    map<string, set<int>> uniqueTickets;

    map<string, int> totalGateScans;

    for(const auto &entry : parkEntriesTable)
    {
        if(
            entry.status == "ENTERED" ||
            entry.status == "EXITED"
        )
        {
            string date =
                entry.entryTime.substr(0, 10);

            uniqueTickets[date].insert(
                entry.ticketId
            );

            totalGateScans[date]++;
        }
    }

    if(totalGateScans.empty())
    {
        cout
            << "No park-entry data found.\n";

        return;
    }

    cout
        << left
        << setw(15) << "Date"
        << setw(22) << "Verified Visitors"
        << "Gate Scans"
        << endl;

    cout
        << "-------------------------------------------------------\n";

    for(const auto &record : totalGateScans)
    {
        cout
            << left
            << setw(15) << record.first
            << setw(22)
            << uniqueTickets[record.first].size()
            << record.second
            << endl;
    }
}

void satisfactionReport()
{
    cout
        << "\n================ CUSTOMER SATISFACTION ================\n";

    map<int, pair<int, int>> ratingInformation;

    for(const auto &feedback : feedbackTable)
    {
        ratingInformation[feedback.rideId].first +=
            feedback.rating;

        ratingInformation[feedback.rideId].second++;
    }

    if(ratingInformation.empty())
    {
        cout
            << "No feedback data found.\n";

        return;
    }

    cout
        << left
        << setw(28) << "Ride"
        << setw(18) << "Feedback Count"
        << "Average Rating"
        << endl;

    cout
        << "-----------------------------------------------------------------\n";

    for(const auto &ride : ridesTable)
    {
        int feedbackCount =
            ratingInformation[ride.id].second;

        if(feedbackCount == 0)
        {
            continue;
        }

        double averageRating =
            (double)ratingInformation[ride.id].first /
            feedbackCount;

        cout
            << left
            << setw(28) << ride.name
            << setw(18) << feedbackCount
            << fixed
            << setprecision(2)
            << averageRating
            << "/5"
            << endl;
    }
}

void viewAndResolveFeedback()
{
    cout
        << "\n================ FEEDBACK / COMPLAINTS ================\n";

    if(feedbackTable.empty())
    {
        cout << "No feedback found.\n";

        return;
    }

    for(const auto &feedback : feedbackTable)
    {
        cout
            << "\nFeedback ID : "
            << feedback.id
            << "\nCustomer    : "
            << getUserName(feedback.customerId)
            << "\nRide        : "
            << getRideName(feedback.rideId)
            << "\nType        : "
            << feedback.type
            << "\nRating      : "
            << feedback.rating
            << "/5"
            << "\nComments    : "
            << feedback.comments
            << "\nStatus      : "
            << feedback.status
            << "\n--------------------------------------------\n";
    }

    int updateChoice =
        readInteger(
            "Update a record? (1=Yes, 2=No): ",
            1,
            2
        );

    if(updateChoice == 2)
    {
        return;
    }

    int feedbackId =
        readInteger(
            "Feedback ID: ",
            1,
            1000000
        );

    for(auto &feedback : feedbackTable)
    {
        if(feedback.id != feedbackId)
        {
            continue;
        }

        cout << "\n1. REVIEWED\n";

        cout << "2. RESOLVED\n";

        int statusChoice =
            readInteger(
                "New status: ",
                1,
                2
            );

        if(statusChoice == 1)
        {
            feedback.status = "REVIEWED";
        }

        else
        {
            feedback.status = "RESOLVED";
        }

        saveData();

        cout
            << "Feedback status updated.\n";

        return;
    }

    cout << "Feedback not found.\n";
}

void createEmergencyAlert()
{
    EmergencyAlert alert;

    alert.id =
        generateNextId(alertsTable);

    alert.title =
        readLine("Alert title: ");

    alert.message =
        readLine("Alert message: ");

    cout << "\n1. LOW\n";

    cout << "2. MEDIUM\n";

    cout << "3. HIGH\n";

    cout << "4. CRITICAL\n";

    int severityChoice =
        readInteger(
            "Severity: ",
            1,
            4
        );

    if(severityChoice == 1)
    {
        alert.severity = "LOW";
    }

    else if(severityChoice == 2)
    {
        alert.severity = "MEDIUM";
    }

    else if(severityChoice == 3)
    {
        alert.severity = "HIGH";
    }

    else
    {
        alert.severity = "CRITICAL";
    }

    alert.active = true;

    alert.createdAt =
        currentDateTime();

    alertsTable.push_back(alert);

    saveData();

    cout
        << "Emergency alert published.\n";
}

void resolveEmergencyAlert()
{
    viewActiveAlerts();

    int alertId =
        readInteger(
            "Alert ID to resolve: ",
            1,
            1000000
        );

    for(auto &alert : alertsTable)
    {
        if(alert.id == alertId)
        {
            alert.active = false;

            saveData();

            cout << "Alert resolved.\n";

            return;
        }
    }

    cout << "Alert not found.\n";
}


void dashboard()
{
    double totalRevenue = 0.0;

    int totalPaidTickets = 0;

    int activeRides = 0;

    int openMaintenance = 0;

    int activeAlerts = 0;

    int totalCustomers = 0;

    for(const auto &user : usersTable)
    {
        if(user.role == "CUSTOMER")
        {
            totalCustomers++;
        }
    }

    for(const auto &ticket : ticketsTable)
    {
        if(ticket.paymentStatus == "PAID")
        {
            totalRevenue += ticket.price;

            totalPaidTickets++;
        }
    }

    for(const auto &ride : ridesTable)
    {
        if(ride.status == "ACTIVE")
        {
            activeRides++;
        }
    }

    for(const auto &maintenance : maintenanceTable)
    {
        if(
            maintenance.status != "COMPLETED" &&
            maintenance.status != "CANCELLED"
        )
        {
            openMaintenance++;
        }
    }

    for(const auto &alert : alertsTable)
    {
        if(alert.active)
        {
            activeAlerts++;
        }
    }

    int pendingFeedback = 0;

    for(const auto &feedback : feedbackTable)
    {
        if(feedback.status == "PENDING")
        {
            pendingFeedback++;
        }
    }

    cout
        << "\n================ ADMIN DASHBOARD ================\n";

    cout
        << "Total Customers       : "
        << totalCustomers
        << endl;

    cout
        << "Total Rides           : "
        << ridesTable.size()
        << endl;

    cout
        << "Active Rides          : "
        << activeRides
        << endl;

    cout
        << "Total Tickets         : "
        << ticketsTable.size()
        << endl;

    cout
        << "Paid Tickets          : "
        << totalPaidTickets
        << endl;

    cout
        << "Verified Park Entries : "
        << parkEntriesTable.size()
        << endl;

    cout
        << "Ride Usage Records    : "
        << rideUsageTable.size()
        << endl;

    cout
        << "Open Maintenance      : "
        << openMaintenance
        << endl;

    cout
        << "Pending Feedback      : "
        << pendingFeedback
        << endl;

    cout
        << "Emergency Requests    : "
        << emergencyRequestsTable.size()
        << endl;

    cout
        << "Active Alerts         : "
        << activeAlerts
        << endl;

    cout
        << "Total Revenue         : "
        << fixed
        << setprecision(2)
        << totalRevenue
        << " BDT"
        << endl;
}


void customerMenu()
{
    int choice;

    do
    {
        cout
            << "\n=================================================\n";

        cout
            << "                CUSTOMER PANEL\n";

        cout
            << "=================================================\n";

        cout << "1. View Zones\n";

        cout << "2. View Rides and Waiting Time\n";

        cout << "3. Book Ticket\n";

        cout << "4. View My Tickets\n";

        cout << "5. Pay Pending Ticket\n";

        cout << "6. Get Ride Recommendation\n";

        cout << "7. Submit Feedback / Complaint\n";

        cout << "8. Emergency Help Request\n";

        cout << "9. View Emergency Alerts\n";

        cout << "10. Logout\n";

        choice =
            readInteger(
                "Enter choice: ",
                1,
                10
            );

        switch(choice)
        {
            case 1:

                viewZones();

                break;

            case 2:

                viewRides();

                break;

            case 3:

                bookTicket();

                break;

            case 4:

                viewMyTickets();

                break;

            case 5:

                payPendingTicket();

                break;

            case 6:

                rideRecommendation();

                break;

            case 7:

                submitFeedback();

                break;

            case 8:

                submitEmergencyRequest();

                break;

            case 9:

                viewActiveAlerts();

                break;

            case 10:

                cout << "Customer logged out.\n";

                break;
        }

    }
    while(choice != 10);
}


void staffMenu()
{
    int choice;

    do
    {
        cout
            << "\n=================================================\n";

        cout
            << "                  STAFF PANEL\n";

        cout
            << "=================================================\n";

        cout << "1. View Rides\n";

        cout
            << "2. Update Ride Status and Waiting Time\n";

        cout << "3. Verify QR Park Entry\n";

        cout << "4. Record Visitor Exit\n";

        cout << "5. Record Ride Entry\n";

        cout << "6. Create Maintenance Request\n";

        cout << "7. View Maintenance Records\n";

        cout << "8. Update Maintenance Status\n";

        cout << "9. View Emergency Requests\n";

        cout << "10. Update Emergency Request\n";

        cout << "11. View Emergency Alerts\n";

        cout << "12. Logout\n";

        choice =
            readInteger(
                "Enter choice: ",
                1,
                12
            );

        switch(choice)
        {
            case 1:

                viewRides();

                break;

            case 2:

                updateRideOperation();

                break;

            case 3:

                verifyParkEntry();

                break;

            case 4:

                recordVisitorExit();

                break;

            case 5:

                recordRideEntry();

                break;

            case 6:

                createMaintenanceRequest();

                break;

            case 7:

                viewMaintenance();

                break;

            case 8:

                updateMaintenanceStatus();

                break;

            case 9:

                viewEmergencyRequests();

                break;

            case 10:

                updateEmergencyRequest();

                break;

            case 11:

                viewActiveAlerts();

                break;

            case 12:

                cout << "Staff logged out.\n";

                break;
        }

    }
    while(choice != 12);
}


void adminMenu()
{
    int choice;

    do
    {
        cout
            << "\n=================================================\n";

        cout
            << "                  ADMIN PANEL\n";

        cout
            << "=================================================\n";

        cout << "1. Dashboard\n";

        cout << "2. View Zones\n";

        cout << "3. View Rides\n";

        cout << "4. Add Ride\n";

        cout << "5. Update Ride\n";

        cout << "6. Delete Ride\n";

        cout << "7. Add Staff Account\n";

        cout << "8. View All Tickets\n";

        cout << "9. Revenue Report\n";

        cout << "10. Most Popular Ride Report\n";

        cout << "11. Daily Visitor Report\n";

        cout << "12. Customer Satisfaction Report\n";

        cout << "13. View Maintenance Records\n";

        cout << "14. View / Resolve Feedback\n";

        cout << "15. Create Emergency Alert\n";

        cout << "16. Resolve Emergency Alert\n";

        cout << "17. View Emergency Requests\n";

        cout << "18. Logout\n";

        choice =
            readInteger(
                "Enter choice: ",
                1,
                18
            );

        switch(choice)
        {
            case 1:

                dashboard();

                break;

            case 2:

                viewZones();

                break;

            case 3:

                viewRides();

                break;

            case 4:

                addRide();

                break;

            case 5:

                updateRide();

                break;

            case 6:

                deleteRide();

                break;

            case 7:

                addStaff();

                break;

            case 8:

                viewAllTickets();

                break;

            case 9:

                revenueReport();

                break;

            case 10:

                popularRideReport();

                break;

            case 11:

                dailyVisitorReport();

                break;

            case 12:

                satisfactionReport();

                break;

            case 13:

                viewMaintenance();

                break;

            case 14:

                viewAndResolveFeedback();

                break;

            case 15:

                createEmergencyAlert();

                break;

            case 16:

                resolveEmergencyAlert();

                break;

            case 17:

                viewEmergencyRequests();

                break;

            case 18:

                cout << "Admin logged out.\n";

                break;
        }

    }
    while(choice != 18);
}


int main()
{
    loadData();

    cout
        << "=====================================================\n";

    cout
        << "       AMUSEMENT PARK MANAGEMENT SYSTEM\n";

    cout
        << "=====================================================\n";

    cout
        << "Database : Local Table Simulation\n";

    cout
        << "Storage  : amusement_park_data.txt\n";

    cout
        << "Status   : System Ready\n";

    cout
        << "=====================================================\n";

    while(true)
    {
        cout << "\n1. Login\n";

        cout << "2. Register as Customer\n";

        cout << "3. Reset Demo Data\n";

        cout << "4. Exit\n";

        int choice =
            readInteger(
                "Enter choice: ",
                1,
                4
            );

        if(choice == 2)
        {
            registerCustomer();

            continue;
        }

        if(choice == 3)
        {
            int confirmation =
                readInteger(
                    "Restore sample data? (1=Yes, 2=No): ",
                    1,
                    2
                );

            if(confirmation == 1)
            {
                initializeSampleData();

                saveData();

                cout << "Demo data restored.\n";
            }

            continue;
        }

        if(choice == 4)
        {
            saveData();

            cout
                << "\nThank you for using "
                << "the Amusement Park Management System.\n";

            break;
        }

        string email =
            readLine("Email    : ");

        string password =
            readLine("Password : ");

        if(!login(email, password))
        {
            cout
                << "Invalid email or password.\n";

            continue;
        }

        cout << "\nLogin Successful!\n";

        cout
            << "Welcome : "
            << currentUser.name
            << endl;

        cout
            << "Role    : "
            << currentUser.role
            << endl;

        if(currentUser.role == "ADMIN")
        {
            adminMenu();
        }

        else if(currentUser.role == "STAFF")
        {
            staffMenu();
        }

        else if(currentUser.role == "CUSTOMER")
        {
            customerMenu();
        }

        else
        {
            cout << "Unknown user role.\n";
        }

        isLoggedIn = false;

        saveData();
    }

    return 0;
}
