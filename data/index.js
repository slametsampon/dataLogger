function updateGraph(t,h)
{

  var graphCanvas = 'graph';
  var graphMin = -30;
  var graphMax = 100;
  var drawLines = true;
  drawGraph(graphCanvas, graphMin, graphMax, drawLines, t, h);
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

function updateValues(data){
  const dataJson = JSON.parse(data);

  var t = dataJson.Temperature.value;
  var statusT = dataJson.Temperature.status;

  var h = dataJson.Humidity.value;
  var statusH = dataJson.Humidity.status;

  document.getElementById('temp').innerHTML = t;
  document.getElementById('StatusTemp').innerHTML = statusT;

  document.getElementById('humd').innerHTML = h;
  document.getElementById('StatusHumd').innerHTML = statusH;

  drawDial('canvasTemp',  '#0b9106', 160, 20, -30,  50, 40, t);
  drawDial('canvasHumid', '#aaaaff', 160, 20,   0, 100, 90, h);
  updateGraph(t,h);
}

function updateTime() 
{  
   var d = new Date();
   var t = d.toLocaleTimeString();
   document.getElementById('time').innerHTML = t;
}

// This is executed after the document has finished loading.
function init() 
{
  //Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
  //Socket.onmessage = function(event) { processReceivedData(event); };

  console.log('started');
  //drawDial('canvasTemp',  '#ffaaaa', 160, 20, -30,  50, 20); 
  drawDial('canvasTemp',  '#0b9106', 160, 20, -30,  50, 40, t);
  drawDial('canvasHumid', '#aaaaff', 160, 20,   0, 100, 90, h);
  drawGraph('graph', -30, 100, false, t, h);

  var myVarTime = setInterval(updateTime, 1000); 
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

//new functions
/*
function fillDataTable(data){
  const dataJson = JSON.parse(data);
  let dataLen = dataJson.time.length;

  text = "<ul>";
  for (let i = 0; i < dataLen; i++) {
    if (i<10){
      i_str = i;
      timeArray[i] = "0" + i.toString() + ":00";
      }
      else{
          timeArray[i] = i.toString() + ":00";
      }
      tempArray[i] = dataJson.temperature[i];
      humdArray[i] = dataJson.humidity[i];
  }  
}

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      //document.getElementById("temperature").innerHTML = this.responseText;
      document.getElementById('temp').innerHTML = this.responseText;
      t = parseInt(this.responseText);
      drawDial('canvasTemp',  '#0b9106', 160, 20, -30,  50, 40, t);
      updateGraph(t,h);
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      //document.getElementById("humidity").innerHTML = this.responseText;
      document.getElementById('humd').innerHTML = this.responseText;
      h = parseInt(this.responseText);
      drawDial('canvasHumid', '#aaaaff', 160, 20,   0, 100, 90, h);
      updateGraph(t,h);
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000 ) ;

*/

var tempArray = [ -9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999 ];
var humdArray = [ -9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999 ];
var t = 20;
var h = 60;

//document.addEventListener('DOMContentLoaded', init, false);

function setupIndex(){
    init();
    userAccess(9);
}

document.addEventListener('DOMContentLoaded', setupIndex, false);
