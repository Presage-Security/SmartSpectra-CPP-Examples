# Smart Spectra C++ Examples
This repository holds usage examples for the Smart Spectra C++ SDK, which demonstrate how to use the SDK in various C++
applications to obtain physiological metrics, i.e. pulse an breathing rate, from different forms of visual input.

## Table of Contents

 - [Supported Systems & Architectures](#supported-systems--architectures)
 - [Smart Spectra C++ SDK Setup Guide](#smart-spectra-c-sdk-setup-guide)
   - [Debian / Ubuntu/ Linux Mint](#debian--ubuntu-linux-mint)
     - [Installing Build Tools](#installing-build-tools)
     - [Setting up the Presage Personal Package Archive](#setting-up-the-presage-personal-package-archive-ppa)
     - [Installing/Upgrading the Smart Spectra C++ SDK](#installingupgrading-the-smart-spectra-c-sdk)
 - [Building and Running Smart Spectra C++ Examples](#building-and-running-smart-spectra-c-examples)
   - [Building](#building)
   - [Running](#running)
     - [Command Line Interface](#command-line-interface)
     - [Keyboard Shortcuts](#keyboard-shortcuts)
 - [Developing Your Own Smart Spectra C++ Application](#developing-your-own-smart-spectra-c-application) 


## Supported Systems & Architectures

We currently provide packages only for Linux Debian, Ubuntu, and Mint distributions running on amd64/x86_64 architecture.
However, we plan to support the following in the near future:

| Package Type (OS List)      | CPU Architecture (System Type) |
|-----------------------------|--------------------------------|
| Debian (Debian/Ubuntu/Mint) | `amd64`,`arm64` (`linux`)      |
| RPM (RHEL/Fedora)           | `x86_64`,`aarch64` (`linux`)   |
| Bundle (MacOS X)            | `arm64e`           (`darwin`)  |

Windows support is may be added in future versions depending on demand.
## Smart Spectra C++ SDK Setup Guide
### Debian / Ubuntu/ Linux Mint
#### Installing Build Tools
1. CMake 3.28 or newer is required. Follow the [directions from Kitware](https://apt.kitware.com/) (publishers of CMake) to get it set up if your cmake version is below the requirement (run `cmake --version` to check this).
2. Ninja 1.10 or newer is recommended. Run `sudo apt install ninja-build` to install it. If you choose not to do so, you can still use `make` to build. Ensure you have that installed (`sudo apt-install build-essential`).

#### Setting up the Presage Personal Package Archive (PPA)
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

#### Installing/Upgrading the Smart Spectra C++ SDK

1. Update the `apt` database:
    ```bash
    sudo apt update
    ```
2. Install (or upgrade) the SDK:
    ```bash
    sudo apt install libsmartspectra-dev
    ```

## Building and Running Smart Spectra C++ Examples
### Building
1. Clone this repository.
2. Build the examples using either Ninja or Make.
   - Using Ninja: 
       ```bash
       mkdir build
       cd build
       cmake -G Ninja ..
       ninja
       ```
   - Using Make:
       ```bash
       mkdir build
       cd build
       cmake -G Ninja ..
       ninja
       ```
### Running
1. Register and obtain the Presage Technologies Physiology API key from https://physiology.presagetech.com/ 
2. Run the C++ web example (substitute `<YOUR_API_KEY>` with your Physiology REST API key):
    ```bash
    cpp_web_example/cpp_web_example --also_log_to_stderr \
      --camera_device_index=0 --auto_lock=false --physiology_key=<YOUR_API_KEY_HERE>
    ```
#### Command Line Interface
To list and read about all available command line options for an example, pass `--help=main` to terminal command, e.g.:
```bash
    cpp_web_example/cpp_web_example --help=main
```

To read about a specific command line option, pass `--help=<OPTION_NAME>` to terminal command, e.g.:
```bash
    cpp_web_example/cpp_web_example --help=use_gpu
```

#### Keyboard Shortcuts
During the run, use the following keyboard shortcuts:
- `q` or `ESC`: exit
- `s`: start/stop recording data (**webcam input / streaming** mode only)
- `e`: lock/unlock exposure (**webcam input / streaming** mode only)
- `-` and `=`: decrease or increase exposure (**webcam input / streaming** mode only, and only when exposure is locked)

See documentation for the `--auto_loc` option by passing `--help=auto_loc` for more details about locking the camera exposure.

## Developing Your Own Smart Spectra C++ Application

More examples, tutorials, and reference documentation are coming soon! 

In the meantime, please consult the C++ Web Example and C++ File Example code.
The C++ gRPC Example may become more useful in the future, when we provide access to our Physiology gRPC API.
