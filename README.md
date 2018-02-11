# ExpLoRerD2D

Very simple end-to-end application demonstrating LoRa device-to-device communication.

# Functional specifications

A remote device reads local temperature, and transmits it to a central application around every 30 seconds.

The central application displays received temperature values. 

The user can click on a button, to send a command to the remote device, requesting it to turn a LED on and set it to red, green or blue color, or to turn it off, depending on the command.

# Architecture

## Global architecture

A [SODAQ ExpLoRer board](http://support.sodaq.com/sodaq-one/explorer/) is used as the remote device. It is configured in LoRa device-to-device mode.

Another SODAQ ExpLoRer board is used at central application side, as a LoRa interface with the application.

The application is a [JavaFX](http://www.oracle.com/technetwork/java/javase/overview/javafx-overview-2158620.html) application. Communication with the board is performed using the serial-over-USB link.

## Devices

Software environment is the [Arduino](https://www.arduino.cc/) one.

The LoRa module is used in point-to-point mode, i.e. LoRaWAN stack is not used.

The RN2483 mode is used in a very simple way:

* it is set in reception mode for a given period
* when it exits from reception mode, it is able to transmit a message. It does so, if a message has to be transmitted. Otherwise, it goes back into reception mode.

More precisely:

* the radio watchdog is set to a predefined period.
* reception mode is entered.
* the module will exit reception mode on two events:
  * end of watchdog period
  * reception of a message 

### Remote device

The Arduino main loop performs following actions:

* if the board receives a message from the central device, it reads it, and performs related action, i.e. sets LED color
* when it exits from reception state without having received a message, it reads the temperature from the on-board sensor, and requests its transmission to central device

### Central device

The Arduino main loop performs following actions:

* if the board receives a message from the remote device, it transfers it to the central application using the serial-over-USB link
* when the board exits from reception state, it checks whether it received a message from the central application. If it received one, it transmits it to the remote device. There is no message buffering. The board transmits first received message only.

### Messages

#### LED commands

Messages containing commands sent from central device to remote device, to control the RGB LED, adhere to following format:

* a byte containing ASCII code of 'L' letter followed by a byte containing one of the following values:
  * ASCII code of 'R' to set the LED to red color
  * ASCII code of 'G' to set the LED to green color
  * ASCII code of 'B' to set the LED to blue color
  * 0 to set the LED off

ASCII code for 'L' is 4C.

#### Temperature

Temperature sent by remote device is an integer value on 2 bytes, in big endian endianness. The value is the value returned by the ADC. According to sensor data sheet, it has to be transformed in the following way, to get the real temperature:

```
//10mV per C, 0C is 500mV
temperature = (value * 3300 / 1023 - 500) / 10.
```

Formula is slightly modified in order to get temperature values matching real temperature. The new formula has been checked around 20 degrees (Celsius) only.

The value is sent in a message with the first byte set to ASCII code of 'T' letter: 54.

For instance, the message for a voltage value of 244 is sent as the following three bytes:

``` 
5400F4
```

#### Messages from central device to application

In order to be able to multiplex trace messages and application messages, following message format is defined: a first byte containing the type of message, then the message, then CR.

Possible values for type are: 'T' for trace message, and 'A' for application message.

#### Messages from application to central device

The application sends messages to the central device as they should be sent to the remote device.

# Installation and configuration

## Arduino environment

Install and configure the Arduino IDE as described [here](http://support.sodaq.com/sodaq-one/explorer/).

## Central application environment

### Linux

Install e(fx)clipse and configure it as described [here](http://www.monblocnotes.com/node/2274).

Install RXTX as described [here](http://www.monblocnotes.com/node/2153). The part relating to project configuration should not be required, if you import the project as described below.

### Other OS

Install VirtualBox, then create a Linux virtual machine, and refer to above section :-)

More seriously, you should easily find information for your preferred OS.

## Application build

### Device

Using the Arduino IDE, compile <tt>remote.ino</tt> source code, and upload it to the board that will be used as the remote device.

Then, compile <tt>central.ino</tt> source code, and upload it to the board that will be used as the central device.

### Central application

Import <tt>pc/CentralApp/</tt> contents as a new project in e(fx)clipse.

Run the application.

## Operation

### From e(fx)clipse

Once the application is started, select the serial port device connected to the central device.

Click on the buttons to set color of remote device LED.

### Using a jar file

To generate a jar file containing the application, use **Export... / Java / Runnable JAR file**.

An existing jar file is already provided in the repository, in <tt>CentralApp</tt> directory.

To run the jar file, use the following command:

```
java -Djava.library.path=/usr/lib/jni -jar centralapp.jar
```

# Limitations

This sample system is meant to be used as a demonstrator only. For a real production system, following items, among others, should be addressed:

* ensure that radio transmissions comply with local regulation
* handle errors resulting from calls to RN2483 API in the right way
* add identification of remote devices
* increase reliability (ack, repetitions)
* add security:
  * authentication
  * integrity
* increase data accuracy if required (e.g. time-stamps)
* increase autonomy for remote device, if required
* etc.