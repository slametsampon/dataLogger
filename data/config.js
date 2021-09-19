
// This is executed after the document has finished loading.
var prevCfg;
function setupConfig(){

  //init prevCfg
  if(document.getElementById("configT").checked) prevCfg = "configT";
  else if(document.getElementById("configH").checked) prevCfg = "configH";

  var index_url = ["getActiveUser", "getSensorCfg", "getSamplingTime"];
  var request = new XMLHttpRequest();
  (function loop(i, length) {
      if (i>= length) {
          return;
      }
      var url = "/" + index_url[i];
  
      request.open("GET", url, true);
      request.onreadystatechange = function() {
          if(request.readyState === XMLHttpRequest.DONE && request.status === 200) {
            var respText = this.responseText;
            if(i == 0)userAccess(respText);
            else if(i == 1)fillUpParameter(respText);
            else if(i == 2)getSamplingTime(respText);
            loop(i + 1, length);
          }
      }
      request.send();
  })(0, index_url.length);
}

function getSensorCfg(){

  //check change on value config
  var currCfg;
  if(document.getElementById("configT").checked) currCfg = "configT";
  else if(document.getElementById("configH").checked) currCfg = "configH";

  if(prevCfg == currCfg) return;
  prevCfg = currCfg;

  var index_url = ["getSensorCfg"];
  var request = new XMLHttpRequest();
  (function loop(i, length) {
      if (i>= length) {
          return;
      }
      var url = "/" + index_url[i];
  
      request.open("GET", url, true);
      request.onreadystatechange = function() {
          if(request.readyState === XMLHttpRequest.DONE && request.status === 200) {
            var respText = this.responseText;
            if(i == 0)fillUpParameter(respText);
            loop(i + 1, length);
          }
      }
      request.send();
  })(0, index_url.length);
}

function fillUpParameter(data){
  
  let sensorCfg = JSON.parse(data);

  var element;
  if(document.getElementById("configT").checked){

    element = document.getElementById("id_alfaEma");
    element.placeholder = sensorCfg.Temperature.alfaEma.toString();
    element.value = element.placeholder;

    element = document.getElementById("id_alarmH");
    element.placeholder = sensorCfg.Temperature.highLimit.toString();
    element.value = element.placeholder;

    element = document.getElementById("id_alarmL");
    element.placeholder = sensorCfg.Temperature.lowLimit.toString();
    element.value = element.placeholder;

    element = document.getElementById("id_indicatorH");
    element.placeholder = sensorCfg.Temperature.highRange.toString();
    element.value = element.placeholder;

    element = document.getElementById("id_indicatorL");
    element.placeholder = sensorCfg.Temperature.lowRange.toString();
    element.value = element.placeholder;
  }
  else if(document.getElementById("configH").checked){
    element = document.getElementById("id_alfaEma");
    element.placeholder = sensorCfg.Humidity.alfaEma.toString();
    element.value = element.placeholder;

    element = document.getElementById("id_alarmH");
    element.placeholder = sensorCfg.Humidity.highLimit.toString();
    element.value = element.placeholder;

    element = document.getElementById("id_alarmL");
    element.placeholder = sensorCfg.Humidity.lowLimit.toString();
    element.value = element.placeholder;

    element = document.getElementById("id_indicatorH");
    element.placeholder = sensorCfg.Humidity.highRange.toString();
    element.value = element.placeholder;

    element = document.getElementById("id_indicatorL");
    element.placeholder = sensorCfg.Humidity.lowRange.toString();
    element.value = element.placeholder;
  }
}

function getSamplingTime(data){
  var element = document.getElementById("id_samplingTime");
  element.placeholder = data;
  element.value = element.placeholder;
}

document.addEventListener('DOMContentLoaded', setupConfig, false);
