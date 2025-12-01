#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std::chrono_literals;

const std::string API_URL = "http://<ip-address>:8265/api/v2/get-nodes";
const std::string JELLYFIN_API_URL = "http://127.0.0.1:8096";
const std::string JELLYFIN_API_KEY = "<token>";

// Callback
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* out) {
    size_t total = size * nmemb;
    if (out)
        out->append((char*)contents, total);
    return total;
}

// GET request
std::string httpGet(const std::string& url) {
    CURL* curl = curl_easy_init();
    std::string response;

    if (!curl) return "";

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);              // prevent freezes
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
        std::cerr << "GET error: " << curl_easy_strerror(res) << std::endl;

    curl_easy_cleanup(curl);
    return response;
}

// POST trigger Jellyfin
void triggerJellyfinScan() {
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Could not init curl POST\n";
        return;
    }

    std::string response;
    struct curl_slist* headers = NULL;
    std::string url = JELLYFIN_API_URL + "/Library/Refresh";

    headers = curl_slist_append(headers, ("X-Emby-Token: " + JELLYFIN_API_KEY).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);

    if (res == CURLE_OK) {
        long code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);

        if (code == 204)
            std::cout << "Jellyfin scan triggered OK\n";
        else
            std::cerr << "Jellyfin returned code: " << code << " body: " << response << "\n";

    } else {
        std::cerr << "POST error: " << curl_easy_strerror(res) << std::endl;
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
}

int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);   // ONLY ONCE

    bool jellyfinScanTriggered = false;

    while (true) {
        try {
            std::string response = httpGet(API_URL);

            if (response.empty()) {
                std::cerr << "Empty API response\n";
                std::this_thread::sleep_for(5s);
                continue;
            }

            json data = json::parse(response);

            auto nodeID = data.begin().key();
            auto node = data[nodeID];

            auto workers = node["workers"];
            int transcodegpuQueue = node["queueLengths"].value("transcodegpu", 0);

            std::cout << "Node: " << nodeID
                      << " workers: " << workers.size()
                      << " gpuQ: " << transcodegpuQueue << std::endl;

            if (workers.empty() && transcodegpuQueue == 0) {
                if (!jellyfinScanTriggered) {
                    triggerJellyfinScan();
                    jellyfinScanTriggered = true;
                }
            } else {
                jellyfinScanTriggered = false;
            }

        } catch (std::exception& e) {
            std::cerr << "Exception: " << e.what() << "\n";
        }

        std::this_thread::sleep_for(10s);
    }

    curl_global_cleanup();  // ONLY ONCE
}
