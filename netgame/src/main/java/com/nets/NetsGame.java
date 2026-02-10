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
        
        // Window size - 80% of screen
        javafx.stage.Screen screen = javafx.stage.Screen.getPrimary();
        javafx.geometry.Rectangle2D bounds = screen.getVisualBounds();
        double width = bounds.getWidth() * 0.8;
        double height = bounds.getHeight() * 0.8;
        
        primaryStage.setWidth(width);
        primaryStage.setHeight(height);
        primaryStage.setX((bounds.getWidth() - width) / 2);
        primaryStage.setY((bounds.getHeight() - height) / 2);
        primaryStage.setMaximized(false);

        // Initialize UI immediately so Stage has a Scene
        initializeGame();

        // Show startup dialog - always start with new game
        Optional<int[]> dimensions = showStartupDialog();

        if (dimensions.isPresent()) {
            currentRows = dimensions.get()[0];
            currentCols = dimensions.get()[1];
            // Re-initialize with user choice
            controller.initGame(currentRows, currentCols);
        } else {
            // User cancelled, use defaults (already set by initializeGame)
        }
        
        // Show welcome message
        showWelcomeMessage();
    }

    private Optional<int[]> showStartupDialog() {
        Dialog<int[]> dialog = new Dialog<>();
        dialog.setTitle("New Game Setup");
        dialog.setHeaderText("Configure Your Network Board");
        
        // Ensure dialog is centered and has a good size
        dialog.initOwner(primaryStage);

        // Set the button types
        ButtonType startButtonType = new ButtonType("Start Game", ButtonBar.ButtonData.OK_DONE);
        dialog.getDialogPane().getButtonTypes().addAll(startButtonType, ButtonType.CANCEL);

        // Create the grid layout
        GridPane grid = new GridPane();
        grid.setHgap(30);
        grid.setVgap(30);
        grid.setPadding(new Insets(50));
        grid.setAlignment(Pos.CENTER);

        Spinner<Integer> rowSpinner = new Spinner<>(3, 30, 5);
        Spinner<Integer> colSpinner = new Spinner<>(3, 30, 5);

        rowSpinner.setEditable(true);
        colSpinner.setEditable(true);

        rowSpinner.setPrefWidth(220);
        colSpinner.setPrefWidth(220);
        rowSpinner.setStyle("-fx-font-size: 24px;");
        colSpinner.setStyle("-fx-font-size: 24px;");

        Label infoLabel = new Label("Board size: 3x3 to 30x30");
        infoLabel.setStyle("-fx-text-fill: #666; -fx-font-size: 18px;");

        Label rowLabel = new Label("Rows:");
        rowLabel.setStyle("-fx-font-size: 26px; -fx-font-weight: bold;");
        Label colLabel = new Label("Columns:");
        colLabel.setStyle("-fx-font-size: 26px; -fx-font-weight: bold;");

        grid.add(rowLabel, 0, 0);
        grid.add(rowSpinner, 1, 0);
        grid.add(colLabel, 0, 1);
        grid.add(colSpinner, 1, 1);
        grid.add(infoLabel, 0, 2, 2, 1);

        dialog.getDialogPane().setContent(grid);

        // Style the dialog
        dialog.getDialogPane().setStyle(
                "-fx-background-color: #f5f5f5; " +
                        "-fx-font-family: 'Arial'; " +
                        "-fx-font-size: 24px;"
        );
        
        dialog.getDialogPane().setPrefWidth(900);
        dialog.getDialogPane().setPrefHeight(500);

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
        Scene scene = new Scene(root, primaryStage.getWidth(), primaryStage.getHeight());
        primaryStage.setScene(scene);
        primaryStage.show();

        // Initialize game with selected dimensions
        System.out.println("Starting new game: " + currentRows + "x" + currentCols);
        controller.initGame(currentRows, currentCols);
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
        alert.getDialogPane().setPrefWidth(800);
        alert.getDialogPane().setStyle("-fx-font-size: 24px;");
        alert.showAndWait();
    }

    private HBox createControls() {
        HBox controls = new HBox(20);
        controls.setPadding(new Insets(20));
        controls.setAlignment(Pos.CENTER);
        controls.setStyle("-fx-background-color: #16213e;");

        Button newGameButton = new Button("New Game");
        String baseBtnStyle = "-fx-text-fill: white; -fx-font-size: 18px; -fx-font-weight: bold; -fx-padding: 12 25; -fx-cursor: hand; -fx-background-radius: 8;";
        
        newGameButton.setStyle(baseBtnStyle + "-fx-background-color: #00d4ff;");
        newGameButton.setOnMouseEntered(e -> newGameButton.setStyle(baseBtnStyle + "-fx-background-color: #00b8e6;"));
        newGameButton.setOnMouseExited(e -> newGameButton.setStyle(baseBtnStyle + "-fx-background-color: #00d4ff;"));
        newGameButton.setOnAction(e -> showNewGameDialog());

        Button resetButton = new Button("Reset Board");
        resetButton.setStyle(baseBtnStyle + "-fx-background-color: #e94560;");
        resetButton.setOnMouseEntered(e -> resetButton.setOpacity(0.8));
        resetButton.setOnMouseExited(e -> resetButton.setOpacity(1.0));
        resetButton.setOnAction(e -> controller.resetGame(currentRows, currentCols));

        Button helpButton = new Button("Help");
        String helpBtnStyle = "-fx-background-color: transparent; -fx-text-fill: white; -fx-font-size: 18px; -fx-padding: 12 25; -fx-cursor: hand; -fx-border-color: #0f3460; -fx-border-width: 2; -fx-border-radius: 8; -fx-background-radius: 8;";
        helpButton.setStyle(helpBtnStyle);
        helpButton.setOnMouseEntered(e -> helpButton.setStyle(helpBtnStyle + "-fx-background-color: #0f3460;"));
        helpButton.setOnMouseExited(e -> helpButton.setStyle(helpBtnStyle));
        helpButton.setOnAction(e -> showHelp());

        ToggleButton solutionButton = new ToggleButton("Show Solution");
        String solutionButtonStyle = "-fx-background-color: #ffc107; -fx-text-fill: black; -fx-font-size: 18px; -fx-padding: 12 25; -fx-cursor: hand; -fx-background-radius: 8;";
        String solutionButtonHoverStyle = "-fx-background-color: #ffca2c; -fx-text-fill: black; -fx-font-size: 18px; -fx-padding: 12 25; -fx-cursor: hand; -fx-background-radius: 8;";
        solutionButton.setStyle(solutionButtonStyle);
        solutionButton.setOnAction(e -> {
            boolean isSelected = solutionButton.isSelected();
            if (controller != null) {
                controller.toggleSolution(isSelected);
            }
            solutionButton.setStyle(isSelected ? solutionButtonHoverStyle : solutionButtonStyle);
        });

        Button rotateButton = new Button("Rotate Board");
        String rotateButtonStyle = "-fx-background-color: #9b59b6; -fx-text-fill: white; -fx-font-size: 18px; -fx-padding: 12 25; -fx-cursor: hand; -fx-background-radius: 8;";
        rotateButton.setStyle(rotateButtonStyle);
        rotateButton.setOnAction(e -> {
            if (gameBoard != null) {
                gameBoard.rotateBoardVisual();
            }
        });

        controls.getChildren().addAll(newGameButton, resetButton, helpButton, solutionButton, rotateButton);
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

        VBox content = new VBox(15);
        content.setPadding(new Insets(15));

        Label goal = new Label("🎯 Goal: Connect all PCs to the power source through the network");
        goal.setStyle("-fx-font-weight: bold; -fx-font-size: 18px;");

        Label controls = new Label(
                "🖱️ Controls:\n" +
                        "• Left Click: Rotate network wire clockwise (90°)\n" +
                        "• Locked tiles (power source) cannot be rotated"
        );
        controls.setStyle("-fx-font-size: 16px;");

        Label tiles = new Label(
                "🔌 Network Components:\n" +
                        "• Straight Wire: Connects two opposite sides\n" +
                        "• Corner Wire: Connects two adjacent sides\n" +
                        "• T-Junction Wire: Connects three sides\n" +
                        "• ⚡ Power Source: Network power supply\n" +
                        "• 💻 PC: Computer that needs network connection"
        );
        tiles.setStyle("-fx-font-size: 16px;");

        Label win = new Label(
                "🏆 Win Condition:\n" +
                        "• All PCs connected to power source\n" +
                        "• Single connected network\n" +
                        "• No loose wire ends\n" +
                        "• No loops in the network"
        );
        win.setStyle("-fx-font-weight: bold; -fx-font-size: 16px;");

        content.getChildren().addAll(goal, new Separator(), controls, new Separator(),
                tiles, new Separator(), win);

        alert.getDialogPane().setContent(content);
        alert.getDialogPane().setPrefWidth(1000);
        alert.getDialogPane().setPrefHeight(700);
        alert.getDialogPane().setStyle("-fx-font-size: 24px;");
        alert.showAndWait();
    }

    public static void main(String[] args) {
        launch(args);
    }
}