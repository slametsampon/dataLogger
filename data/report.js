// arrays to hold the temperature and humidity values.
var headerArray = ["Time", "Tempt\-°C", "Humid\-\%"];
var timeArray = [];
var tempArray = [];
var humdArray = [];

var baseReportDate = new Date();
var DEFAULT_DATE_TIME = "2022-01-04T06:59";

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
  //let dataLen = dataJson.time.length;
  let dataLen = 24;

  for (let i = 0; i < dataLen; i++) {
    if (i < 10) {
      i_str = i;
      timeArray[i] = "0" + i.toString() + ":00";
    }
    else {
      timeArray[i] = i.toString() + ":00";
    }
    tempArray[i] = dataJson.temperature[i];
    humdArray[i] = dataJson.humidity[i];
    if (typeof dataJson.temperature[i] === "undefined") {
      tempArray[i] = 0.0;
      humdArray[i] = 0.0;
    }
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

function calcDate(){
  var difDay = 0;
  if(document.getElementById("select_days").value != ""){
    let reqDay = parseInt(document.getElementById("select_days").value);
    let currDay = baseReportDate.getDay();
  
    difDay = currDay - reqDay;
  
    if (difDay == 0) difDay = 0;
    else if (difDay < 0) difDay = 7 + difDay;
  }
  return difDay;
}

function setBaseReportDate(data){
  DEFAULT_DATE_TIME = data;
  let arrDT = DEFAULT_DATE_TIME.split("T");
  let dateVal = arrDT[0];
  let timeVal = arrDT[1];

  let arrDate = dateVal.split("-");
  let year = arrDate[0];
  let month = arrDate[1] - 1;
  let dtVal = arrDate[2];

  //var d = new Date();
  baseReportDate.setFullYear(year);
  baseReportDate.setMonth(month);
  baseReportDate.setDate(dtVal);
  
  //set select day
  let day = baseReportDate.getDay();
  document.getElementById("select_days").value = day;

}

function createHeaderDate(dateVal) {
  
  var d = new Date();
  let baseDate = baseReportDate.getDate();
  d.setDate(baseDate - dateVal);

  let day = d.getDay();

  document.getElementById("id_day").innerHTML = dayName(day);
  document.getElementById("id_date").innerHTML = d.toLocaleDateString();
  
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
  createHeaderDate(calcDate());  

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
}

function getDay(date) {
  let arrDate = date.split("-");
  let year = arrDate[0];
  let month = arrDate[1] - 1;
  let dateVal = arrDate[2];

  var d = new Date();
  d.setFullYear(year);
  d.setMonth(month);
  d.setDate(dateVal);

  return d.getDay();

}

function reportBuildingSimul() {

  //clear table
  $("#table").empty();

  initRandomDataTable();
  generateReportTable(table);

  userAccess(9);
}

function getHourlyAvg(){
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
  xhttp.open("GET", "/hourlyAvgDay?days="+document.getElementById("select_days").value, true);
  xhttp.send();
}

function downloadAsPDF() {
  var dName = dayName(baseReportDate.getDay());
  if(document.getElementById("select_days").value != ""){
    dName = dayName(parseInt(document.getElementById("select_days").value));
  }
  var fileName = dName + '_ls.pdf';
  //generateToPdf('report_form', fileName,'canvas');

  var element = document.querySelector('report_form');  
  window.scrollTo(0, 0); // this will help to print if div hidden or on mobile screen

  // This row fixed problem
  element.parentNode.style.overflow = 'visible';

  const opt = {
      filename: fileName,
      margin: 2,
      image: {type: 'jpg', quality: 0.9}, 
      html2canvas: {element,
        allowTaint: true,
        useCORS: true,
        logging: false,
        letterRendering: true,
        scale: 1,
      }, 
      jsPDF: {
          format: 'a4',
          unit:'cm',
          orientation: 'portrait'
      }
  };
  html2pdf().set(opt).from(element).save();
}

// This is executed after the document has finished loading.
function setupReport(){
  if (SIMULATION) {
    userAccess(activeUser);
  }
  //set current day

  else {
    var url_day = "hourlyAvgDay?days=" + document.getElementById("select_days").value;
    var index_url = ["getActiveUser", "getDateTime",url_day];
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
          else if (i == 1) {
            setBaseReportDate(this.responseText);
            url_day = "hourlyAvgDay?days=" + document.getElementById("select_days").value;
            index_url = ["getActiveUser", "getDateTime",url_day];
          } 
          else if (i == 2) {
            fillDataTable(this.responseText);
            generateReportTable(table);
          }
          loop(i + 1, length);
        }
      }
      request.send();
    })(0, index_url.length);
  }
}
document.addEventListener('DOMContentLoaded', setupReport, false);
