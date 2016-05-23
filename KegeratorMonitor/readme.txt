### To enable i2c-0 ###
add bcm2708.vc_i2c_override=1 to /boot/cmdline.txt
add dtparam=i2c_vc=on to /boot/config.txt

### To disable screensaver ###
sudo apt-get update
sudo apt-get install xscreensaver
Disable from Linux GUI (in Preference)

### Iceweasel Fullscreen ###
Just kill iceweasel while in fullscreen, it will restart in fullscreen.

### Co2 tank ###
Tank empty mass: 102
Tank full mass: ~162

### fstab ###
//192.168.0.103/philx02/MyPassport/Kegerator/data /home/pi/KegeratorMonitor/data cifs user=philx02,password=orbitxxx,sec=ntlm,uid=pi,gid=pi 0 0