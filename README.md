
# sleepmemtester
### What is sleepmemtester?
A tiny utilities for system sleep/resume memory verification. sleepmemtester is mainly for system memory verification after resume from sleep state. We always meet many memory problems when system resume from sleep state, espicailly in system board bring up/firmware development phase. sleepmemtester can help to check where the memory data corrupted. 
### How does sleepmemtester work?
SleepMemtester allocates a part of the system memory, populates the test value (default 0xFF), setting the RTC timer for system wake, and then suspend the system to the corresponding sleep type. When the RTC timer is timeout, the system will automatically wake, and the sleepmemtester process will continue to verify if the data in memory changed.

sleepmemtester is a user space program, the test can cover as much as user space free memory. sleepmemtester use mlock to prevent the memory under test from being paged to the swap area.
### Usage
sleepmemtester provide the option to set sleep type, sleep time, wakeup time, and test value (The special value write to memory for verification).

The command is likely to be

    ./sleepmemtester -m mem -t 60 -s 40 14G 10
  
  Above sample command will start a 14G system memory test. The sleep type is suspend to ram (S3). And system power state change is S0 ==> Sleep (S3) == 40s ==> wake (S0) ==> 60s ==> sleep (S3) ==> ... ==> test end. The total test loops is 10.
  
  Following is a simple introduction for the options.
  * -m [mem | disk]   is used to choose the sleep type. mem is for suspend to ram, aka ACPI S3. disk is for hibernate, aka ACPI S4. 
  * -t seconds        is used to choose the wakeup time. It will determine the system wakeup time before fall into sleep.
  * -s seconds        is used to set the sleep time. It will setup wakeup timer, which will wake the system after timer timeout.
  * mem[B|K|M|G]      is used to set the memory size to be tested. 
  * [loops]           is used to set test loops.
  
### Build
Need install gcc and make. 

### Background
sleepmemtester is developed based on https://github.com/jnavila/memtester. Since the purpose is not for memory stability test, I removed the original test cases and write a simple test program for system sleep/resume memory test. 
