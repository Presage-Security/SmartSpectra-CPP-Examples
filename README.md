# Smart Spectra C++ Examples
This repository holds usage examples for the Smart Spectra C++ SDK, which demonstrate how to use the SDK in various C++
applications to obtain physiological metrics, i.e. pulse an breathing rate, from different forms of visual input.

## Supported Systems & Architectures

We currently provide packages only for Linux Debian, Ubuntu, and Mint distributions running on amd64/x86_64 architecture.
However, we plan to support the following in the near future:

| Package Type (OS List)      | CPU Architecture (System Type) |
|-----------------------------|--------------------------------|
| Debian (Debian/Ubuntu/Mint) | `amd64`,`arm64` (`linux`)      |
| RPM (RHEL/Fedora)           | `x86_64`,`aarch64` (`linux`)   |
| Bundle (MacOS X)            | `arm64e`           (`darwin`)  |

Windows support is may be added in future versions depending on demand.
## Smart Spectra C++ SDK Setup Guide for Debian / Ubuntu / Linux Mint

### Setting up the Presage Personal Package Archive (PPA)
You'll want to set up the Presage PPA to be able to install and update the Smart Spectra C++ SDK. You'll only need to do this once on your system.

Run the following commands in your terminal.
1. Download the GPG key:
    ```bash
    curl -s "https://presage-security.github.io/PPA/KEY.gpg" | gpg --dearmor | sudo tee /etc/apt/trusted.gpg.d/presage-technologies.gpg >/dev/null
    ```
2. Copy the PPA list:
    ```bash
    sudo curl -s --compressed -o /etc/apt/sources.list.d/presage-technologies.list "https://presage-security.github.io/PPA/presage-technologies.list"
    ```

### Installing/Updating the Smart Spectra C++ SDK

1. Update the `apt` database:
    ```bash
    sudo apt update
    ```
2. Install (or upgrade) the SDK:
    ```bash
    sudo apt install libsmartspectra-dev
    ```

## Building and Running Smart Spectra C++ Examples

1. Clone this repository.
2. Build the examples:
    ```bash
    mkdir build
    cd build
    cmake -G Ninja ..
    ninja
    ```
3. Register and obtain the Presage Technologies Physiology API key from https://physiology.presagetech.com/ 
4. Run the C++ web example (substitute `<YOUR_API_KEY>` with your Physiology API key):
    ```bash
    cpp_web_example/cpp_web_example --also_log_to_stderr \
      --camera_device_index=0 --auto_lock=false --physiology_key=<YOUR_API_KEY_HERE>
    ```

