/*
Usage: ./send <systemCode> <unitCode> <command> [protocol] [pulseLength]
Sends a tri-state code (e.g. 433Mhz protocol) on a GPIO.

systemCode  -  DIP switch settings, e.g. "00100"
unitCode    - switch number [1..5] or pin pattern corresponding to the buttons pressed [10000..00001]
command     - turns the specified switches on or off, 0 for off or 1 for on
protocol    - optional. according to rc-switch definitions
pulseLength - optional. pulselength in microseconds

Examples:
    ./send 00100 10000 0          - Turns switch 1/A off
    ./send 00100 00010 1 2 150    - Turns switch 4/D on
*/
#include "../rc-switch/RCSwitch.h"
#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <unistd.h>
#include <sys/file.h>

int main(int argc, char *argv[]) {
    
    /* If the number of arguments are incorrect, print the help text and exit */
    if (argc < 4 || argc > 6) {

        printf("Usage: %s <systemCode> <unitCode> <command> [protocol] [pulseLength]\n", argv[0]);
        printf("Sends a tri-state code (e.g. 433Mhz protocol) on a GPIO.\n");
        printf("\n");
        printf("systemCode  -  DIP switch settings, e.g. \"00100\"\n");
        printf("unitCode    - switch number [1..5] or pin pattern corresponding to the buttons pressed [10000..00001]\n");
        printf("command     - turns the specified switches on or off, 0 for off or 1 for on\n");
        printf("protocol    - optional. according to rc-switch definitions\n");
        printf("pulseLength - optional. pulselength in microseconds\n");
        printf("\n");
        printf("Examples:\n");
        printf("    %s 00100 10000 0          - Turns switch 1/A off\n", argv[0]);
        printf("    %s 00100 00010 1 2 150    - Turns switch 4/D on\n", argv[0]);
        return -1;
    }

    /* Check whether wiringPI is available, no idea otherwise */
    if (wiringPiSetup () == -1) {
        printf("error: wiringPI is missing\n");
        return 1;
    }

    /*
     output PIN is hardcoded for testing purposes
     see https://projects.drogon.net/raspberry-pi/wiringpi/pins/
     for pin mapping of the raspberry pi GPIO connector
     */
    int PIN = 0;

    const char* code[6] = { "00000", "10000", "01000", "00100", "00010", "00001" };
    
    const char* systemCode = argv[1];
    const char* unitCode;
    if (strlen(argv[2]) == 5) {
        unitCode = argv[2];
    } else if (atoi(argv[2]) > 0 and atoi(argv[2]) < 6) {
        unitCode = code[atoi(argv[2])];
    } else {
        printf("error: unitCode[%s] is unsupported\n", argv[2]);
        return -1;
    }
    const int command  = atoi(argv[3]);
    int protocol = 0; // A value of 0 will use rc-switch's default value
    int pulseLength = 0;
    
    // Change protocol and pulse length according to parameters
    if (argc >= 5) protocol = atoi(argv[4]);
    if (argc >= 6) pulseLength = atoi(argv[5]);

    printf("sending systemCode[%s] unitCode[%s] command[%i]\n", systemCode, unitCode, command);
    RCSwitch mySwitch = RCSwitch();
    if (protocol != 0) mySwitch.setProtocol(protocol);
    if (pulseLength != 0) mySwitch.setPulseLength(pulseLength);
    mySwitch.enableTransmit(PIN);
    
    // Obtain advisory lock (exclusive), flock will block until successful
    FILE *lockfile = fopen("/var/lock/gpio-send.lock", "w+");
	int res = flock(fileno(lockfile), LOCK_EX);

    // Send the command
    if (command == 1) {
        mySwitch.switchOn(systemCode, unitCode);
    } else if (command == 0) {
        mySwitch.switchOff(systemCode, unitCode);
    } else {
        printf("error: command[%i] is unsupported\n", command);
        return -1;
    }

    // Close the lock file (not sure this is needed..)
    fclose(lockfile);
    return 0;
}
