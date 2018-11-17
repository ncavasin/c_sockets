package view;
	
import javafx.application.Application;
import javafx.stage.Stage;
import javafx.scene.Scene;
import javafx.scene.layout.BorderPane;
import javafx.fxml.FXMLLoader;

/**
 * @author nicolas cavasin
 *
 */
public class Main extends Application {
	
	@Override
	public void start(Stage primaryStage) {
		try {
			BorderPane root = (BorderPane)FXMLLoader.load(getClass().getResource("MainWindow.fxml"));
			Scene scene = new Scene(root);
			primaryStage.setScene(scene);
			primaryStage.centerOnScreen();
			primaryStage.setTitle("Chatter");
			primaryStage.show();

		} catch(Exception e) {
			System.out.println(e.getMessage());
		}
	}


	public static void main(String[] args) {
		launch(args);
	}
}
