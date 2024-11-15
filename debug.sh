mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
gdb voicevox_engine_cpp
cd ..
