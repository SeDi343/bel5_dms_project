/**
 * 
 */
package fhtw.controller;

import java.net.URL;
import java.util.ResourceBundle;

import fhtw.util.BasicKnob;
import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Label;
import javafx.scene.input.MouseButton;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.GridPane;

/**
 * @author Markus Lechner
 *
 */
public class Controller implements Initializable {
	@FXML
	private AnchorPane anchor_pane;
	@FXML
	private GridPane gp_knob;
    @FXML
    private Label lbl_knob_value;

	private BasicKnob knob_example;
	
	@Override
	public void initialize(URL arg0, ResourceBundle arg1) {
		knob_example = new BasicKnob("vertical-knob", "knob", 100);
		knob_example.setInverted(false);
		knob_example.setValue(50.0);
		knob_example.setOnMouseClicked(new EventHandler<MouseEvent>() {
			@Override
			public void handle(MouseEvent mouseEvent) {
				if(mouseEvent.getButton() == MouseButton.PRIMARY){
					if(mouseEvent.getClickCount() == 2){
						knob_example.setValue(50.0);
					}
				}
			}
		});		
		knob_example.valueProperty().addListener((obs, oldValue, newValue) -> {
			lbl_knob_value.setText(String.format("%.2f", newValue) + "%");
		});
		gp_knob.add(knob_example, 0, 0);
	}
}

/* EOF */