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

import java.util.ArrayList;

import com.monblocnotes.explorerd2d.central.UserInterfaceController.ProcessAction;

import javafx.application.Application;
import javafx.application.Platform;
import javafx.fxml.FXMLLoader;
import javafx.stage.Stage;
import javafx.scene.Parent;
import javafx.scene.Scene;

/**
 *
 * Main class.
 * Implements DisplayMessage interface.
 *
 */
public class Main extends Application implements DisplayMessage, ProcessAction {

	// Current version.
	private final static String VERSION = "0.3";
	private final static String APPLICATION = "IoT LoRa D2D demonstrator - version ";

	// Size of main window.
	private final static int WIDTH = 600;
	private final static int HEIGHT = 600;

	private UserInterfaceController controller;
	private PortHandler portHandler;

	/**
	 *
	 */
	@Override
	public void start(Stage primaryStage) {

		FXMLLoader fxmlLoader = null;
		try {
			fxmlLoader = new FXMLLoader();
			Parent root = fxmlLoader.load(getClass().getResource("UserInterface.fxml").openStream());
			Scene scene = new Scene(root,WIDTH,HEIGHT);
			scene.getStylesheets().add(getClass().getResource("application.css").toExternalForm());
			primaryStage.setScene(scene);
			primaryStage.show();
		} catch(Exception e) {
			e.printStackTrace();
		}
		controller = (UserInterfaceController)fxmlLoader.getController();
		// Display version.
		controller.displayLogMsg(APPLICATION + VERSION);
		// Display list of available serial ports.
		portHandler = new PortHandler(this);
		ArrayList<String> portNameList = portHandler.getSerialPorts();
		if (portNameList != null) {
			controller.displaySerialPorts(portNameList);
		} else {
			controller.displayLogMsg("No serial port available");
		}
		controller.setProcessAction(this);

	}

	/**
	 *
	 * @param args
	 */
	public static void main(String[] args) {

		launch(args);

	}

	/**
	 * For DisplayMessage interface.
	 *
	 * @param message
	 */
	@Override
	public void displayLog(String message) {

		controller.displayLogMsg(message);

	}

	/**
	 * For DisplayMessage interface.
	 *
	 */
	@Override
	public void displayLogLater(String message) {

		Platform.runLater(new Runnable() {

			@Override
			public void run() {

				controller.displayLogMsg(message);

			}

		});
	}

	/**
	 * For DisplayMessage interface.
	 *
	 * @param message
	 */
	@Override
	public void displayFrame(String message) {

		controller.displayFrame(message);

	}

	/**
	 * For DisplayMessage interface.
	 *
	 */
	@Override
	public void displayFrameLater(String message) {

		Platform.runLater(new Runnable() {

			@Override
			public void run() {

				controller.displayFrame(message);

			}

		});
	}

	/**
	 * For DisplayMessage interface.
	 *
	 */
	@Override
	public void displayTemperature(String temp) {

		Platform.runLater(new Runnable() {
			@Override
			public void run() {
				controller.displayTemperature(temp);
			}
		});
	}

	/**
	 * For ProcessAction interface.
	 *
	 * @param serialPortName
	 */
	@Override
	public void serialPortValue(String serialPortName) {

		int rs = portHandler.setSerialPort(serialPortName);
		if (rs == 0) {
			// Enable buttons.
			Platform.runLater(new Runnable() {

				@Override
				public void run() {

					controller.enableActions(true);

				}

			});
		}

	}

	/**
	 *
	 */
	@Override
	public void setRemoteLedR() {

		portHandler.sendMsg(portHandler.createSetRemoteLedRMsg());
		displayLog("set remote LED red message sent");

	}

	/**
	 *
	 */
	@Override
	public void setRemoteLedG() {

		portHandler.sendMsg(portHandler.createSetRemoteLedGMsg());
		displayLog("set remote LED green message sent");

	}

	/**
	 *
	 */
	@Override
	public void setRemoteLedB() {

		portHandler.sendMsg(portHandler.createSetRemoteLedBMsg());
		displayLog("set remote LED blue message sent");

	}

	/**
	 *
	 */
	@Override
	public void setRemoteLedOff() {

		portHandler.sendMsg(portHandler.createSetRemoteLedOffMsg());
		displayLog("set remote LED off message sent");

	}

}
