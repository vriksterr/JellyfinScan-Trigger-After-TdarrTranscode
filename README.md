# Jellyfin Monitor

This project is a C++ program that monitors the status of a media server and triggers a media scan on Jellyfin when the Tdarr transcoding process is complete.  
It uses:

- **libcurl** — HTTP requests  
- **nlohmann::json** — JSON parsing  

---

## Features

- Monitors a custom Tdarr API to track worker activity and GPU transcode queue.
- Automatically triggers a Jellyfin library scan when:
  - All Tdarr workers are idle  
  - GPU queue is empty  
- Re-checks every 10 seconds.

---

# Requirements

- **C++17 or later**
- **libcurl**
- **nlohmann::json**
- Works on **Windows** and **Linux (Debian/Ubuntu)**

---

# Installation

# ⚡ Windows Installation

## ✅ Using vcpkg (recommended)

1. Install [vcpkg](https://github.com/microsoft/vcpkg)
2. Install required packages:

    ```bash
    vcpkg install nlohmann-json curl
    ```

3. Integrate vcpkg:

    ```bash
    vcpkg integrate install
    ```

4. Compile:

    ```bash
    g++ -o jellyfin_monitor.exe final.cpp ^
        -I[path_to_vcpkg]/installed/x64-windows/include ^
        -L[path_to_vcpkg]/installed/x64-windows/lib ^
        -lcurl -pthread
    ```

---

## ⚡ Using MinGW (Alternate)

```bash
pacman -Syuu
pacman -S mingw-w64-x86_64-curl
# For 32-bit:
# pacman -S mingw-w64-i686-curl
```

---

## ⚡ Manual Setup (Windows)

1. Download the `json.hpp` header:  
   https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp
2. Create a folder `nlohmann/` and place `json.hpp` inside it.
3. Install libcurl using vcpkg or MinGW.
4. Compile:

    ```bash
    g++ -o jellyfin_monitor.exe main.cpp -I. -lcurl -pthread
    ```

---

# 🐧 Linux (Debian/Ubuntu) Installation

Below are the **exact commands** needed to install dependencies and compile the project.

---

## ✅ Step 1 — Update system packages

```bash
sudo apt update
```

---

## ✅ Step 2 — Install C++ compiler (g++, make, etc.)

```bash
sudo apt install -y build-essential
```

---

## ✅ Step 3 — Install libcurl + development headers

```bash
sudo apt install -y libcurl4-openssl-dev
```

---

## ✅ Step 4 — Install nlohmann-json library

```bash
sudo apt install -y nlohmann-json3-dev
```

---

## ✅ Step 5 — Compile the program on Linux

### If using system-installed JSON:

```bash
g++ -o jellyfin_monitor main.cpp -lcurl -pthread
```

### If you manually downloaded `json.hpp`:

```bash
g++ -o jellyfin_monitor main.cpp -I. -lcurl -pthread
```

---

# Usage

## Windows

```bash
.\jellyfin_monitor.exe
```

## Linux

```bash
./jellyfin_monitor
```

---

If you want, I can also generate:

- 🚀 `build.sh` script for Linux  
- 🚀 `build.bat` script for Windows  
- 📦 A cross-platform `CMakeLists.txt`  
- 📦 A `.deb` installer package for Debian  

Just tell me!
