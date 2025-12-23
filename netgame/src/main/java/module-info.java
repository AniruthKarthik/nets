module com.mets      {
    requires javafx.controls;
    requires javafx.fxml;
    requires com.google.gson;


    opens com.nets to javafx.fxml;
    exports com.nets;
    exports com.nets.controller;
    opens com.nets.controller to javafx.fxml;
    exports com.nets.model;
    opens com.nets.model to javafx.fxml;
    exports com.nets.view;
    opens com.nets.view to javafx.fxml;
    exports com.nets.util;
    opens com.nets.util to javafx.fxml;
}