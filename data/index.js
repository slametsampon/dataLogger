var tempArray = [ -9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999 ];
var humdArray = [ -9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999 ];
var t = 20;
var h = 60;
var sensorCfg;
var activeUser;

function updateGraph(t,h)
{
  drawGraph('graph',
    sensorCfg.Temperature.lowRange,
    sensorCfg.Humidity.highRange,
    true, t, h);
}

function processReceivedData(evt) 
{
  var data = evt.data;
  console.log(data);

  var tmp = data.split('|');

  // convert the received string in to numbers
  var h = parseInt(tmp[0]);
  var t = parseInt(tmp[1]);
  //updateValues(h,t);
  //updateValues(60,20);
}

function setupWidgets(data){
  sensorCfg = JSON.parse(data);

  drawDial('canvasTemp',  '#0b9106', 160, 20,
    sensorCfg.Temperature.lowRange,
    sensorCfg.Temperature.highRange,
    sensorCfg.Temperature.highLimit,
    t);

  drawDial('canvasHumid', '#aaaaff', 160, 20,
    sensorCfg.Humidity.lowRange,
    sensorCfg.Humidity.highRange,
    sensorCfg.Humidity.highLimit,
    h);
  
  
  drawGraph('graph',
    sensorCfg.Temperature.lowRange,
    sensorCfg.Humidity.highRange,
    false, t, h);

  var myVarTime = setInterval(updateTime, 1000); 
}

function updateValues(data){
  const sensor = JSON.parse(data);

  var t = sensor.Temperature.value;
  var statusT = sensor.Temperature.status;

  var h = sensor.Humidity.value;
  var statusH = sensor.Humidity.status;

  document.getElementById('temp').innerHTML = t;
  document.getElementById('StatusTemp').innerHTML = statusT;

  document.getElementById('humd').innerHTML = h;
  document.getElementById('StatusHumd').innerHTML = statusH;

  drawDial('canvasTemp',  '#0b9106', 160, 20,
    sensorCfg.Temperature.lowRange,
    sensorCfg.Temperature.highRange,
    sensorCfg.Temperature.highLimit,
    t);

  drawDial('canvasHumid', '#aaaaff', 160, 20,
    sensorCfg.Humidity.lowRange,
    sensorCfg.Humidity.highRange,
    sensorCfg.Humidity.highLimit,
    h);
  
  
  drawGraph('graph',
    sensorCfg.Temperature.lowRange,
    sensorCfg.Humidity.highRange,
    false, t, h);

  updateGraph(t,h);
}

function updateTime() 
{  
   var d = new Date();
   var t = d.toLocaleTimeString();
   document.getElementById('time').innerHTML = t;
}

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      updateValues(this.responseText);
    }
  };
  xhttp.open("GET", "/getSensor", true);
  xhttp.send();
}, 10000 ) ;

// This is executed after the document has finished loading.
function setupIndex(){
  var index_url = ["getActiveUser", "getSensorCfg"];
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
            else if(i == 1)setupWidgets(this.responseText);
            loop(i + 1, length);
          }
      }
      request.send();
  })(0, index_url.length);
}

document.addEventListener('DOMContentLoaded', setupIndex, false);
