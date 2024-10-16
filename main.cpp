#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std::chrono_literals;

// Configuration
const std::string API_URL = "http://<ip-domain-tdarr-server>/api/v2/get-nodes";
const std::string JELLYFIN_API_URL = "http://<Jellyfin-ip-address>";
const std::string JELLYFIN_API_KEY = "<Jellyfin-api-key>";

// Helper function for HTTP GET requests
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* out) {
    size_t totalSize = size * nmemb;
    if (out) {
        out->append((char*)contents, totalSize);
    }
    return totalSize;
}

// Function to send HTTP GET request and return the response
std::string httpGet(const std::string& url) {
    CURL* curl;
    CURLcode res;
    std::string response;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            std::cerr << "Error during GET request: " << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return response;
}

// Function to send HTTP POST request to trigger Jellyfin scan
void triggerJellyfinScan() {
    CURL* curl;
    CURLcode res;
    struct curl_slist* headers = NULL;
    std::string api_url = JELLYFIN_API_URL + "/Library/Refresh";

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        headers = curl_slist_append(headers, ("X-Emby-Token: " + JELLYFIN_API_KEY).c_str());

        curl_easy_setopt(curl, CURLOPT_URL, api_url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        res = curl_easy_perform(curl);
        if(res == CURLE_OK) {
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            if (response_code == 204) {
                std::cout << "Triggered Jellyfin media scan successfully" << std::endl;
            } else {
                std::cerr << "Failed to trigger Jellyfin scan, status code: " << response_code << std::endl;
            }
        } else {
            std::cerr << "Error during POST request: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    curl_global_cleanup();
}

int main() {
    bool jellyfinScanTriggered = false;

    while (true) {
        try {
            // Fetch the current status from the API
            std::string response = httpGet(API_URL);
            if (response.empty()) {
                throw std::runtime_error("Empty response from API");
            }

            // Parse the response JSON
            json responseData = json::parse(response);

            auto workers = responseData["dPjqRnrnl"]["workers"];
            auto queueLengths = responseData["dPjqRnrnl"]["queueLengths"];

            // Check the conditions
            bool workersEmpty = workers.empty();
            int transcodegpuQueue = queueLengths.value("transcodegpu", 0);

            // Debugging outputs
            std::cout << "Workers empty: " << workersEmpty
                      << ", Transcode GPU queue: " << transcodegpuQueue << std::endl;

            // Trigger Jellyfin scan if conditions are met
            if (workersEmpty && transcodegpuQueue == 0) {
                if (!jellyfinScanTriggered) {
                    triggerJellyfinScan();
                    jellyfinScanTriggered = true;  // Mark that the scan has been triggered
                }
            } else {
                jellyfinScanTriggered = false;  // Reset the flag if conditions are not met
            }

            std::this_thread::sleep_for(10s);  // Wait for 10 seconds before the next check

        } catch (const std::exception& e) {
            std::cerr << "Error occurred: " << e.what() << std::endl;
            std::this_thread::sleep_for(10s);  // Wait before retrying on error
        }
    }

    return 0;
}
