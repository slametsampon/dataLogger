

function loginStatus(data){

  if (data == "INVALID_LOGIN"){
    document.getElementById("loginStatus").innerHTML = "Invalid username or password";
  }

}
  
// This is executed after the document has finished loading.
function setupLogin() {
  if (SIMULATION) {
    userAccess(activeUser);
  }
  else {
    var index_url = ["getActiveUser", "loginStatus"];
    var request = new XMLHttpRequest();
    (function loop(i, length) {
      if (i >= length) {
        return;
      }
      var url = "/" + index_url[i];    
      request.open("GET", url, true);
      request.onreadystatechange = function () {
        if (request.readyState === XMLHttpRequest.DONE && request.status === 200) {
          if (i == 0) userAccess(this.responseText);
          if (i == 1) loginStatus(this.responseText);
          loop(i + 1, length);
        }
      }
      request.send();
    })(0, index_url.length);
  }
}
document.addEventListener('DOMContentLoaded', setupLogin, false);
