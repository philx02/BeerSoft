#|/bin/bash
pushd ~/builds/BeerSoft
make
scp bin/* pi@fermon:/home/pi/FermentationMonitor
scp lib/* pi@fermon:/home/pi/FermentationMonitor
popd
