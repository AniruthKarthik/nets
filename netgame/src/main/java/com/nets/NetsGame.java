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
import javafx.scene.layout.Region;
import javafx.scene.layout.Priority;
import javafx.stage.Stage;

import com.nets.model.VisualStep;
import java.util.List;
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
        rowSpinner.setStyle("-fx-font-size: 14px;");
        colSpinner.setStyle("-fx-font-size: 14px;");

        Label infoLabel = new Label("Board size: 3x3 to 30x30");
        infoLabel.setStyle("-fx-text-fill: #666; -fx-font-size: 12px;");

        Label rowLabel = new Label("Rows:");
        rowLabel.setStyle("-fx-font-size: 14px; -fx-font-weight: bold;");
        Label colLabel = new Label("Columns:");
        colLabel.setStyle("-fx-font-size: 14px; -fx-font-weight: bold;");

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
                        "-fx-font-size: 14px;"
        );
        
        dialog.getDialogPane().setPrefWidth(500);
        dialog.getDialogPane().setPrefHeight(350);

        // Convert result
        dialog.setResultConverter(dialogButton -> {
            if (dialogButton == startButtonType) {
                return new int[]{rowSpinner.getValue(), colSpinner.getValue()};
            }
            return null;
        });

        return dialog.showAndWait();
    }

    private TabPane mainTabPane;
    private Tab gameTab;

    private void initializeGame() {
        // Create game board
        gameBoard = new GameBoard();

        // Create controller
        controller = new GameController(gameBoard);

        // Layout
        BorderPane gameRoot = new BorderPane();
        gameRoot.setCenter(gameBoard);
        gameRoot.setStyle("-fx-background-color: #1a1a2e;");

        mainTabPane = new TabPane();
        mainTabPane.setTabClosingPolicy(TabPane.TabClosingPolicy.SELECTED_TAB);
        gameTab = new Tab("Nets Game", gameRoot);
        gameTab.setClosable(false);
        mainTabPane.getTabs().add(gameTab);

        // Scene
        Scene scene = new Scene(mainTabPane, primaryStage.getWidth(), primaryStage.getHeight());
        primaryStage.setScene(scene);
        primaryStage.show();

        // Create side panel after scene is shown so we can use its properties
        VBox sidePanel = createSidePanel(scene);
        gameBoard.setSidePanel(sidePanel);

        // Create control buttons
        HBox controls = createControls(scene);
        gameRoot.setBottom(controls);

        // Initialize game with selected dimensions
        System.out.println("Starting new game: " + currentRows + "x" + currentCols);
        controller.initGame(currentRows, currentCols);
    }

    private VBox createSidePanel(Scene scene) {
        VBox sidePanel = new VBox();
        sidePanel.setAlignment(Pos.CENTER);
        sidePanel.setStyle("-fx-background-color: transparent;");

        // Bind panel width to scene width (approx 22%)
        sidePanel.prefWidthProperty().bind(scene.widthProperty().multiply(0.22));
        sidePanel.spacingProperty().bind(scene.heightProperty().multiply(0.03));

        Label titleLabel = new Label("AI SETTINGS");
        titleLabel.styleProperty().bind(scene.widthProperty().add(scene.heightProperty()).divide(140).asString("-fx-text-fill: #00d4ff; -fx-font-size: %fpx; -fx-font-weight: bold; -fx-letter-spacing: 1px;"));

        // Algo Dropdown Container
        VBox algoBox = new VBox();
        algoBox.setAlignment(Pos.CENTER);
        algoBox.paddingProperty().bind(javafx.beans.binding.Bindings.createObjectBinding(() -> {
            double p = scene.getWidth() * 0.015;
            return new Insets(p);
        }, scene.widthProperty()));
        
        algoBox.styleProperty().bind(scene.widthProperty().divide(1200).asString("-fx-background-color: #16213e; -fx-background-radius: 15; -fx-border-color: #0f3460; -fx-border-radius: 15; -fx-border-width: %f;"));

        Label algoLabel = new Label("Algo for AI's next move:");
        algoLabel.styleProperty().bind(scene.widthProperty().divide(100).asString("-fx-text-fill: white; -fx-font-size: %fpx; -fx-font-weight: bold;"));
        
        ChoiceBox<String> algoChoice = new ChoiceBox<>();
        algoChoice.getItems().addAll("Greedy", "Backtracking", "DP", "Divide and Conquer");
        algoChoice.setValue("Greedy");
        algoChoice.prefWidthProperty().bind(sidePanel.prefWidthProperty().multiply(0.75));
        algoChoice.styleProperty().bind(scene.widthProperty().divide(110).asString("-fx-font-size: %fpx; -fx-background-radius: 8; -fx-cursor: hand;"));
        
        algoChoice.getSelectionModel().selectedItemProperty().addListener((obs, oldVal, newVal) -> {
            if (controller != null) {
                controller.setAiAlgorithm(newVal.toLowerCase().replace(" ", ""));
            }
        });

        Label descLabel = new Label("The AI will use this algorithm to calculate its move after your turn.");
        descLabel.setWrapText(true);
        descLabel.setAlignment(Pos.CENTER);
        descLabel.styleProperty().bind(scene.widthProperty().divide(140).asString("-fx-text-fill: #aaa; -fx-font-size: %fpx; -fx-font-style: italic;"));

        Button visualizeBtn = new Button("Visualize last move");
        visualizeBtn.styleProperty().bind(scene.widthProperty().divide(120).asString("-fx-background-color: #9b59b6; -fx-text-fill: white; -fx-font-size: %fpx; -fx-padding: 8 16; -fx-cursor: hand; -fx-background-radius: 8;"));
        visualizeBtn.setOnAction(e -> openVisualizer());

        algoBox.getChildren().addAll(algoLabel, algoChoice, descLabel, visualizeBtn);
        algoBox.spacingProperty().bind(scene.heightProperty().multiply(0.015));
        
        sidePanel.getChildren().addAll(titleLabel, algoBox);
        
        return sidePanel;
    }

    private void openVisualizer() {
        if (controller == null || controller.getLastUsedAiAlgorithm() == null) {
            Alert alert = new Alert(Alert.AlertType.WARNING);
            alert.setTitle("Visualizer Unavailable");
            alert.setHeaderText("No AI Move Recorded Yet");
            alert.setContentText("The AI must perform at least one move before you can visualize its logic.\n\nPlay a move yourself or wait for the AI to respond.");
            alert.getDialogPane().setStyle("-fx-font-size: 14px;");
            alert.show();
            return;
        }

        List<VisualStep> steps = controller.getVisualizationSteps();
        if (steps == null || steps.isEmpty()) {
            Alert alert = new Alert(Alert.AlertType.INFORMATION);
            alert.setContentText("No evaluation steps were recorded for the last move.");
            alert.getDialogPane().setStyle("-fx-font-size: 14px;");
            alert.show();
            return;
        }

        com.nets.view.VisualizerView visView = new com.nets.view.VisualizerView(
            gameBoard.getGameState(), 
            steps,
            controller.getPreAiMoveRotations(),
            controller.getLastAiMove(),
            controller.formatAlgoName(controller.getLastUsedAiAlgorithm()),
            mainTabPane.getScene(),
            () -> {
                Tab current = mainTabPane.getSelectionModel().getSelectedItem();
                if (current != gameTab) {
                    mainTabPane.getTabs().remove(current);
                    mainTabPane.getSelectionModel().select(gameTab);
                }
            }
        );

        Tab visTab = new Tab("AI Visualizer", visView);
        mainTabPane.getTabs().add(visTab);
        mainTabPane.getSelectionModel().select(visTab);
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
        alert.getDialogPane().setPrefWidth(500);
        alert.getDialogPane().setStyle("-fx-font-size: 14px;");
        alert.showAndWait();
    }

    private HBox createControls(Scene scene) {
        HBox controls = new HBox();
        controls.paddingProperty().bind(javafx.beans.binding.Bindings.createObjectBinding(() -> {
             double p = scene.getHeight() * 0.015;
             return new Insets(p);
        }, scene.heightProperty()));
        controls.setAlignment(Pos.CENTER);
        controls.setStyle("-fx-background-color: #16213e;");
        controls.spacingProperty().bind(scene.widthProperty().multiply(0.015));

        String baseBtnStyle = "-fx-text-fill: white; -fx-font-weight: bold; -fx-cursor: hand; -fx-background-radius: 6;";
        javafx.beans.binding.StringBinding dynamicBtnStyle = scene.widthProperty().divide(120).asString(baseBtnStyle + "-fx-font-size: %fpx; -fx-padding: 8 16;");

        Button newGameButton = new Button("New Game");
        newGameButton.styleProperty().bind(javafx.beans.binding.Bindings.concat(dynamicBtnStyle, "-fx-background-color: #00d4ff;"));
        newGameButton.setOnAction(e -> showNewGameDialog());

        Button resetButton = new Button("Reset Board");
        resetButton.styleProperty().bind(javafx.beans.binding.Bindings.concat(dynamicBtnStyle, "-fx-background-color: #e94560;"));
        resetButton.setOnAction(e -> controller.resetGame(currentRows, currentCols));

        Button helpButton = new Button("Help");
        helpButton.styleProperty().bind(javafx.beans.binding.Bindings.concat(dynamicBtnStyle, "-fx-background-color: #0f3460;"));
        helpButton.setOnAction(e -> showHelp());

        ToggleButton solutionButton = new ToggleButton("Show Solution");
        solutionButton.styleProperty().bind(javafx.beans.binding.Bindings.concat(dynamicBtnStyle, "-fx-background-color: #ffc107; -fx-text-fill: black;"));
        solutionButton.setOnAction(e -> {
            boolean isSelected = solutionButton.isSelected();
            if (controller != null) {
                controller.toggleSolution(isSelected);
            }
        });

        Button rotateButton = new Button("Rotate Board");
        rotateButton.styleProperty().bind(javafx.beans.binding.Bindings.concat(dynamicBtnStyle, "-fx-background-color: #9b59b6;"));
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
        alert.getDialogPane().setPrefWidth(600);
        alert.getDialogPane().setPrefHeight(500);
        alert.getDialogPane().setStyle("-fx-font-size: 14px;");
        alert.showAndWait();
    }

    public static void main(String[] args) {
        launch(args);
    }
}