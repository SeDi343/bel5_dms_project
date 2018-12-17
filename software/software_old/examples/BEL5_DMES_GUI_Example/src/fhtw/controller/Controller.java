/**
 * 
 */
package fhtw.controller;

import java.net.URL;
import java.util.ResourceBundle;

import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Button;
import javafx.scene.control.MenuBar;
import javafx.scene.control.TextField;

/**
 * @author Markus Lechner
 *
 */
public class Controller implements Initializable {
	@FXML
	private Button btn_apply;

	@FXML
	private Button btn_clear;

	@FXML
	private TextField tf_input;

	@FXML
	private TextField tf_applied;

	@FXML
	private MenuBar mb_menu;

	@Override
	public void initialize(URL arg0, ResourceBundle arg1) {

		btn_apply.setOnAction((ActionEvent)-> {
			tf_applied.setText(tf_input.getText());
		});		

		btn_clear.setOnAction((ActionEvent)-> {
			tf_applied.clear();
			tf_input.clear();
		});		

	}
}
