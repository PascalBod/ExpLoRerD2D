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

import java.net.URL;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.ResourceBundle;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Button;
import javafx.scene.control.ComboBox;
import javafx.scene.control.ListView;
import javafx.scene.text.Text;

public class UserInterfaceController implements Initializable, EventHandler<ActionEvent> {

	private final static int MAX_NB_FRAMES = 100;
	private final static int MAX_NB_LOGMSGS = 100;

	private final static String SEPARATOR = " - ";

	@FXML private ComboBox<String> serialPortCB;
	@FXML private Button setRemoteLedRBtn;
	@FXML private Button setRemoteLedGBtn;
	@FXML private Button setRemoteLedBBtn;
	@FXML private Button setRemoteLedOffBtn;
	@FXML private ListView<String> recFramesLV;
	@FXML private ListView<String> logMsgsLV;
	@FXML private Text temperatureTxt;

	private ObservableList<String> displayedFrames;
	private ListViewMessages recFrames;

	private ObservableList<String> displayedLogMsgs;
	private ListViewMessages logMsgs;

	private ProcessAction processAction;

	private SimpleDateFormat timeFormat;

	/**
	 * For EventHandler<ActionEvent> interface.
	 * Is called only by the combo box, when the user selects a serial port.
	 */
	@Override
	public void handle(ActionEvent event) {

		if (processAction != null) {
			processAction.serialPortValue(serialPortCB.getValue());
			// User is no more allowed to modify serial port.
			serialPortCB.setDisable(true);
		} else {
			displayLogMsg("internal error: processAction is null");
		}
	}

	/**
	 *
	 */
	@Override
	public void initialize(URL location, ResourceBundle resources) {

		setRemoteLedRBtn.setOnAction(new EventHandler<ActionEvent>() {

			@Override
			public void handle(ActionEvent event) {

				if (processAction != null) {
					processAction.setRemoteLedR();
				} else {
					displayLogMsg("internal error: processAction is null");
				}

			}
		});

		setRemoteLedGBtn.setOnAction(new EventHandler<ActionEvent>() {

			@Override
			public void handle(ActionEvent event) {

				if (processAction != null) {
					processAction.setRemoteLedG();
				} else {
					displayLogMsg("internal error: processAction is null");
				}

			}
		});

		setRemoteLedBBtn.setOnAction(new EventHandler<ActionEvent>() {

			@Override
			public void handle(ActionEvent event) {

				if (processAction != null) {
					processAction.setRemoteLedB();
				} else {
					displayLogMsg("internal error: processAction is null");
				}

			}
		});

		setRemoteLedOffBtn.setOnAction(new EventHandler<ActionEvent>() {

			@Override
			public void handle(ActionEvent event) {

				if (processAction != null) {
					processAction.setRemoteLedOff();
				} else {
					displayLogMsg("internal error: processAction is null");
				}

			}
		});

		// To display latest received frames.
		displayedFrames = FXCollections.observableArrayList();
		recFramesLV.setItems(displayedFrames);
		recFrames = new ListViewMessages(displayedFrames, MAX_NB_FRAMES);

		// To display latest log messages.
		displayedLogMsgs = FXCollections.observableArrayList();
		logMsgsLV.setItems(displayedLogMsgs);
		logMsgs = new ListViewMessages(displayedLogMsgs, MAX_NB_LOGMSGS);

		// To get serial port selected by the user.
		serialPortCB.setOnAction(this);

		// Setup button states.
		enableActions(false);

		timeFormat = new SimpleDateFormat("HH:mm:ss.SSS");

	}

	/**
	 *
	 * @param frame
	 */
	public void displayFrame(String frame) {

		Date currentTime = new Date();
		String s = timeFormat.format(currentTime);
		recFrames.addMessage(s + SEPARATOR + frame);

	}

	/**
	 *
	 * @param logMsg
	 */
	public void displayLogMsg(String logMsg) {

		Date currentTime = new Date();
		String s = timeFormat.format(currentTime);
		logMsgs.addMessage(s + SEPARATOR + logMsg);

	}

	/**
	 *
	 * @param temp
	 */
	public void displayTemperature(String temp) {

		temperatureTxt.setText(temp);

	}

	/**
	 *
	 * @param processAction
	 */
	public void setProcessAction(ProcessAction processAction) {

		this.processAction = processAction;

	}

	/**
	 * To call once serial port is setup successfully.
	 */
	public void enableActions(boolean enable) {

		setRemoteLedRBtn.setDisable(!enable);
		setRemoteLedGBtn.setDisable(!enable);
		setRemoteLedBBtn.setDisable(!enable);
		setRemoteLedOffBtn.setDisable(!enable);

	}

	/**
	 *
	 * @param portNameList
	 */
	public void displaySerialPorts(ArrayList<String> portNameList) {

		ObservableList<String> ol = FXCollections.observableArrayList();
		ol.setAll(portNameList);
		serialPortCB.setItems(ol);

	}

	/**
	 * How to process actions. Implemented by Main.
	 */
	public interface ProcessAction {

		public void serialPortValue(String serialPortName);

		public void setRemoteLedR();

		public void setRemoteLedG();

		public void setRemoteLedB();

		public void setRemoteLedOff();

	}

}
