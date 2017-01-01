export REPODIR=$PWD

mkdir -p "$HOME/builds/FermentationMonitor"
pushd "$HOME/builds/FermentationMonitor"
cmake -DCMAKE_TOOLCHAIN_FILE=../Toolchain-gcc-chip-arm.cmake $REPODIR
popd
