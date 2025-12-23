package com.nets;

import com.nets.controller.GameController;
import com.nets.model.GameState;
import com.nets.util.FileWatcherService;
import com.nets.util.JsonHandler;
import com.nets.view.GameBoard;
import javafx.application.Application;
import javafx.application.Platform;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
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
        dialog.setHeaderText("Configure Your Game Board");

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

    private Label looseEndsLabel;
    private Label componentsLabel;
    private Label solvedLabel;
    private FileWatcherService fileWatcherService;

    private void initializeGame() {
        // Create game board
        gameBoard = new GameBoard();

        // Create controller
        controller = new GameController(gameBoard, this::updateStats);

        // Create control buttons
        HBox controls = createControls();

        // Create status bar
        VBox statusBar = createStatusBar();

        // Layout
        BorderPane root = new BorderPane();
        root.setCenter(gameBoard);

        VBox bottomContainer = new VBox(controls, statusBar);
        root.setBottom(bottomContainer);
        root.setStyle("-fx-background-color: #1a1a2e;");

        // Scene
        Scene scene = new Scene(root, 1000, 800);
        primaryStage.setScene(scene);
        primaryStage.show();

        // Initialize game with selected dimensions
        System.out.println("Starting new game: " + currentRows + "x" + currentCols);
        controller.initGame(currentRows, currentCols);

        // Start file watcher
        try {
            String currentDir = System.getProperty("user.dir");
            Path jsonPath = Paths.get(currentDir, "game_state.json");
            if (!Files.exists(jsonPath)) {
                Files.createFile(jsonPath);
                GameState emptyState = new GameState();
                JsonHandler.saveGameState(emptyState, jsonPath.toString());
            }
            fileWatcherService = new FileWatcherService(jsonPath, this::onFileModified);
            fileWatcherService.start();
        } catch (IOException e) {
            e.printStackTrace();
            // Handle error appropriately
        }

        // Show welcome message
        showWelcomeMessage();
    }

    private void onFileModified(Path path) {
        Platform.runLater(() -> {
            controller.loadExistingGame();
        });
    }

    @Override
    public void stop() throws Exception {
        if (fileWatcherService != null) {
            fileWatcherService.stop();
        }
        super.stop();
    }

    public void updateStats(GameState gameState) {
        if (gameState != null && gameState.getStats() != null) {
            looseEndsLabel.setText("Loose Ends: " + gameState.getStats().getLooseEnds());
            componentsLabel.setText("Components: " + gameState.getStats().getComponents());
            solvedLabel.setText("Solved: " + (gameState.getStats().isSolved() ? "Yes" : "No"));
        }
    }

    private VBox createStatusBar() {
        HBox statusBar = new HBox(15);
        statusBar.setPadding(new Insets(10));
        statusBar.setAlignment(Pos.CENTER);
        statusBar.setStyle("-fx-background-color: #16213e;");

        looseEndsLabel = new Label("Loose Ends: -");
        looseEndsLabel.setStyle("-fx-text-fill: white; -fx-font-size: 14px;");

        componentsLabel = new Label("Components: -");
        componentsLabel.setStyle("-fx-text-fill: white; -fx-font-size: 14px;");

        solvedLabel = new Label("Solved: -");
        solvedLabel.setStyle("-fx-text-fill: white; -fx-font-size: 14px;");

        statusBar.getChildren().addAll(looseEndsLabel, componentsLabel, solvedLabel);

        VBox container = new VBox(statusBar);
        container.setAlignment(Pos.CENTER);
        return container;
    }

    private void showWelcomeMessage() {
        Alert alert = new Alert(Alert.AlertType.INFORMATION);
        alert.setTitle("Welcome to Nets!");
        alert.setHeaderText("🎮 Game Ready!");
        alert.setContentText(
                "Board Size: " + currentRows + "×" + currentCols + "\n\n" +
                        "🖱️ Left Click: Rotate clockwise\n" +
                        "🖱️ Right Click: Lock/Unlock tile\n\n" +
                        "Connect all tiles to win!\n" +
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

        Button loadButton = new Button("Load Saved");
        loadButton.setStyle(
                "-fx-background-color: #0f3460; " +
                        "-fx-text-fill: white; " +
                        "-fx-font-size: 14px; " +
                        "-fx-padding: 10 20; " +
                        "-fx-cursor: hand; " +
                        "-fx-background-radius: 5;"
        );
        loadButton.setOnMouseEntered(e -> loadButton.setOpacity(0.8));
        loadButton.setOnMouseExited(e -> loadButton.setOpacity(1.0));
        loadButton.setOnAction(e -> controller.loadExistingGame());

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

        controls.getChildren().addAll(newGameButton, resetButton, loadButton, helpButton);
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

        Label goal = new Label("🎯 Goal: Connect all tiles to form a single network");
        goal.setStyle("-fx-font-weight: bold; -fx-font-size: 14px;");

        Label controls = new Label(
                "🖱️ Controls:\n" +
                        "• Left Click: Rotate tile clockwise (90°)\n" +
                        "• Right Click: Lock/Unlock tile\n" +
                        "• Locked tiles (with lock icon) cannot be rotated"
        );

        Label tiles = new Label(
                "🔧 Tile Types:\n" +
                        "• Straight: Connects two opposite sides\n" +
                        "• Corner: Connects two adjacent sides\n" +
                        "• T-Junction: Connects three sides\n" +
                        "• Cross: Connects all four sides\n" +
                        "• Power: Power source (locked)\n" +
                        "• PC: Computer terminal"
        );

        Label win = new Label(
                "🏆 Win Condition:\n" +
                        "• All tiles connected (1 component)\n" +
                        "• No loose ends (0 loose ends)\n" +
                        "• No loops"
        );
        win.setStyle("-fx-font-weight: bold;");

        Label cpu = new Label(
                "🤖 CPU Mode:\n" +
                        "After your move, the CPU will make its move automatically.\n" +
                        "The C++ engine determines the CPU's strategy."
        );

        content.getChildren().addAll(goal, new Separator(), controls, new Separator(),
                tiles, new Separator(), win, new Separator(), cpu);

        alert.getDialogPane().setContent(content);
        alert.getDialogPane().setPrefWidth(500);
        alert.showAndWait();
    }

    public static void main(String[] args) {
        launch(args);
    }
}