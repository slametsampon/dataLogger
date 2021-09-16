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

// This is executed after the document has finished loading.
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
      //document.getElementById("temperature").innerHTML = this.responseText;
      updateValues(this.responseText);
    }
  };
  xhttp.open("GET", "/getSensor", true);
  xhttp.send();
}, 10000 ) ;

function getSensorCfg() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      setupWidgets(this.responseText);
    }
  };
  xhttp.open("GET", "/getSensorCfg", true);
  xhttp.send();
};

function getActiveUser() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      activeUser = JSON.parse(this.responseText);
    }
  };
  xhttp.open("GET", "/getActiveUser", true);
  xhttp.send();
};

var tempArray = [ -9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999 ];
var humdArray = [ -9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999 ];
var t = 20;
var h = 60;
var sensorCfg;
var activeUser;

//document.addEventListener('DOMContentLoaded', init, false);

function setupIndex(){
  getSensorCfg();
  userAccess(9);

}

document.addEventListener('DOMContentLoaded', setupIndex, false);
