# Macho
## A simple interface to IBM Z MACHine instructions and z/OS System MacrOs
Macho is a library of callable services named after, and providing an interface to invoke, various IBM Z machine instructions and z/OS system macros.
## Purpose
The primary purpose of Macho is education.  This library makes chosen Z machine instructions and system macros accessible to high-level languages (e.g. COBOL, PL/I, etc) and script languages (e.g. REXX, Python).  For machine instructions, the interface provided is a mirror of the description of the corresponding instruction in the [IBM z/Architecture Principles of Operation](https://publibfp.dhe.ibm.com/epubs/pdf/a227832d.pdf), affectionately known as the "POP".  For system macros, the interface provided by Macho attempts to expose the full functionality, but the interface may not closely resemble the macro syntax, for practical reasons.

More instructions and macros will be added to Macho over time.  The intent is to expose the interesting, powerful and complex features of the Z environment, that would otherwise be very difficult to access directly from a high-level language.

If you want a particular instruction or macro to be added, raise an issue and I will take a look at adding it.
## Installation
Copy each of the *.xmit files to MVS data sets with RECFM=FB, LRECL=80.  Make sure no translation of the file contents occurs - these are BINARY files.

Use the TSO RECEIVE command to reconstitute each of the libraries.  The naming of these libraries is up to you, but in the instructions below, they are named as follows:
|Library|Purpose|
|--|--|
|MACHO.PDSE|31-bit library functions|
|MACHO.PDSE64|64-bit library functions|
|MACHO.AUTH|APF-authorised library to enable privileged operations|
|MISSING.MACLIB|Only required if building Macho from source|
### Privileged Operations
Some instructions and macros require the caller to be in supervisor state (e.g. the PCKMO machine instruction).  Macho provides a stacked PC routine for this purpose.  You can read all about this routine and supporting components in the article, [Writing A Stacking PC Routine in Metal C](https://www.linkedin.com/pulse/writing-stacking-pc-routine-metal-c-andrew-mattingly-tvupc/).

The installation of the PC routine requires these steps:

 - APF-authorise the MACHO.AUTH library, or copy the modules contained therein to an APF-authorised library.
 - Create a started task procedure for the MACPCREG module, which loads the PC routine into z/OS.
 - Create a RACF profile for each privileged instruction/macro provided by Macho, and authorise users to execute these instructions/macros.

The started task procedure should look something like this:

    //MACPCREG PROC
    //STEP01   EXEC PGM=MACPCREG,REGION=4M
    //STEPLIB  DD   DISP=SHR,DSN=MACHO.AUTH
    //SYSPRINT DD   SYSOUT=*
 
For example, to authorise use of the PCKMO instruction for a user called FRED, you would issue the following TSO commands to RACF:

    RDEFINE FACILITY MACHO.PCKMO UACC(NONE)
    PERMIT MACHO.PCKMO CLASS(FACILITY) ID(FRED) ACCESS(READ)
    SETROPTS RACLIST(FACILITY) REFRESH

The MACPCREG started task must be running in order for privileged Macho routines to successfully execute.  Accordingly, consider updating the automation for your system to start this task at IPL.  It has no dependencies on other system components.
## Using Macho
The [tests](https://github.com/admattingly/Macho/tree/main/tests) directory contains several examples of how to call Macho, and you are welcome/encouraged to use these examples as a guide.

For IBM Z machine instructions, the process you should go through is as follows:

 - Check the [src](https://github.com/admattingly/Macho/tree/main/src) directory to see if the instruction is implemented, and take a look at the parameter list for the Macho routine - it will have some extra parameters, over and above the registers and /or addresses required by the machine instruction.
 - Check the [tests](https://github.com/admattingly/Macho/tree/main/tests) directory to see if an example exists for your target instruction or something like it.
 - Find the description of the machine instruction in the [IBM z/Architecture Principles of Operation](https://publibfp.dhe.ibm.com/epubs/pdf/a227832d.pdf).  This is your **documentation** for the Macho routine.
 - Write and test your application, making sure the MACHO.PDSE library (or MACHO.PDSE64 library for a 64-bit application) is in your STEPLIB.
