package com.nets.util;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.nets.model.GameState;
import com.nets.model.Tile;
import com.nets.model.TileType;

import java.io.*;
import java.nio.file.Files;
import java.nio.file.Paths;

public class JsonHandler {
    private static final Gson gson = new GsonBuilder().setPrettyPrinting().create();

    public static GameState loadGameState(String filePath) throws IOException {
        // Read file and remove comments (JSONC support)
        String content = new String(Files.readAllBytes(Paths.get(filePath)));
        content = removeJsonComments(content);

        // Parse JSON with custom deserialization
        GameState state = gson.fromJson(content, GameState.class);

        // Convert grid from nested array format
        if (state != null && state.getGrid() != null) {
            processGrid(state);
        }

        return state;
    }

    private static void processGrid(GameState state) {
        // The grid is already loaded, just ensure tile types are correct
        Tile[][] grid = state.getGrid();
        for (int i = 0; i < grid.length; i++) {
            for (int j = 0; j < grid[i].length; j++) {
                if (grid[i][j] != null) {
                    // Ensure type is properly set
                    if (grid[i][j].getType() == null) {
                        grid[i][j].setType(TileType.EMPTY);
                    }
                }
            }
        }
    }

    public static void saveGameState(GameState state, String filePath) throws IOException {
        String json = gson.toJson(state);
        try (FileWriter writer = new FileWriter(filePath)) {
            writer.write(json);
        }
    }

    private static String removeJsonComments(String jsonc) {
        // Remove single-line comments
        jsonc = jsonc.replaceAll("//.*", "");
        // Remove multi-line comments
        jsonc = jsonc.replaceAll("/\\*.*?\\*/", "");
        return jsonc;
    }

    public static void callCppEngine(String inputFile, String outputFile) throws IOException, InterruptedException {
        File engineFile = new File("./nets_engine");
        if (!engineFile.exists() || !engineFile.canExecute()) {
            // Try alternative path
            engineFile = new File("nets_engine");
            if (!engineFile.exists()) {
                throw new IOException("C++ engine not found. Using standalone mode.");
            }
        }

        ProcessBuilder pb = new ProcessBuilder(engineFile.getAbsolutePath(), inputFile, outputFile);
        pb.directory(new File("."));
        pb.redirectErrorStream(true);

        Process process = pb.start();

        // Capture output
        BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
        String line;
        StringBuilder output = new StringBuilder();
        while ((line = reader.readLine()) != null) {
            System.out.println("[C++ Engine] " + line);
            output.append(line).append("\n");
        }

        int exitCode = process.waitFor();
        if (exitCode != 0) {
            throw new IOException("C++ engine returned error code: " + exitCode + "\nOutput: " + output);
        }
    }

    public static boolean isCppEngineAvailable() {
        File engine = new File("./nets_engine");
        return engine.exists() && engine.canExecute();
    }
}