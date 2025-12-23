package com.nets.util;

import java.io.IOException;
import java.nio.file.*;
import java.util.function.Consumer;

public class FileWatcherService {

    private final Path path;
    private final Consumer<Path> onModified;
    private WatchService watchService;
    private volatile boolean running = true;

    public FileWatcherService(Path path, Consumer<Path> onModified) {
        this.path = path;
        this.onModified = onModified;
    }

    public void start() throws IOException {
        watchService = FileSystems.getDefault().newWatchService();
        Path dir = path.toAbsolutePath().getParent();
        dir.register(watchService, StandardWatchEventKinds.ENTRY_MODIFY);

        Thread watcherThread = new Thread(() -> {
            while (running) {
                try {
                    WatchKey key = watchService.take();
                    for (WatchEvent<?> event : key.pollEvents()) {
                        if (event.kind() == StandardWatchEventKinds.ENTRY_MODIFY) {
                            Path modifiedFile = (Path) event.context();
                            if (modifiedFile.equals(path.getFileName())) {
                                onModified.accept(path);
                            }
                        }
                    }
                    key.reset();
                } catch (InterruptedException | ClosedWatchServiceException e) {
                    // Stop watching if the service is closed or interrupted
                    running = false;
                }
            }
        });
        watcherThread.setDaemon(true);
        watcherThread.start();
    }

    public void stop() throws IOException {
        running = false;
        if (watchService != null) {
            watchService.close();
        }
    }
}
