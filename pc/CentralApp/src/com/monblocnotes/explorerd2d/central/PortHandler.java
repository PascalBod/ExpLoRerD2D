/**
 * MIT License
 *
 * Copyright (c) 2018 Pascal Bodin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
package com.monblocnotes.explorerd2d.central;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.TooManyListenersException;

import gnu.io.CommPortIdentifier;
import gnu.io.NoSuchPortException;
import gnu.io.PortInUseException;
import gnu.io.SerialPort;
import gnu.io.SerialPortEvent;
import gnu.io.SerialPortEventListener;
import gnu.io.UnsupportedCommOperationException;

/**
 *
 * Handles serial port.
 *
 */
public class PortHandler implements SerialPortEventListener {

	private final static int PORT_OPEN_WAIT = 1000;
	// Characteristics of Sodaq ExpLoRer serial port (over USB).
	private final static int PORT_SPEED = 57600;
	private final static int PORT_DATA_BITS = SerialPort.DATABITS_8;
	private final static int PORT_STOP_BITS = SerialPort.STOPBITS_1;
	private final static int PORT_PARITY = SerialPort.PARITY_NONE;
	private final static int PORT_FLOW_CONTROL = SerialPort.FLOWCONTROL_NONE;

	private SerialPort serialPort;
	private OutputStream out;
	private InputStream in;
	private DisplayMessage displayMessage;
	private FrameHandler frameHandler;

	/**
	 *
	 * @param displayMessage
	 */
	public PortHandler(DisplayMessage displayMessage) {

		this.displayMessage = displayMessage;
		frameHandler = new FrameHandler(displayMessage);

	}

	/**
	 *
	 */
	public short[] createSetRemoteLedRMsg() {

		return frameHandler.createSetRemoteLedRMsg();

	}

	/**
	 *
	 */
	public short[] createSetRemoteLedGMsg() {

		return frameHandler.createSetRemoteLedGMsg();

	}

	/**
	 *
	 */
	public short[] createSetRemoteLedBMsg() {

		return frameHandler.createSetRemoteLedBMsg();

	}

	/**
	 *
	 */
	public short[] createSetRemoteLedOffMsg() {

		return frameHandler.createSetRemoteLedOffMsg();

	}

	/**
	 *
	 */
	public ArrayList<String> getSerialPorts() {

		ArrayList<String> portNameList = new ArrayList<String>();
		@SuppressWarnings("unchecked")
		Enumeration<CommPortIdentifier> portList = CommPortIdentifier.getPortIdentifiers();
		if (portList == null) {
			displayMessage.displayLog("no CommPortIdentifier!");
			return null;
		}
		CommPortIdentifier port;
		while (portList.hasMoreElements()) {
			port = portList.nextElement();
			if (port.getPortType() != CommPortIdentifier.PORT_SERIAL) {
				displayMessage.displayLog(port.getName() + " not a serial port");
				continue;
			}
			if (port.isCurrentlyOwned()) {
				displayMessage.displayLog(port.getName() + " currently owned");
				continue;
			}
			portNameList.add(port.getName());
			displayMessage.displayLog(port.getName() + " added to list");
		}
		if (portNameList.isEmpty()) {
			return null;
		}

		return portNameList;
	}

	/**
	 * Tries to open the serial port. Side effect:
	 * - set serialPort
	 * - set out
	 * In current version, can be called only once (i.e. serial port is never closed).
	 * @param serialPortName
	 * @return -1 - no such port
	 *         -2 - port in use
	 *         -3 - internal error
	 *         -4 - too many event listeners
	 *         -5 - can't get output stream
	 *         -6 - can't get input stream
	 *          0 - success
	 */
	public int setSerialPort(String serialPortName) {

		CommPortIdentifier commPortIdentifier;
		try {
			commPortIdentifier = CommPortIdentifier.getPortIdentifier(serialPortName);
		} catch (NoSuchPortException e) {
			displayMessage.displayLog(serialPortName + " does not exist");
			return -1;
		}
		try {
			serialPort = (SerialPort) commPortIdentifier.open("FrameHandler", PORT_OPEN_WAIT);
		} catch (PortInUseException e) {
			displayMessage.displayLog(serialPortName + " is in use");
			return -2;
		}
		try {
			serialPort.setSerialPortParams(PORT_SPEED, PORT_DATA_BITS, PORT_STOP_BITS, PORT_PARITY);
		} catch (UnsupportedCommOperationException e) {
			displayMessage.displayLog("internal error: bad port profile");
			return -3;
		}
		try {
			serialPort.setFlowControlMode(PORT_FLOW_CONTROL);
		} catch (UnsupportedCommOperationException e) {
			displayMessage.displayLog("internal error: bad flow control");
			return -3;
		}
		try {
			out = serialPort.getOutputStream();
		} catch (IOException e) {
			displayMessage.displayLog("can't get output stream");
			return -5;
		}
		try {
			in = serialPort.getInputStream();
		} catch (IOException e) {
			displayMessage.displayLog("can't get input stream");
			return -6;
		}
		try {
			serialPort.addEventListener(this);
			serialPort.notifyOnDataAvailable(true);
		} catch (TooManyListenersException e) {
			displayMessage.displayLog("too many event listeners");
			return -4;
		}
		displayMessage.displayLog(serialPortName + " opened and configured");
		return 0;
	}

	/**
	 * Event notification must be enabled for every event in setSerialPort() above.
	 * We can't call displayMessage.display() from this method, as we are not in FX thread.
	 *
	 * @param event
	 */
	@Override
	public void serialEvent(SerialPortEvent event) {

		switch(event.getEventType()) {
		case SerialPortEvent.BI:
		case SerialPortEvent.CD:
		case SerialPortEvent.CTS:
		case SerialPortEvent.DSR:
		case SerialPortEvent.FE:
		case SerialPortEvent.OE:
		case SerialPortEvent.OUTPUT_BUFFER_EMPTY:
		case SerialPortEvent.PE:
		case SerialPortEvent.RI:
			break;
		case SerialPortEvent.DATA_AVAILABLE:
			short[] frame;
			int b;
			while (true) {
				try {
					if (in.available() == 0) {
						// No more data available.
						break;
					}
					b = in.read();
				} catch (IOException e) {
					displayMessage.displayLogLater("error on receive: " + e.getMessage());
					break;
				}
				frame = frameHandler.frameAssembler(b);
				if (frame != null) {
					frameHandler.processFrame(frame);
				}
			}
			break;
		default:
			displayMessage.displayLogLater("FrameHandler.serialEvent() - unknown event type: " +
					event.getEventType());
		}

	}

	/**
	 *
	 * @param bytes
	 */
	public void sendMsg(short[] bytes) {

		// To allow for tests while building the application.
		if (bytes == null) {
			displayMessage.displayLogLater("can't send null message");
			return;
		}

		try {
			for (short b: bytes) {
				out.write(b);
			}
			out.write(FrameHandler.CR);
			out.write(FrameHandler.LF);
		} catch (IOException e) {
			displayMessage.displayLogLater("write error: " + e.getMessage());
			return;
		}

	}

}
