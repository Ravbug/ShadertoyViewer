## ShadertoyViewer

A native app for viewing Shadertoy projects written in C++

### Building
1. Install the following prereqs:
   - macOS: Xcode, CMake
   - Windows: Visual Studio, CMake
2. `git clone` the source code
3. Create `src/key.hpp` with the following contents:
```c
#define API_KEY "YOUR SHADERTOY API KEY HERE"
```
4. Execute `config/config-mac.sh` or `config/config-win.sh` depending on your platform. It may take a while the first time.
5. Go to the `build` folder that was created by the script and open the Xcode or Visual Studio project
6. Select the `ShadertoyViewer` target and Run