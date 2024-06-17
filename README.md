# cpptobiwarp

This is an application that warps the mouse cursor ahead of the line of sight when certain buttons are pressed.

No eye tracking data is stored or transmitted to other devices or networks by this application.

## Important Notice

Please note that while you are free to use and modify this source code for personal use, you are not permitted to share or distribute any built executables or other compiled forms of this project. This restriction is in accordance with the license agreement for the Tobii Game Integration library.

To use this project, each user must obtain their own personal use license for the Tobii Game Integration library from [Tobii's official site](https://www.tobii.com/tech/contact/sales-form/).

## Requirements

- g++ compiler (part of MinGW on Windows)
- Tobii Game Integration library
- Windows OS
- Visual Studio Code (recommended for development)

The "include" and "lib" folders and their contents must be located in the root directory of this project.

## Project structure

```
cpptobiwarp/
├── bin/
│ └── x64/
│  └── <built_executable>.exe
├── include/
│ ├── <library_header_files>.h
│ └── <library_source_files>.cpp
├── lib/
│ └── <library_files>.dll
└── src/
  └── <main_source_code>.cpp
```

## Setup Instructions

1. **Clone the repository**:

2. **Install g++**:

   - Ensure you have g++ installed. You can install it via MinGW on Windows.
   - Download and install MinGW from [MinGW official site](http://www.mingw.org/).
   - Add the `bin` directory of MinGW to your system's PATH.

3. **Place Tobii Game Integration library**:

   - Copy the necessary header files to the `include` directory.
   - Copy the necessary DLL files to the `lib` directory.

4. **Build the project**:

   - Open the project in Visual Studio Code.
   - Ensure you have the C++ extension installed.
   - Build the project using the provided tasks in `.vscode/tasks.json`.

   You can build the project by running the following command in the terminal:

```sh
g++ -g src/main.cpp -o bin/myApp -Iinclude -Llib -ltobii_gameintegration_x64 -lstdc++
```

    Alternatively, you can use the build tasks defined in `.vscode/tasks.json`:
    - Press `Ctrl+Shift+B` to run the default build task.

## Note

This project uses the Tobii Game Integration library, which requires a personal use license. Please obtain the necessary license from [Tobii's official site](https://www.tobii.com/tech/contact/sales-form/).

This project is intended for personal use only. For commercial use or distribution, a commercial license from Tobii is required.
