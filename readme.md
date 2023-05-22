## Game of life
### Dependencies
1. Cmake 3.8+
2. C++17 compliant compiler
3. boost::program_options 1.71+
<dd> On Ubuntu 20.04+ can be installed by running </dd>

```
sudo apt-get install libboost-options-dev
```

<dd> On Windows can be installed using vcpkg (https://vcpkg.io/en/) </dd>

```
.\vcpkg install boost-program-options
```
4. Optionally gtest for building unit tests
<dd> On Ubuntu 20.04+ can be installed by running </dd>

```
sudo apt-get install libgtest-dev
```

<dd> On Windows can be installed using vcpkg (https://vcpkg.io/en/) </dd>

```
.\vcpkg install gtest
```


### Build Instructions
On ubuntu 20.04+ run the following commands:
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```
The executable will be created in the  REPOSITORY_ROOT/build/bin folder.

To also build tests and run them:
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON ..
make
ctest
```

On Windows, the simplest way is to open root folder of the repository in Visual Studio, and build it as cmake project.

### Usage
To produce a help message, run
```
./game_of_life # or ./game_of_life --help
```

To run game of life for 5 iterations starting from the board in [./examples/example3.txt](./examples/example3.txt) and output all iterations, run
```
./game_of_life --input ../examples/example3.txt --iterations 5 --all
```
The results will be available as ../examples/example3_iteration_number.txt.