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

import javafx.collections.ObservableList;

/**
 *
 * Handles a list of a maximum number of messages, sorted from most recent
 * to least recent one. When a new message is inserted, and maximum number
 * of messages is reached, oldest message is removed from the list.
 *
 */
public class ListViewMessages {

	private ObservableList<String> messageList;
	private int maxNumberOfMessages;

	/**
	 *
	 * @param frameList
	 */
	public ListViewMessages(ObservableList<String> frameList, int maxNumberOfMessages) {

		this.messageList = frameList;
		this.maxNumberOfMessages = maxNumberOfMessages;

	}

	/**
	 *
	 * @param frame
	 */
	public void addMessage(String message) {

		int s = messageList.size();
		if (s >= maxNumberOfMessages) {
			// Remove oldest element.
			messageList.remove(s - 1);
		}
		// Add new element.
		messageList.add(0, message);

	}

}
