const SIMULATION = false;
const OPERATOR_LEVEL = 1;
const ENGINEER_LEVEL = 9;

var activeUser ={
      "username":"engineer",
      "password":"123456",
      "email":"engineer@example.com",
      "level":9
    };

function userAccess(data) {
    if (SIMULATION) activeUser = data;
    else activeUser = JSON.parse(data);    
    let user = 'GST';
    let level = parseInt(activeUser.level)
    if (level == ENGINEER_LEVEL){
        user = 'ENG';
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
    else if (level == OPERATOR_LEVEL){
        user = 'OPR';
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
    //default user = guest
    else {
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
    //display active user
    document.getElementById('activeUser').innerHTML = user;
}

