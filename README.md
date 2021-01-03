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

Example code (main.c) creates files to use bash script or console to send/receive data to/from EasyCAT slave

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
