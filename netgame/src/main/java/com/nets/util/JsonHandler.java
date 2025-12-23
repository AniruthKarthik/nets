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
        String content = new String(Files.readAllBytes(Paths.get(filePath)));
        content = removeJsonComments(content);

        GameState gameState = gson.fromJson(content, GameState.class);
        if (gameState != null && gameState.getGrid() == null) {
            // Initialize an empty grid if it's null after deserialization
            // This assumes a default empty state or handles cases where grid might be omitted.
            gameState.setGrid(new Tile[0][0]); 
        } else if (gameState == null) {
            // If the entire game state is null, return a new empty one
            gameState = new GameState();
            gameState.setGrid(new Tile[0][0]);
        }
        return gameState;
    }

    public static void saveGameState(GameState state, String filePath) throws IOException {
        String tempFilePath = filePath + ".tmp";
        File tempFile = new File(tempFilePath);
        File finalFile = new File(filePath);

        // Write to a temporary file
        try (FileWriter writer = new FileWriter(tempFile)) {
            gson.toJson(state, writer);
        } catch (IOException e) {
            // If writing to the temp file fails, delete it and rethrow the exception
            tempFile.delete();
            throw e;
        }

        // Rename the temporary file to the final file name
        if (!tempFile.renameTo(finalFile)) {
            // If rename fails, try to copy and delete
            try {
                Files.copy(tempFile.toPath(), finalFile.toPath(), java.nio.file.StandardCopyOption.REPLACE_EXISTING);
                if (!tempFile.delete()) {
                    System.err.println("Warning: Failed to delete temporary file: " + tempFile.getAbsolutePath());
                }
            } catch (IOException e) {
                // If copy also fails, delete the temp file and throw an exception
                tempFile.delete();
                throw new IOException("Failed to atomically save game state to " + filePath, e);
            }
        }
    }

    private static String removeJsonComments(String jsonc) {
        // Naive comment removal
        return jsonc.replaceAll("//.*|/\\*.*?\\*/", "");
    }
}