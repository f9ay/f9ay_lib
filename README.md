clone repo
```bash
git clone --recurse-submodules <repo url>
```

generate build file
```bash
mkdir build
cd build
cmake ..
```

build debug (in build directory)
```bash
cmake --build .
```

build release (in build directory)
```bash
cmake --build . --config Release
```

update submodule
```bash
git submodule update --init --recursive
```
