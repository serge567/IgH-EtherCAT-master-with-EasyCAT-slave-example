/* 2021-01-03
 IgH EtherCAT master modifyed example for EasyCAT slave
 overwrite with this file in /ethercat/examples/dc_user
 and compile there with "make"
*/
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/mman.h>
#include <malloc.h>
#include <sched.h> /* sched_setscheduler() */
/****************************************************************************/
#include "ecrt.h"
/****************************************************************************/
// Application parameters
#define FREQUENCY 100 //Hz
#define CLOCK_TO_USE CLOCK_MONOTONIC
/****************************************************************************/
#define NSEC_PER_SEC (1000000000L) 
#define PERIOD_NS (NSEC_PER_SEC / FREQUENCY)
#define TIMESPEC2NS(T) ((uint64_t) (T).tv_sec * NSEC_PER_SEC + (T).tv_nsec)
/****************************************************************************/
// EtherCAT
static ec_master_t *master = NULL;
static ec_master_state_t master_state = {};
static ec_domain_t *domain1 = NULL;
static ec_domain_state_t domain1_state = {};
/****************************************************************************/
// process data
static uint8_t *domain1_pd = NULL;
// slave const
#define EasyCATPos	     		  0, 0			// Alias, Position
#define EasyCAT  		 0x0000079a, 0x00defede // Vendor ID, Product ID (EasyCAT.XML file)
// offsets for PDO entries
static int offset_in;
static int offset_out;
// counters, cycle time
static unsigned int counter = 0;
static unsigned int sync_ref_counter = 0;
const struct timespec cycletime = {0, PERIOD_NS};
/*****************************************************************************/
struct timespec timespec_add(struct timespec time1, struct timespec time2)
{   struct timespec result;
    if ((time1.tv_nsec + time2.tv_nsec) >= NSEC_PER_SEC) 
    {   result.tv_sec = time1.tv_sec + time2.tv_sec + 1;
        result.tv_nsec = time1.tv_nsec + time2.tv_nsec - NSEC_PER_SEC;
    } else 
    {   result.tv_sec = time1.tv_sec + time2.tv_sec;
        result.tv_nsec = time1.tv_nsec + time2.tv_nsec;
    }
    return result;
}
/*****************************************************************************/
void check_domain1_state(void)
{
    ec_domain_state_t ds;
    ecrt_domain_state(domain1, &ds);
    if (ds.working_counter != domain1_state.working_counter)
        printf("Domain1: WC %u.\n", ds.working_counter);
    if (ds.wc_state != domain1_state.wc_state)
        printf("Domain1: State %u.\n", ds.wc_state);
    domain1_state = ds;
}
/*****************************************************************************/
void check_master_state(void)
{
    ec_master_state_t ms;
    ecrt_master_state(master, &ms);
    if (ms.slaves_responding != master_state.slaves_responding)
        printf("%u slave(s).\n", ms.slaves_responding);
    if (ms.al_states != master_state.al_states)
        printf("AL states: 0x%02X.\n", ms.al_states);
    if (ms.link_up != master_state.link_up)
        printf("Link is %s.\n", ms.link_up ? "up" : "down");
    master_state = ms;
}
/****************************************************************************/
void write_pdo_files()
{
	int input;
	for (input = 0; input < 32; ++input) // 0..31 inpits for EasyCAT
	{	FILE *fp;
		char sfilename[255] = "/run/ethercat/EasyCAT/pdoin";
		char istr[5];
		sprintf(istr, "%d", input);
		strncat (sfilename, istr, 5);
		char rstr[5] = "000";
		if ((fp = fopen(sfilename, "r"))!=NULL)
		{	if ((fgets(rstr, 5, (FILE*)fp))==NULL)
			{ printf("Couldn't read %s file.\n", sfilename);}
			fclose(fp);
		}
		char wstr[5];
		sprintf (wstr, "%d", EC_READ_U8(domain1_pd + offset_in + input));		
		if(strncmp(rstr, wstr, 5) != 0)
		{	fp = fopen(sfilename, "w+");
			fputs(wstr, fp);
			fclose(fp);
		}
	}
}
/****************************************************************************/
void read_pdo_files()
{
	int output;	
	for (output = 0; output < 32; ++output) // 0..31 outputs for EasyCAT
	{	FILE *fp;
		int iCFile = 0;
		char sfilename[255] = "/run/ethercat/EasyCAT/pdoout";
		char istr[5];
		sprintf(istr, "%d", output);
		strncat (sfilename, istr, 5);
		char rstr[5];
		if ((fp = fopen(sfilename, "r"))!=NULL)
		{	if ((fgets(rstr, 5, (FILE*)fp))!=NULL)	
			{	uint8_t wbyte  = atoi (rstr); 
				EC_WRITE_U8(domain1_pd + offset_out + output, wbyte);
			}
			else {iCFile = -1;}			
			fclose(fp);
		}
		else {iCFile = -1;}
		if (iCFile != 0)
		{	char wstr[5];
			sprintf (wstr, "%d", EC_READ_U8(domain1_pd + offset_out + output));
			fp = fopen(sfilename, "w+");
			fputs(wstr, fp);
			fclose(fp);
		}			
	}
}
/****************************************************************************/
void cyclic_task()
{
    struct timespec wakeupTime, time;   
    // get current time
    clock_gettime(CLOCK_TO_USE, &wakeupTime);
    while(1) 
    {	wakeupTime = timespec_add(wakeupTime, cycletime);
        clock_nanosleep(CLOCK_TO_USE, TIMER_ABSTIME, &wakeupTime, NULL);
        // Write application time to master
        // It is a good idea to use the target time (not the measured time) as
        // application time, because it is more stable.
        ecrt_master_application_time(master, TIMESPEC2NS(wakeupTime));
        // receive process data
        ecrt_master_receive(master);
        ecrt_domain_process(domain1);
        // check process data state (optional)
        check_domain1_state();
        if (counter) 
        { counter--;} 
        else // do this at 1 Hz
        {   counter = FREQUENCY;
            // check for master state (optional)
            check_master_state();
        }
        // write process data
		write_pdo_files();
		read_pdo_files();
        if (sync_ref_counter) 
        { sync_ref_counter--; } 
        else 
        {   sync_ref_counter = 1; // sync every cycle
            clock_gettime(CLOCK_TO_USE, &time);
            ecrt_master_sync_reference_clock_to(master, TIMESPEC2NS(time));
        }
        ecrt_master_sync_slave_clocks(master);
        // send process data
        ecrt_domain_queue(domain1);
        ecrt_master_send(master);
    }
}
/****************************************************************************/
int main(int argc, char **argv)
{
    ec_slave_config_t *sc;
    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) 
    {   perror("mlockall failed");
        return -1;
    }
    master = ecrt_request_master(0);
    if (!master)  return -1;
    domain1 = ecrt_master_create_domain(master);
    if (!domain1) return -1;
    // Create configuration for bus coupler
    if (!(sc = ecrt_master_slave_config(master, EasyCATPos, EasyCAT))) 
    {   fprintf(stderr, "Failed to get slave configuration.\n");
        return -1;
    }  
	/*	./ethercat pdos
		SM1: PhysAddr 0x1200, DefaultSize    0, ControlRegister 0x20, Enable 1 TxPDO 0x1a00 "Inputs"
			PDO entry 0x0006:01,  8 bit, "Byte0"
			PDO entry 0x0006:02,  8 bit, "Byte1"
			:::::::::::::::::::::::::::::::::::
			PDO entry 0x0006:1f,  8 bit, "Byte30"
			PDO entry 0x0006:20,  8 bit, "Byte31"  */   
    offset_in = ecrt_slave_config_reg_pdo_entry(sc, 0x0006, 0x01, domain1, NULL);
    if (offset_in < 0)  return -1;
	/*	./ethercat pdos
		SM0: PhysAddr 0x1000, DefaultSize    0, ControlRegister 0x64, Enable 1 RxPDO 0x1600 "Outputs"
			PDO entry 0x0005:01,  8 bit, "Byte0"
			PDO entry 0x0005:02,  8 bit, "Byte1"
			:::::::::::::::::::::::::::::::::::
			PDO entry 0x0005:1f,  8 bit, "Byte30"
			PDO entry 0x0005:20,  8 bit, "Byte31"  */  
    offset_out = ecrt_slave_config_reg_pdo_entry(sc, 0x0005, 0x01, domain1, NULL);
    if (offset_out < 0) return -1;
    printf("Activating master...\n");
    if (ecrt_master_activate(master)) return -1;
    if (!(domain1_pd = ecrt_domain_data(domain1))) 
    {  return -1;}
    /* Set priority */
    struct sched_param param = {};
    param.sched_priority = sched_get_priority_max(SCHED_FIFO);
    printf("Using priority %i.", param.sched_priority);
    if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) 
    {   perror("sched_setscheduler failed");}
    if (system("mkdir -p /run/ethercat/EasyCAT") == -1)
    {	printf("Couldn't create /run/ethercat/EasyCAT \n");
		return -1;
	}    
    printf("Starting cyclic function.\n");
    cyclic_task();
    return 0;
}
/****************************************************************************/
