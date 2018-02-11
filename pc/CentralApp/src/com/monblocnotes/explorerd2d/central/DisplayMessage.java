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

/**
 *
 * Interface used to specify display services provided by main class.
 *
 */
public interface DisplayMessage {

	/**
	 * Must be called from FX application context.
	 * @param message
	 */
	public void displayLog(String message);

	/**
	 * Can be called from any context.
	 * @param message
	 */
	public void displayLogLater(String message);

	/**
	 * Must be called from FX application context.
	 * @param message
	 */
	public void displayFrame(String message);

	/**
	 * Can be called from any context.
	 * @param message
	 */
	public void displayFrameLater(String message);

	/**
	 * Can be called from any context.
	 */
	public void displayTemperature(String temp);

}
