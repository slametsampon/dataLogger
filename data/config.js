
var prevCfg;

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

function validateForm() {
  
  //sampling time
  var data = document.forms["configForm"]["samplingTime"];
  if (data.value =="")data.value = data.placeholder;
  var x = parseInt(data.value);
  if (x < 2000) {
    alert("Sampling time min 2000");
    return false;
  }

  //afaEma
  data = document.forms["configForm"]["alfaEma"];
  if (data.value =="")data.value = data.placeholder;
  x = parseInt(data.value);
  if (x < 10) {
    alert("afaEma time min 10");
    return false;
  }

  else if (x > 100) {
    alert("afaEma time max 100");
    return false;
  }

  //alarm
  data = document.forms["configForm"]["alarmH"];
  if (data.value =="")data.value = data.placeholder;
  var aH = parseInt(data.value);

  data = document.forms["configForm"]["alarmL"];
  if (data.value =="")data.value = data.placeholder;
  var aL = parseInt(data.value);

  data = document.forms["configForm"]["indicatorH"];
  if (data.value =="")data.value = data.placeholder;
  var rH = parseInt(data.value);

  data = document.forms["configForm"]["indicatorL"];
  if (data.value =="")data.value = data.placeholder;
  var rL = parseInt(data.value);
  
  if (aL > aH) {
    alert("alarmL GT alarmH");
    return false;
  }
  else if (aL < rL) {
    alert("alarmL LT indicatorL");
    return false;
  }
  else if (aH > rH) {
    alert("alarmH GT indicatorH");
    return false;
  }
  else if (rL > rH) {
    alert("indicatorL GT indicatorH");
    return false;
  }

  //confirmation submit
  return confirm('Are you, sure you want to submit this parameters ?');  
}

// This is executed after the document has finished loading.
function setupConfig(){
  if (SIMULATION) {
    userAccess(activeUser);
  }
  else {
    //init prevCfg
    if (document.getElementById("configT").checked) prevCfg = "configT";
    else if (document.getElementById("configH").checked) prevCfg = "configH";
    var index_url = ["getActiveUser", "getSensorCfg", "getSamplingTime"];
    var request = new XMLHttpRequest();
    (function loop(i, length) {
      if (i >= length) {
        return;
      }
      var url = "/" + index_url[i];    
      request.open("GET", url, true);
      request.onreadystatechange = function () {
        if (request.readyState === XMLHttpRequest.DONE && request.status === 200) {
          var respText = this.responseText;
          if (i == 0) userAccess(respText);
          else if (i == 1) fillUpParameter(respText);
          else if (i == 2) getSamplingTime(respText);
          loop(i + 1, length);
        }
      }
      request.send();
    })(0, index_url.length);
  }
}
document.addEventListener('DOMContentLoaded', setupConfig, false);
