package com.nets;

import com.nets.controller.GameController;
import com.nets.view.GameBoard;
import javafx.application.Application;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;

import java.util.Optional;

public class NetsGame extends Application {

    private GameController controller;
    private GameBoard gameBoard;
    private Stage primaryStage;
    private int currentRows = 5;
    private int currentCols = 5;

    @Override
    public void start(Stage primaryStage) {
        this.primaryStage = primaryStage;
        primaryStage.setTitle("Nets Game");

        // Show startup dialog - always start with new game
        Optional<int[]> dimensions = showStartupDialog();

        if (dimensions.isPresent()) {
            currentRows = dimensions.get()[0];
            currentCols = dimensions.get()[1];
        } else {
            // User cancelled, use defaults
            currentRows = 5;
            currentCols = 5;
        }

        // Always initialize with new game (no file loading at startup)
        initializeGame();
    }

    private Optional<int[]> showStartupDialog() {
        Dialog<int[]> dialog = new Dialog<>();
        dialog.setTitle("New Game Setup");
        dialog.setHeaderText("Configure Your Network Board");

        // Set the button types
        ButtonType startButtonType = new ButtonType("Start Game", ButtonBar.ButtonData.OK_DONE);
        dialog.getDialogPane().getButtonTypes().addAll(startButtonType, ButtonType.CANCEL);

        // Create the grid layout
        GridPane grid = new GridPane();
        grid.setHgap(10);
        grid.setVgap(10);
        grid.setPadding(new Insets(20, 150, 10, 10));

        Spinner<Integer> rowSpinner = new Spinner<>(3, 15, 5);
        Spinner<Integer> colSpinner = new Spinner<>(3, 15, 5);

        rowSpinner.setEditable(true);
        colSpinner.setEditable(true);

        rowSpinner.setPrefWidth(100);
        colSpinner.setPrefWidth(100);

        Label infoLabel = new Label("Board size: 3x3 to 15x15");
        infoLabel.setStyle("-fx-text-fill: #666;");

        grid.add(new Label("Rows:"), 0, 0);
        grid.add(rowSpinner, 1, 0);
        grid.add(new Label("Columns:"), 0, 1);
        grid.add(colSpinner, 1, 1);
        grid.add(infoLabel, 0, 2, 2, 1);

        dialog.getDialogPane().setContent(grid);

        // Style the dialog
        dialog.getDialogPane().setStyle(
                "-fx-background-color: #f5f5f5; " +
                        "-fx-font-family: 'Arial';"
        );

        // Convert result
        dialog.setResultConverter(dialogButton -> {
            if (dialogButton == startButtonType) {
                return new int[]{rowSpinner.getValue(), colSpinner.getValue()};
            }
            return null;
        });

        return dialog.showAndWait();
    }

    private void initializeGame() {
        // Create game board
        gameBoard = new GameBoard();

        // Create controller
        controller = new GameController(gameBoard);

        // Create control buttons
        HBox controls = createControls();

        // Layout
        BorderPane root = new BorderPane();
        root.setCenter(gameBoard);
        root.setBottom(controls);
        root.setStyle("-fx-background-color: #1a1a2e;");

        // Scene
        Scene scene = new Scene(root, 1000, 800);
        primaryStage.setScene(scene);
        primaryStage.show();

        // Initialize game with selected dimensions
        System.out.println("Starting new game: " + currentRows + "x" + currentCols);
        controller.initGame(currentRows, currentCols);

        // Show welcome message
        showWelcomeMessage();
    }

    private void showWelcomeMessage() {
        Alert alert = new Alert(Alert.AlertType.INFORMATION);
        alert.setTitle("Welcome to Nets!");
        alert.setHeaderText("🎮 Network Connection Game!");
        alert.setContentText(
                "Board Size: " + currentRows + "×" + currentCols + "\n\n" +
                        "🖱️ Left Click: Rotate wire clockwise\n\n" +
                        "🎯 Goal: Connect all PCs to the power source!\n" +
                        "Rotate the network wires to complete the connections.\n\n" +
                        "Good luck! 🍀"
        );
        alert.showAndWait();
    }

