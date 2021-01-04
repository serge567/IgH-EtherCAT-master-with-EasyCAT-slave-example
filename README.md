# IgH EtherCAT master with EasyCAT slave example

**Files:**
*    **ethercat.tar.bz2.00**  - IgH EtherCAT master, part 1
*    **ethercat.tar.bz2.01**  - IgH EtherCAT master, part 2
*    **main.c**               - IgH EtherCAT master modifyed example for EasyCAT slave, overwrite with main.c file in /ethercat/examples/dc_user and compile there with "make"
*    **ethercat**             - config file for ethercat master, copy it to /etc/sysconfig, change lan mac and name (look installed lan for ethercat master)
                           
Combine IgH EtherCAT master source code to one file with 
*    cat ethercat.tar.bz2.?? > ethercat.tar.bz2

Unpack IgH EtherCAT master source code with
*    tar -xvjf ethercat.tar.bz2

Use ./configure --disable-8139too instead of ./configure, read /ethercat/INSTALL for details

After IgH EtherCAT master installed, check ethercat service settings /etc/sysconfig/ethercat, start ethercat service with
* /etc/init.d/ethercat start

It might be helpful to check ethercat service messages with
* dmesg
* cat /var/log/messages

Check connected EasyCAT slave with
* cd /usr/local/bin
* ./ethercat slave -m0
* ./ethercat pdos -m0

Expected output of "./ethercat slave -m0":
* 0  0:0  PREOP  +  Generic 32+32 bytes rev 1

Expected output of "./ethercat pdos -m0":
* SM0: PhysAddr 0x1000, DefaultSize    0, ControlRegister 0x64, Enable 1 RxPDO 0x1600 "Outputs"
*    PDO entry 0x0005:01,  8 bit, "Byte0"

       ::::::::::::::::::::::::::::::
*    PDO entry 0x0005:20,  8 bit, "Byte31"
* SM1: PhysAddr 0x1200, DefaultSize    0, ControlRegister 0x20, Enable 1 TxPDO 0x1a00 "Inputs"
*    PDO entry 0x0006:01,  8 bit, "Byte0"

       ::::::::::::::::::::::::::::::
*    PDO entry 0x0006:20,  8 bit, "Byte31"

Example code (main.c) creates files to use bash script or console to send/display received data to/from EasyCAT slave

* /run/ethercat/EasyCAT/pdoin0
* ::::::::::::::::::::::::::::
* /run/ethercat/EasyCAT/pdoin31

* /run/ethercat/EasyCAT/pdoout0
* :::::::::::::::::::::::::::::
* /run/ethercat/EasyCAT/pdoout31

Example to display received byte from input 1
* cat /run/ethercat/EasyCAT/pdoin1

Example to send byte 255 to output 10
* echo 255 > /run/ethercat/EasyCAT/pdoout10
