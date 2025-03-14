# Jellyfin Monitor

This project is a C++ program that monitors the status of a media server and triggers a media scan on Jellyfin when Tdarr transcoding process is done. The program makes use of the `libcurl` library for HTTP requests and the `nlohmann::json` library for handling JSON responses.

## Features

- Monitors a custom API to check the current status of workers and GPU transcoding queue.
- Triggers a Jellyfin media scan when all workers are idle and the GPU transcoding queue is empty.
- Rechecks every 10 seconds to keep conditions up to date.

## Requirements

- **C++17 or later** Follow [these steps to install MINGW64 for windows](https://code.visualstudio.com/docs/cpp/config-mingw#_prerequisites)
- **libcurl**: for making HTTP requests.
- **nlohmann::json**: for handling JSON responses.

## Installation

### Using vcpkg (recommended)

1. Install [vcpkg](https://github.com/microsoft/vcpkg) by following the installation instructions.
2. Install the required libraries:

    ```bash
    vcpkg install nlohmann-json curl
    ```

3. Integrate `vcpkg` with your project by running:

    ```bash
    vcpkg integrate install
    ```

4. Compile your project using `g++` or any other compiler that supports linking with vcpkg libraries:

    ```bash
    g++ -o jellyfin_monitor.exe final.cpp -I[path_to_vcpkg]/installed/x64-windows/include -L[path_to_vcpkg]/installed/x64-windows/lib -lcurl -pthread
    ```
### Using MingW to install libcurl (Alternate)(Follow these steps that you will need on the system to run the EXE)
    ```bash
    # First, update the package database and install development tools
    pacman -Syuu  # This updates the package database and system

    # Install curl with development headers and libraries for MinGW 64-bit:
    pacman -S mingw-w64-x86_64-curl

    # If you're using a 32-bit system, install the 32-bit version:
    pacman -S mingw-w64-i686-curl

    ```
### Manual Setup

1. Download the `nlohmann::json` header from [here](https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp).
2. Create a folder `nlohmann` in your project directory and place the `json.hpp` file inside it.
3. Install `libcurl` by following the instructions for your platform (e.g., use [vcpkg](https://github.com/microsoft/vcpkg) or manually download and set it up).
4. Compile your project:

    ```bash
    g++ -o jellyfin_monitor.exe final.cpp -I. -lcurl -pthread
    ```

## Usage

After compiling the program, run it in the terminal or command prompt:

```bash
./jellyfin_monitor.exe
