/*
Usage: ./send systemcode unitcode command [protocol] [pulselength]
systemcode  - string representation of DIP switches, e.g. "00100"
unitcode    - string representation of units (A..E) to command, e.g. "10000" (A)
command     - "on" or "off", turns the switches on or off
protocol    - According to rc-switch definitions
pulselength - pulselength in microseconds
*/

#include "../rc-switch/RCSwitch.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    
    /* If the number of arguments are incorrect, print the help text and exit */
    if (argc < 4 || argc > 6) {

        printf("Usage: %s <systemCode> <unitCode> <command> [protocol] [pulseLength]\n", argv[0]);
        printf("Sends a tri-state code (e.g. 433Mhz protocol) on a GPIO.\n");
        printf("\n");
        printf("systemCode\t-  DIP switch settings, e.g. \"00100\"\n");
        printf("unitCode\t- switch number [1..5] or pin pattern corresponding to the buttons pressed [10000..00001]\n");
        printf("command\t - turns the specified switches on or off, 0 for off or 1 for on\n");
        printf("protocol\t- optional. according to rc-switch definitions\n");
        printf("pulseLength\t- optional. pulselength in microseconds\n");
        printf("\n");
        printf("Examples:\n");
        printf("\t%s 00100 10000 0\n\t- Turns switch 1/A off", argv[0]);
        printf("\t%s 00100 00010 1 2 150\n\t- Turns switch 4/D on", argv[0]);
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
    const int  command  = atoi(argv[3]);
    int protocol = 0; // A value of 0 will use rc-switch's default value
    int pulseLength = 0;
    
    // Change protocol and pulse length according to parameters
    if (argc >= 5) protocol = atoi(argv[4]);
    if (argc >= 6) pulseLength = atoi(argv[5]);

    printf("sending systemCode[%s] unitCode[%s] command[%s]\n", systemCode, unitCode, command);
    RCSwitch mySwitch = RCSwitch();
    if (protocol != 0) mySwitch.setProtocol(protocol);
    if (pulseLength != 0) mySwitch.setPulseLength(pulseLength);
    mySwitch.enableTransmit(PIN);
    
    if (command) {
        mySwitch.switchOn(systemCode, unitCode);
	} else {
		mySwitch.switchOff(systemCode, unitCode);
    }
    return 0;
}
