var tempArray = [ -9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999 ];
var humdArray = [ -9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999 ];

var sensorCfg = {
    "Temperature": {
      "unit":"°C",
      "value":20.25,
      "highRange":50.0,
      "lowRange":0.0,
      "highLimit":26.0,
      "lowLimit":16.0,
      "alfaEma":80.0,
      "alarm":0
    },
    "Humidity": {
      "unit":"%",
      "value":80.75,
      "highRange":100.0,
      "lowRange":0.0,
      "highLimit":65.0,
      "lowLimit":40.0,
      "alfaEma":80.0,
      "alarm":0
    }
};
  
var sensor = {
    "Temperature": {
      "value":20.25,
      "alarm":0
    },
    "Humidity": {
      "value":80.75,
      "alarm":0
    }
};

function random(min, max) {
  const num = Math.floor(Math.random() * (max - min + 1)) + min;
  return num;
}

function simulateSensor() {
  let deltaT = sensorCfg.Temperature.highLimit - sensorCfg.Temperature.lowLimit;
  let deltaH = sensorCfg.Humidity.highLimit - sensorCfg.Humidity.lowLimit;

  let t = random(0, deltaT) + sensorCfg.Temperature.lowLimit;
  let h = random(0, deltaH) + sensorCfg.Humidity.lowLimit;
  let alarmT = 0;
  let alarmH = 0;

  if (t >= sensorCfg.Temperature.highLimit) alarmT = 2;
  if (h >= sensorCfg.Humidity.highLimit) alarmH = 2;

  sensor.Temperature.value = t;
  sensor.Humidity.value = h;
  sensor.Temperature.alarm = alarmT;
  sensor.Humidity.alarm = alarmH;

  return sensor;
}

function updateGraph(t, h)
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
}

function setupWidgets(data) {
  
  if (SIMULATION) sensorCfg = data;
  else sensorCfg = JSON.parse(data);

  console.log('sensorCfg : ', sensorCfg);

  let dialTemp = new Dial('canvasTemp', '#0b9106', 160, 20,
    sensorCfg.Temperature.lowRange,
    sensorCfg.Temperature.highRange,
    sensorCfg.Temperature.lowLimit,
    sensorCfg.Temperature.highLimit,
    sensorCfg.Temperature.unit);

  let dialHumid = new Dial('canvasHumid',  '#aaaaff', 160, 20,
    sensorCfg.Humidity.lowRange,
    sensorCfg.Humidity.highRange,
    sensorCfg.Humidity.lowLimit,
    sensorCfg.Humidity.highLimit,
    sensorCfg.Humidity.unit);
  let trendHT = new Trending('graph', sensorCfg.Temperature.lowRange, sensorCfg.Humidity.highRange);

  dialTemp.drawBody();
  dialHumid.drawBody();
  trendHT.drawFrame();
  
  dialTemp.drawPointer(sensorCfg.Temperature.value, sensorCfg.Temperature.alarm);
  dialHumid.drawPointer(sensorCfg.Humidity.value, sensorCfg.Humidity.alarm);

  if (SIMULATION) var myVarTime = setInterval(updateTime, 6000);
  else var myVarTime = setInterval(updateTime, 1000); 
}

function initTrendingData(data) {
  let trendHT = new Trending('graph', sensorCfg.Temperature.lowRange, sensorCfg.Humidity.highRange);
  let lastTemp = 0;
  let lastHumd = 0;
  if (SIMULATION) {
    //init trending values
    for (let index = 0; index < tempArray.length; index++) {
      simulateSensor();
      tempArray[index] = sensor.Temperature.value;
      humdArray[index] = sensor.Humidity.value;
      lastTemp = tempArray[index];
      lastHumd = humdArray[index];
    }    
  }
  else {
    console.log('data : ', data);
    let initData = JSON.parse(data);
    console.log('initData : ', initData);
    for (let index = 0; index < tempArray.length; index++) {
      if (initData.humidity[index] > 0) {
        tempArray[index] = initData.temperature[index];
        humdArray[index] = initData.humidity[index];
        lastTemp = tempArray[index];
        lastHumd = humdArray[index];
      }
    }    
  }
  trendHT.drawLine(lastTemp, lastHumd);
}

function updateValues(data) {
  if (SIMULATION) sensor = data;
  else sensor = JSON.parse(data);

  let dialTemp = new Dial('canvasTemp', '#0b9106', 160, 20,
    sensorCfg.Temperature.lowRange,
    sensorCfg.Temperature.highRange,
    sensorCfg.Temperature.lowLimit,
    sensorCfg.Temperature.highLimit,
    sensorCfg.Temperature.unit);

  let dialHumid = new Dial('canvasHumid',  '#aaaaff', 160, 20,
    sensorCfg.Humidity.lowRange,
    sensorCfg.Humidity.highRange,
    sensorCfg.Humidity.lowLimit,
    sensorCfg.Humidity.highLimit,
    sensorCfg.Humidity.unit);
  let trendHT = new Trending('graph', sensorCfg.Temperature.lowRange, sensorCfg.Humidity.highRange);

  dialTemp.drawBody();
  dialHumid.drawBody();
  trendHT.drawFrame();

  dialTemp.drawPointer(sensor.Temperature.value, sensor.Temperature.alarm);
  dialHumid.drawPointer(sensor.Humidity.value,sensor.Humidity.alarm);
  trendHT.drawLine(sensor.Temperature.value, sensor.Humidity.value);

}

function updateTime() {  
  var d = new Date();
  var t = d.toLocaleTimeString();
  document.getElementById('time').innerHTML = t;
  
  if (SIMULATION) updateValues(simulateSensor());
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
function setupIndex() {
  if (SIMULATION) {
    userAccess(activeUser);
    setupWidgets(sensorCfg);
    initTrendingData('dummy-data-simulation');
  }

  else {
    var index_url = ["getActiveUser", "getSensorCfg", "getTrendingData"];
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
              else if(i == 2)initTrendingData(this.responseText);
              loop(i + 1, length);
            }
        }
        request.send();
    })(0, index_url.length);
  }
}

document.addEventListener('DOMContentLoaded', setupIndex, false);
