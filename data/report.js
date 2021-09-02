// arrays to hold the temperature and humidity values.
var headerArray = ["Time", "Temp\-DegC", "Humd\-\%"];
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
}

function reportBuilding() {
  var xhttp = new XMLHttpRequest();
xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var respText = this.responseText;
      //document.getElementById("json").innerHTML = respText;
      //initRandomDataTable();
      fillDataTable(respText);
      generateReportTable(table);
      generateReportTableHead(table, headerArray);
      respText = '';

    }
  };
  xhttp.open("GET", "/jsonData", true);
  xhttp.send();
}
document.addEventListener('DOMContentLoaded', reportBuilding, false);
