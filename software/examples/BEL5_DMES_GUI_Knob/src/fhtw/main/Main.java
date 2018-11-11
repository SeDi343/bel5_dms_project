/**
 * 
 */
package fhtw.main;

import java.io.InputStream;

import fhtw.controller.Controller;
import javafx.application.Application;
import javafx.event.EventHandler;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.scene.layout.AnchorPane;
import javafx.stage.Stage;
import javafx.stage.WindowEvent;

/**
 * @author Markus Lechner
 *
 */
public class Main extends Application {

	@Override
	public void start(Stage primaryStage) throws Exception {
		try {
			Controller controller = new Controller();

			FXMLLoader fxmlLoader = new FXMLLoader();
			fxmlLoader.setClassLoader(getClass().getClassLoader());
			fxmlLoader.setLocation(getClass().getResource("/fxml/Example.fxml"));
			fxmlLoader.setController(controller);
			
			AnchorPane root = fxmlLoader.load();			

			Scene scene = new Scene(root);
			scene.getStylesheets().add(getClass().getResource("/css/Example.css").toExternalForm());
			primaryStage.setTitle("Knob Example Application");				

			InputStream isIcon = Main.class.getResourceAsStream("/images/android.png");			
			primaryStage.getIcons().add(new Image(isIcon));			
			isIcon.close();

			primaryStage.setResizable(false);
			primaryStage.setScene(scene);				

			primaryStage.setOnCloseRequest(new EventHandler<WindowEvent>() {
				@Override
				public void handle(WindowEvent event) {
					System.exit(0);
				}
			});		

			primaryStage.show();	

		} catch(Exception e) {
			e.printStackTrace();
		}		
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		launch(args);
	}
}

/* EOF */