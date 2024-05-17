#include <httpserver.hpp>
#include <cpr/cpr.h>

#define CACHE_CAPACITY 500000 // Size of the Cache.
// Cache items
HashTable* cacheTable = createCache(50000);

// Cache definition
typedef struct cacheItem
{
    std::string key;
    std::string value;
} cacheItem;

// Defines the HashTable used for the cache
typedef struct HashTable
{
    cacheItem** items;
    int size;
    int count;
} HashTable;

// Adds IP addresses to our cache so we can check the cache quickly to validate the country 
cacheItem* addIP(std::string ip, std::string country)
{
    // Creates a pointer to a new HashTable item.
    cacheItem* item = (cacheItem*) malloc(sizeof(cacheItem));
    item->key = ip;
    item->value = country;
    return item;
}

// Creates our hash function for checking our cache
unsigned long hash_function(std::string ip)
{
    unsigned long i = 0;

    for (int j = 0; ip[j]; j++)
        i += ip[j];

    return i % CACHE_CAPACITY;
}

// Our cache table
HashTable* createCache(int size)
{
    // Creates a new HashTable.
    HashTable* table = (HashTable*) malloc(sizeof(HashTable));
    table->size = size;
    table->count = 0;
    table->items = (cacheItem**) calloc(table->size, sizeof(cacheItem*));

    for (int i = 0; i < table->size; i++)
        table->items[i] = NULL;

    return table;
}

// Define the HTTP Server
using namespace httpserver;
class IPValidator : public http_resource {
public:
    std::shared_ptr<http_response> render_POST(const http_request&) {
        // Pull the IP and the allowed countires from the JSON body
        std::string ipAddress = "";
        std::vector<std::string> allowedCountries = {};

        // If the IP is missing then this is a bad request
        if (ipAddress == "") {
            return std::shared_ptr<http_response>(new string_response("IP Address is missing", 422, "text/plain"));
        }
        // Return false here, cause of course it won't be allowed if the list is empty
        if (allowedCountries.size() == 0) {
            return std::shared_ptr<http_response>(new string_response("{\"IP-Allowed\":false}}"));
        }

        // Valdiate the IP address by checking cache then checking the API
        bool ipAllowed = validateIP(ipAddress, allowedCountries);

        return std::shared_ptr<http_response>(new string_response("{\"IP-Allowed\":" + std::to_string(ipAllowed) + "}}"));
    }
};

// Validate if the IP is good or not
bool validateIP(std::string ip, std::vector<std::string> allowedCountries) {
    // Check cache first
    int index = hash_function(ip);
    if (index != NULL) {
        cacheItem* country = cacheTable->items[index];
    
        if (ip == country->value) {
            return true;
        }
    }
    
    // TODO: If not in cache then check our Database

    return false;
}

std::string getCountry() {

}

int main(int argc, char** argv) {
    // Create a new Webserver using httpserver
    webserver webServer = create_webserver(8080);

    // Register our api as /ip-validate
    IPValidator malik;
    webServer.register_resource("/ip-validate", &malik);
    webServer.start(true);
        
    return 0;
}