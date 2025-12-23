module com.nets {
    requires javafx.controls;
    requires javafx.fxml;
    requires com.google.gson;


    opens com.nets to javafx.fxml;
    exports com.nets;
    exports com.nets.controller;
    opens com.nets.controller to javafx.fxml;
    exports com.nets.model;
    opens com.nets.model to javafx.fxml, com.google.gson;
    exports com.nets.view;
    opens com.nets.view to javafx.fxml;
}