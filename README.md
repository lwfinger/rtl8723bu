# rtl8723bu
Driver for Realtek RTL8723BU Wireless Adapter with Hardware ID `0bda:b720`

# How to use?
Run the following commands in the Linux terminal.

```
git clone https://github.com/lwfinger/rtl8723bu.git
cd rtl8723bu
```

## Manual install
### a. Concurrent Mode
If you want to operate the hardware as a station AND as an access point *simultaneously*, follow these instructions.  This will show two devices when you run the `iwconfig` command.

Run the following commands in the Linux terminal.

```
make
sudo make install
sudo modprobe -v 8723bu

```

### b. Non Concurrent Mode
If you do not want two devices (station and an access point) separately, then follow these instructions.

Step - 1: Run the following commands in the Linux terminal. 
```
nano Makefile
```

Step - 2: Find the line that contains `EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE` and insert a `#` symbol at the beginning of that line. This comments that line and disables concurrent mode.

Step - 3: Now, run the following commands in the same Linux terminal.

```
make
sudo make install
sudo modprobe -v 8723bu

## Automatic install using DKMS
If you don't want to worry about building/installing driver after kernel update, use this scenario. For Ubuntu/Debian install DKMS package using command `sudo apt install dkms`.

Then run following commands in terminal
```
source dkms.conf
sudo mkdir /usr/src/$PACKAGE_NAME-$PACKAGE_VERSION
sudo cp -r core hal include os_dep platform dkms.conf Makefile rtl8723b_fw.bin /usr/src/$PACKAGE_NAME-$PACKAGE_VERSION
sudo dkms add $PACKAGE_NAME/$PACKAGE_VERSION
sudo dkms autoinstall $PACKAGE_NAME/$PACKAGE_VERSION
```

