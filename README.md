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

Use ./configure --disable-8139too instead of ./configure, read /ethercat/INSTALL for details, 
The IgH EtherCAT master source code has been tested to compile with kernel version 5.4.80-gentoo-x86_64.

After IgH EtherCAT master installed, check ethercat service settings /etc/sysconfig/ethercat,
example
* ifconfig 
   
      enp3s0u1: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
              inet 169.254.124.186  netmask 255.255.0.0  broadcast 169.254.255.255
              ether 70:88:6b:84:2d:b4  txqueuelen 1000  (Ethernet)
              RX packets 358799  bytes 28470392 (27.1 MiB)
              RX errors 0  dropped 0  overruns 0  frame 0
              TX packets 358978  bytes 33531024 (31.9 MiB)
              TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
    
    where the interface name is enp3s0u1, mac is 70:88:6b:84:2d:b4
            
* lsusb  # (use lspci -nn in case of lan is on PCI)
              
      Bus 004 Device 002: ID 0bda:8153 Realtek Semiconductor Corp. RTL8153 Gigabit Ethernet Adapter

    where LAN with chipset RTL8153 (0bda:8153), IgH EtherCAT master source code does not include Ethernet driver module specially for RTL8153 (0bda:8153) to use it for EtherCAT operation, ethercat master "generic" lan driver is going to be used.
    
    for this example, in /etc/sysconfig/ethercat file:
    
    MASTER0_DEVICE="70:88:6b:84:2d:b4"
    
    DEVICE_MODULES="generic"
    
    LINK_DEVICES="enp3s0u1"
    

Start ethercat service with
* /etc/init.d/ethercat start

It might be helpful to check ethercat service messages with
* dmesg
* cat /var/log/messages

Check connected EasyCAT slave with
* cd /usr/local/bin
* ./ethercat slave -m0
* ./ethercat pdos -m0

Expected output of "./ethercat slave -m0":

    0  0:0  PREOP  +  Generic 32+32 bytes rev 1

Expected output of "./ethercat pdos -m0":

    SM0: PhysAddr 0x1000, DefaultSize    0, ControlRegister 0x64, Enable 1 RxPDO 0x1600 "Outputs"

     PDO entry 0x0005:01,  8 bit, "Byte0"

       ::::::::::::::::::::::::::::::

     PDO entry 0x0005:20,  8 bit, "Byte31"
     
    SM1: PhysAddr 0x1200, DefaultSize    0, ControlRegister 0x20, Enable 1 TxPDO 0x1a00 "Inputs"

     PDO entry 0x0006:01,  8 bit, "Byte0"

       ::::::::::::::::::::::::::::::

     PDO entry 0x0006:20,  8 bit, "Byte31"

Copy main.c file to /ethercat/examples/dc_user (/ethercat is IgH EtherCAT master source code)

Go to "/ethercat/examples/dc_user", compile "main.c" there with "make", the output file will be "ec_dc_user_example" 

Start EasyCAT slave example in "/ethercat/examples/dc_user" with
* ./ec_dc_user_example

Example code (main.c) creates files to use bash script or console to send/display received data to/from EasyCAT slave

    /run/ethercat/EasyCAT/pdoin0

    ::::::::::::::::::::::::::::

    /run/ethercat/EasyCAT/pdoin31


    /run/ethercat/EasyCAT/pdoout0

    :::::::::::::::::::::::::::::

    /run/ethercat/EasyCAT/pdoout31

Example to display received byte from input 1
* cat /run/ethercat/EasyCAT/pdoin1

Example to send byte 255 to output 10
* echo 255 > /run/ethercat/EasyCAT/pdoout10
