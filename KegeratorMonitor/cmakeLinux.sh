export REPODIR=$PWD

mkdir -p "$HOME/builds/KegeratorMonitor"
pushd "$HOME/builds/KegeratorMonitor"
cmake -DCMAKE_TOOLCHAIN_FILE=../Toolchain-gcc-linaro-arm.cmake $REPODIR
popd