    private HBox createControls() {
        HBox controls = new HBox(15);
        controls.setPadding(new Insets(15));
        controls.setAlignment(Pos.CENTER);
        controls.setStyle("-fx-background-color: #16213e;");

        Button newGameButton = new Button("New Game");
        newGameButton.setStyle(
                "-fx-background-color: #00d4ff; " +
                        "-fx-text-fill: white; " +
                        "-fx-font-size: 14px; " +
                        "-fx-font-weight: bold; " +
                        "-fx-padding: 10 20; " +
                        "-fx-cursor: hand; " +
                        "-fx-background-radius: 5;"
        );
        newGameButton.setOnMouseEntered(e -> newGameButton.setStyle(
                "-fx-background-color: #00b8e6; " +
                        "-fx-text-fill: white; " +
                        "-fx-font-size: 14px; " +
                        "-fx-font-weight: bold; " +
                        "-fx-padding: 10 20; " +
                        "-fx-cursor: hand; " +
                        "-fx-background-radius: 5;"
        ));
        newGameButton.setOnMouseExited(e -> newGameButton.setStyle(
                "-fx-background-color: #00d4ff; " +
                        "-fx-text-fill: white; " +
                        "-fx-font-size: 14px; " +
                        "-fx-font-weight: bold; " +
                        "-fx-padding: 10 20; " +
                        "-fx-cursor: hand; " +
                        "-fx-background-radius: 5;"
        ));
        newGameButton.setOnAction(e -> showNewGameDialog());

        Button resetButton = new Button("Reset Board");
        resetButton.setStyle(
                "-fx-background-color: #e94560; " +
                        "-fx-text-fill: white; " +
                        "-fx-font-size: 14px; " +
                        "-fx-padding: 10 20; " +
                        "-fx-cursor: hand; " +
                        "-fx-background-radius: 5;"
        );
        resetButton.setOnMouseEntered(e -> resetButton.setOpacity(0.8));
        resetButton.setOnMouseExited(e -> resetButton.setOpacity(1.0));
        resetButton.setOnAction(e -> controller.resetGame(currentRows, currentCols));

        Button helpButton = new Button("Help");
        helpButton.setStyle(
                "-fx-background-color: transparent; " +
                        "-fx-text-fill: white; " +
                        "-fx-font-size: 14px; " +
                        "-fx-padding: 10 20; " +
                        "-fx-cursor: hand; " +
                        "-fx-border-color: #0f3460; " +
                        "-fx-border-width: 2; " +
                        "-fx-border-radius: 5; " +
                        "-fx-background-radius: 5;"
        );
        helpButton.setOnMouseEntered(e -> helpButton.setStyle(
                "-fx-background-color: #0f3460; " +
                        "-fx-text-fill: white; " +
                        "-fx-font-size: 14px; " +
                        "-fx-padding: 10 20; " +
                        "-fx-cursor: hand; " +
                        "-fx-border-color: #0f3460; " +
                        "-fx-border-width: 2; " +
                        "-fx-border-radius: 5; " +
                        "-fx-background-radius: 5;"
        ));
        helpButton.setOnMouseExited(e -> helpButton.setStyle(
                "-fx-background-color: transparent; " +
                        "-fx-text-fill: white; " +
                        "-fx-font-size: 14px; " +
                        "-fx-padding: 10 20; " +
                        "-fx-cursor: hand; " +
                        "-fx-border-color: #0f3460; " +
                        "-fx-border-width: 2; " +
                        "-fx-border-radius: 5; " +
                        "-fx-background-radius: 5;"
        ));
        helpButton.setOnAction(e -> showHelp());

        controls.getChildren().addAll(newGameButton, resetButton, helpButton);
        return controls;
    }

    private void showNewGameDialog() {
        Optional<int[]> dimensions = showStartupDialog();

        if (dimensions.isPresent()) {
            currentRows = dimensions.get()[0];
            currentCols = dimensions.get()[1];
            controller.resetGame(currentRows, currentCols);
        }
    }

    private void showHelp() {
        Alert alert = new Alert(Alert.AlertType.INFORMATION);
        alert.setTitle("How to Play");
        alert.setHeaderText("🎮 Nets Game Instructions");

        VBox content = new VBox(10);
        content.setPadding(new Insets(10));

        Label goal = new Label("🎯 Goal: Connect all PCs to the power source through the network");
        goal.setStyle("-fx-font-weight: bold; -fx-font-size: 14px;");

        Label controls = new Label(
                "🖱️ Controls:\n" +
                        "• Left Click: Rotate network wire clockwise (90°)\n" +
                        "• Locked tiles (power source with lock icon) cannot be rotated"
        );

        Label tiles = new Label(
                "🔌 Network Components:\n" +
                        "• Straight Wire: Connects two opposite sides\n" +
                        "• Corner Wire: Connects two adjacent sides\n" +
                        "• T-Junction Wire: Connects three sides\n" +
                        "• ⚡ Power Source: Network power supply (locked, cannot rotate)\n" +
                        "• 💻 PC: Computer that needs network connection (square tiles)"
        );

        Label win = new Label(
                "🏆 Win Condition:\n" +
                        "• All PCs connected to power source through network wires\n" +
                        "• Single connected network (1 component)\n" +
                        "• No loose wire ends (0 loose ends)\n" +
                        "• No loops in the network"
        );
        win.setStyle("-fx-font-weight: bold;");

        content.getChildren().addAll(goal, new Separator(), controls, new Separator(),
                tiles, new Separator(), win);

        alert.getDialogPane().setContent(content);
        alert.getDialogPane().setPrefWidth(550);
        alert.showAndWait();
    }

    public static void main(String[] args) {
        launch(args);
    }
}