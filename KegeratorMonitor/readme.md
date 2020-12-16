# Install

```bash
sudo apt-get install xinit chromium-browser git apache2 screen python3-pip
sudo pip3 install websockets Adafruit-ADS1x15 Adafruit-BMP
git clone https://github.com/philx02/BeerSoft.git
ln -s BeerSoft/KegeratorMonitor/Misc/.xinitrc
sudo ln -s /home/pi/BeerSoft/KegeratorMonitor/WebSite/ /var/www/html/kegerator
mkdir -p KegeratorMonitor/keg_level
```
Copy the files `keg0.txt` and `keg1.txt` or create them with content `0`.
```bash
pushd KegeratorMonitor && ln -s ../BeerSoft/KegeratorMonitor/Scripts/* . && popd
```
Add this line to /etc/rc.local:
```bash
su - pi -c /home/pi/KegeratorMonitor/go.sh
```
