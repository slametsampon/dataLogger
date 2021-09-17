// arrays to hold the temperature and humidity values.
var headerArray = ["Time", "Tempt\-°C", "Humid\-\%"];
var timeArray = [];
var tempArray = [];
var humdArray = [];

function getRndInteger(min, max) {
    return Math.floor(Math.random() * (max - min + 1) ) + min;
  }
  
function initRandomDataTable(){
    for (let i = 0; i < 24; i++) {
        if (i<10){
            i_str = i;
            timeArray[i] = "0" + i.toString() + ":00";
        }
        else{
            timeArray[i] = i.toString() + ":00";
        }
        tempArray[i] = getRndInteger(20,49)/10 + 20;
        humdArray[i] = getRndInteger(20,49)/10 + 70;
    }
}

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

let table = document.querySelector("table");

function dayName(dayVal){
  var dName = "";

  if (dayVal == 0)dName = "Sunday";
  else if (dayVal == 1)dName = "Monday";
  else if (dayVal == 2)dName = "Tuesday";
  else if (dayVal == 3)dName = "Wednesday";
  else if (dayVal == 4)dName = "Thursday";
  else if (dayVal == 5)dName = "Friday";
  else if (dayVal == 6)dName = "Saturday";

  return dName;
}

var reqDate = 0;
function calcDate(){
  if(document.getElementById("days").value != ""){
    let reqDay = parseInt(document.getElementById("days").value);
    //let reqDay = 5;
  
    let d = new Date();
    let currDay = d.getDay();
  
    let difDay = currDay - reqDay;
  
    if (difDay == 0) difDay = 0;
    else if (difDay < 0) difDay = 7 + difDay;
    reqDate = difDay;  
  }
  else reqDate = 0;
}

function createCaption(table, dateVal){
  var d = new Date();
  let currDate = d.getDate();
  d.setDate(currDate - dateVal);

  let day = d.getDay();

  var t = d.toLocaleDateString();
  t = dayName(day) + ", " + t;
  
  let caption = table.createCaption();
  caption.textContent = 'Hourly Average : ' + t;
}

function generateReportTableHead(table, dataHeader) {
  let thead = table.createTHead();
  let row = thead.insertRow();
  for (let key of dataHeader) {
    let th = document.createElement("th");
    let text = document.createTextNode(key);
    th.appendChild(text);
    row.appendChild(th);
  }
}

function generateReportTable(table) {

  let fLen = tempArray.length;
  for (let i = 0; i < fLen; i++) {
    let row = table.insertRow();

    let cell = row.insertCell();
    let text = document.createTextNode(timeArray[i]);
    cell.appendChild(text);

    let cell1 = row.insertCell();
    let text1 = document.createTextNode(tempArray[i]);
    cell1.appendChild(text1);

    let cell2 = row.insertCell();
    let text2 = document.createTextNode(humdArray[i]);
    cell2.appendChild(text2);
  }

  generateReportTableHead(table, headerArray);
  createCaption(table,reqDate);  
}

function reportBuildingSimul() {

  //clear table
  $("#table").empty();

  initRandomDataTable();
  generateReportTable(table);

  userAccess(9);
}

function getHourlyAvg(){
  //calculate date
  calcDate();

  //clear table
  $("#table").empty();

  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var respText = this.responseText;
      fillDataTable(respText);
      generateReportTable(table);
    }
  };
  xhttp.open("GET", "/hourlyAvgDay?days="+document.getElementById("days").value, true);
  xhttp.send();
}

function setupReport(){
  var url_day = "hourlyAvgDay?days="+document.getElementById("days").value;
  var index_url = ["getActiveUser", url_day];
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
            else if(i == 1){
              fillDataTable(this.responseText);
              generateReportTable(table);
            }
            loop(i + 1, length);
          }
      }
      request.send();
  })(0, index_url.length);
}

//document.addEventListener('DOMContentLoaded', reportBuildingSimul, false);
document.addEventListener('DOMContentLoaded', setupReport, false);
