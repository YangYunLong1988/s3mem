
# s3mem
### What is s3mem?
A tiny utilities for system sleep/resume memory verification. s3mem is mainly for system memory verification after resume from sleep state. We always meet many memory problems when system resume from sleep state, espicailly in system board bring up/firmware development phase. s3mem can help to check where the memory data corrupted.
### How does s3mem work?
s3mem allocates a part of the system memory, populates the test value (default 0xFF), setting the RTC timer for system wake, and then suspend the system to the corresponding sleep type. When the RTC timer is timeout, the system will automatically wake, and the s3mem process will continue to verify if the data in memory changed.

s3mem is a user space program, the test can cover as much as user space free memory. s3mem use mlock to prevent the memory under test from being paged to the swap area.
### Usage
s3mem provide the option to set sleep type, sleep time, wakeup time, and test value (The special value write to memory for verification).

The command is likely to be

    ./s3mem -t 60 -s 40 14G 10

  Above sample command will start a 14G system memory test. The sleep type is suspend to ram (S3). And system power state change is S0 ==> Sleep (S3) == 40s ==> wake (S0) ==> 60s ==> sleep (S3) ==> ... ==> test end. The total test loops is 10.

  Following is a simple introduction for the options.
  * -t seconds        is used to choose the wakeup time. It will determine the system wakeup time before fall into sleep.
  * -s seconds        is used to set the sleep time. It will setup wakeup timer, which will wake the system after timer timeout.
  * -v testValue      is used to set the test value which will used to fill the memory and tested after system resume from sleep state.
  * -j logfile        is used to specific the test log file path.
  * mem[B|K|M|G]      is used to set the memory size to be tested.
  * [loops]           is used to set test loops.

### Build
For Unix-like system build. cmake, make and gcc is needed.
1. Execute "cmake ." from root directory of the source code. If you need static build, append "-DSTATIC_LINK=1" to config static build.
2. Execute "make".

For windows system build. cmake, MSVC build environment is needed.

Visual Studio:
1. Execute cmake from root directory of the source code. Both cmake-gui and cli are supported.
2, Open s3mem.sln by VS IDE.
3, Build project.

NMake
1. Execute cmake from root directory of the source code. Both cmake-gui and cli are supported.
2, Execute "nmake".
Note that you must make sure the cmake execution environment already enable the windows SDK and any of one c compiler(MSVC/EWDK/etc.). Or the cmake will terminate because of failed to build test program. msvcenv.ps1 is a sample script to set up windows sdk and Visual Studio environment for cmake running. Since the Visual studio and windows SDK have so many versions that have different path, I have no plan to support all the history VS/Windows SDK version currently.  If you want to use nmake to build the source code, you can reference the msvcenv.ps1 and make a scripts for your own.
Or you can try to skip the compiler check of the cmake before execution.

### Background
s3mem is developed based on https://github.com/jnavila/memtester. Since the purpose is not for memory stability test, I removed the original test cases and write a simple test program for system sleep/resume memory test.
In order to make the code structure have better platform compatibility, I refactored the code framework based on the original code. And s3mem can support windows platform now.
