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

import java.text.DecimalFormat;
import java.util.Arrays;

/**
 *
 * Beware: no flow control!
 *
 */
public class FrameHandler {

	public final static short CR  = 0x0D;
	public final static short LF  = 0x0A;
	private final static short FIGURE_0 = (short)'0';
	private final static short LETTER_B = (short)'B';
	private final static short LETTER_G = (short)'G';
	private final static short LETTER_L = (short)'L';
	private final static short LETTER_R = (short)'R';

	// States of frame assembly automaton.
	private static enum AssemblyStates {
		WAIT_CR, WAIT_LF
	};

	private AssemblyStates currentAssemblyState;

	private final static int MAX_LENGTH_PAYLOAD = 40;
	private short[] recBuffer;
	private int recBufferIndex;

	private DisplayMessage displayMessage;

	private DecimalFormat decimalFormat;

	/**
	 *
	 * @param displayMessage
	 */
	public FrameHandler(DisplayMessage displayMessage) {

		this.displayMessage = displayMessage;

		currentAssemblyState = AssemblyStates.WAIT_CR;
		recBuffer = new short[MAX_LENGTH_PAYLOAD];
		recBufferIndex = 0;

		decimalFormat = new DecimalFormat("#0.0");

	}

	/**
	 * Assembles a new frame using byte passed in input parameter. Returns frame when a
	 * whole frame is assembled, null otherwise.
	 *
	 * We use a finite state automaton, even if we could have simply waited for the
	 * final LF. Thus, it would be easy to adapt to more complex frame formats.
	 *
	 * @param b byte to be processed. If -1, assembly processing is reset.
	 * @return null until a new frame is received, otherwise frame
	 */
	public short[] frameAssembler(int b) {

		if (b == -1) {
			displayMessage.displayLogLater("-1 passed to frameAssembler()");
			currentAssemblyState = AssemblyStates.WAIT_CR;
			recBufferIndex = 0;
			return null;
		}

		short[] payload = null;

		switch (currentAssemblyState) {
		case WAIT_CR:
			if (b == CR) {
				currentAssemblyState = AssemblyStates.WAIT_LF;
				break;
			}
			recBuffer[recBufferIndex] = (short)b;
			recBufferIndex++;
			// Stay in same state.
			break;
		case WAIT_LF:
			if (b != LF) {
				// Error. Reset assembly.
				displayMessage.displayLogLater("!= LF received");
				currentAssemblyState = AssemblyStates.WAIT_CR;
				recBufferIndex = 0;
				break;
			}
			// At this stage, LF received.
			payload = Arrays.copyOfRange(recBuffer, 0, recBufferIndex);
			currentAssemblyState = AssemblyStates.WAIT_CR;
			recBufferIndex = 0;
			break;
		default:
			displayMessage.displayLogLater("unknown state for frame assembler");
			currentAssemblyState = AssemblyStates.WAIT_CR;
			recBufferIndex = 0;
		}

		return payload;

	}

	/**
	 *
	 * @param frame
	 */
	public void processFrame(short frame[]) {

		switch(frame[0]) {
		case 'T':
			// Trace frame.
			displayMessage.displayFrameLater(frameToString(frame));
			break;
		case 'A':
			// Application frame.
			displayMessage.displayFrameLater(frameToString(frame));
			if (frame[1] == '5' && frame[2] == '4') {
				// Temperature. Extract and display it.
				if (frame.length < 7) {
					displayMessage.displayLogLater("temperature frame too short");
					break;
				}
				short msbs[] = new short[2];
				msbs[0] = frame[3];
				msbs[1] = frame[4];
				short lsbs[] = new short[2];
				lsbs[0] = frame[5];
				lsbs[1] = frame[6];
				int tempi = hexAsciiToByte(msbs) * 256 + hexAsciiToByte(lsbs);
				// Value used for top voltage is adapted to board 11.
				float tempf = (tempi * 3140.0f / 1023.0f - 500.0f) / 10.0f;
				String temps = decimalFormat.format(tempf) + "°C";
				displayMessage.displayLogLater("temperature: " + temps);
				displayMessage.displayTemperature(temps);
				break;
			}
			displayMessage.displayLogLater("unknown application frame");
			break;
		default:
			displayMessage.displayLogLater("unknown frame type");
		}
	}

	/**
	 *
	 */
	public short[] createSetRemoteLedRMsg() {

		short message[] = new short[2];

		message[0] = LETTER_L;
		message[1] = LETTER_R;
		return message;

	}

	/**
	 *
	 */
	public short[] createSetRemoteLedGMsg() {

		short message[] = new short[2];

		message[0] = LETTER_L;
		message[1] = LETTER_G;
		return message;

	}

	/**
	 *
	 */
	public short[] createSetRemoteLedBMsg() {

		short message[] = new short[2];

		message[0] = LETTER_L;
		message[1] = LETTER_B;
		return message;

	}

	/**
	 *
	 */
	public short[] createSetRemoteLedOffMsg() {

		short message[] = new short[2];

		message[0] = LETTER_L;
		message[1] = FIGURE_0;
		return message;

	}

	/**
	 * Returns a value between 0 and 255, taking as input parameter 2 bytes containing
	 * the ASCII codes of the hex representation of the byte. MSQ has to be in first byte,
	 * LSQ in second one.
	 *
	 * @param b
	 * @return 0 if input parameter is out of range
	 */
	private short hexAsciiToByte(short[] b) {

		int l = 0;
		int m = 0;

		if ((b[1] >= 0x30) && (b[1] <= 0x39)) {
			l = b[1] - 0x30;
		} else if ((b[1] >= 0x41) && (b[1] <= 0x46)) {
			l = b[1] - 0x41 + 10;
		} else if ((b[1] >= 0x61) && (b[1] <= 0x66)) {
			l = b[1] - 0x61 + 10;
		}

		if ((b[0] >= 0x30) && (b[0] <= 0x39)) {
			m = b[0] - 0x30;
		} else if ((b[0] >= 0x41) && (b[0] <= 0x46)) {
			m = b[0] - 0x41 + 10;
		} else if ((b[0] >= 0x61) && (b[0] <= 0x66)) {
			m = b[0] - 0x61 + 10;
		}

		return (short)(m * 16 + l);
	}

	/**
	 *
	 * Used to display frames received from device. First character is not displayed,
	 * as it is used to multiplex trace and application frames.
	 * @param b
	 * @return
	 */
	public String frameToString(short[] b) {

		if (b.length == 0) return "";

		StringBuilder sb = new StringBuilder();
		for (int i = 1; i < b.length; i++) {
			sb.append((char)b[i]);
		}
		return sb.toString();

	}

}
