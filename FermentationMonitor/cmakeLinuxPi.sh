export REPODIR=$PWD

mkdir -p "$HOME/builds/FermentationMonitor"
pushd "$HOME/builds/FermentationMonitor"
cmake -DCMAKE_TOOLCHAIN_FILE=../Toolchain-gcc-linaro-arm.cmake $REPODIR
popd
