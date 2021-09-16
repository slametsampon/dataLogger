
// This is executed after the document has finished loading.
function setupConfig(){
    var index_url = ["getActiveUser"];
    var request = new XMLHttpRequest();
    (function loop(i, length) {
        if (i>= length) {
            return;
        }
        var url = "/" + index_url[i];
    
        request.open("GET", url, true);
        request.onreadystatechange = function() {
            if(request.readyState === XMLHttpRequest.DONE && request.status === 200) {
              if(i == 0)userAccess(this.responseText);
              loop(i + 1, length);
            }
        }
        request.send();
    })(0, index_url.length);
  }

document.addEventListener('DOMContentLoaded', setupConfig, false);
