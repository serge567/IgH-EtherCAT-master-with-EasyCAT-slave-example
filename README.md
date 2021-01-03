# IgH EtherCAT master with EasyCAT slave example

**Files:**
    ethercat.tar.bz2.00  - IgH EtherCAT master, part 1
    ethercat.tar.bz2.01  - IgH EtherCAT master, part 2
    main.c               - IgH EtherCAT master modifyed example for EasyCAT slave, 
                                 overwrite with main.c file in /ethercat/examples/dc_user and compile there with "make"
                           
Combine IgH EtherCAT master source code to one file with 
    cat ethercat.tar.bz2.?? > ethercat.tar.bz2

Unpack IgH EtherCAT master source code with
    tar -xvjf ethercat.tar.bz2

Use ./configure --disable-8139too instead of ./configure
