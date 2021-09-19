  function userAccess(data){
    const operatorLevel = 1;
    const engineerLevel = 9;
    
    let activeUsr = JSON.parse(data);
    let level = parseInt(activeUsr.level)
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

