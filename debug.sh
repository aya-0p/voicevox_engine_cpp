mkdir build
cd build
clear
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
gdb voicevox_engine_cpp
cd ..
