let operatorLevel = 1;
let engineerLevel = 9;

  function userAccess(level){
    if (level == engineerLevel) {
        // Selecting element
        var elem = document.getElementById("report");     
        elem.className = "enable-link";  // Add or replace all classes with note class

        // Selecting element
        var elem = document.getElementById("login");     
        elem.className = "disabled-link";  // Add or replace all classes with note class

        // Selecting element
        var elem = document.getElementById("config");     
        elem.className = "enable-link";  // Add or replace all classes with note class

        // Selecting element
        var elem = document.getElementById("logout");     
        elem.className = "enable-link";  // Add or replace all classes with note class
    }

    else if (level == operatorLevel){
        // Selecting element
        var elem = document.getElementById("report");     
        elem.className = "enable-link";  // Add or replace all classes with note class

        // Selecting element
        var elem = document.getElementById("login");     
        elem.className = "disabled-link";  // Add or replace all classes with note class

        // Selecting element
        var elem = document.getElementById("config");     
        elem.className = "disabled-link";  // Add or replace all classes with note class

        // Selecting element
        var elem = document.getElementById("logout");     
        elem.className = "enable-link";  // Add or replace all classes with note class
    }

    else{
        // Selecting element
        var elem = document.getElementById("report");     
        elem.className = "disabled-link";  // Add or replace all classes with note class

        // Selecting element
        var elem = document.getElementById("login");     
        elem.className = "enable-link";  // Add or replace all classes with note class

        // Selecting element
        var elem = document.getElementById("config");     
        elem.className = "disabled-link";  // Add or replace all classes with note class

        // Selecting element
        var elem = document.getElementById("logout");     
        elem.className = "disabled-link";  // Add or replace all classes with note class
    }
}

