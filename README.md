# What does this do?
LED lights light up and change colors depending on the volume that the attached microphone hears.
Hearing a very loud voice will change the default color of the lights.


# For Ubuntu users using Arduino Nano.
If you don't see any port when attaching your Arduino to your laptop, check
sudo dmesg

If you see that the device has been disconnected, the best way to deal with it is to uninstall brltty
sudo apt remove brltty -y