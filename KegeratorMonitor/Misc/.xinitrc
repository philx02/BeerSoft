xset s off
xset -dpms
xset s noblank
chromium-browser --window-size=1920,1080 --window-position=0,0 --kiosk --incognito "http://localhost/kegerator/?server=ws://localhost:8012&editmode=1"
