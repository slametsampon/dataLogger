
function updateValues(h,t)
{

  //drawDial('canvasTemp',  '#ffaaaa', 160, 20, -30,  50, t); 
  drawDial('canvasTemp',  '#0b9106', 160, 20, -30,  50, t);
  drawDial('canvasHumid', '#aaaaff', 160, 20,   0, 100, h);
  document.getElementById('temp').innerHTML = t;
  document.getElementById('humd').innerHTML = h;

  var graphCanvas = 'graph';
  var graphMin = -30;
  var graphMax = 100;
  var drawLines = true;
  drawGraph(graphCanvas, graphMin, graphMax, drawLines, t, h);
}



// ===========================================  DIAL  =========================================

function drawDial(canvasID, dialColour, startAngle, stopAngle, minVal, maxVal, dialValue)
{
  oneDegreeInRadians = Math.PI/180;
  if (stopAngle < startAngle) { stopAngle = stopAngle + 360;}

  let arcStartAngleInRadians =  oneDegreeInRadians * (startAngle-5)  ;
  let arcStopAngleInRadians  =  oneDegreeInRadians * (stopAngle+5) ;  

  var c = document.getElementById(canvasID);
  var ctx = c.getContext('2d');
  ctx.clearRect(0, 0, c.width, c.height);
  ctx.save();

  let H = c.height;
  let W = c.width;

  let arcLineWidth = W/5;
  ctx.translate(W/2, W/2);        // move coordinates 0,0 to the centre of the canvas

  // draw arc
  ctx.beginPath();
  let radius = W/2 - (arcLineWidth/2) - (W/100);      
  ctx.lineWidth = arcLineWidth;
  ctx.lineCap = 'butt';
  ctx.strokeStyle = dialColour;
  ctx.arc(0, 0, radius, arcStartAngleInRadians, arcStopAngleInRadians, false);
  ctx.stroke();


  // draw centre circle
  ctx.beginPath();
  let centerCircleRadius = W/100*3.5
  ctx.strokeStyle = '#000000';
  ctx.fillStyle = '#222222';
  ctx.lineWidth = 2;
  ctx.arc(0, 0, centerCircleRadius, 0, 2 * Math.PI, true);
  ctx.stroke();
  ctx.fill();


  // draw ticks 
  ctx.beginPath();
  ctx.lineWidth = 1;
  ctx.lineCap = 'butt';
  ctx.strokeStyle = '#333333';

  ctx.font = '12px Arial';
  ctx.fillStyle = '#333333';
  ctx.textAlign = 'center'; 
  ctx.textBaseline = 'top'; 

  let tickStartPoint = radius - (arcLineWidth/2) ;   // bottom of the arc
  let tickLength =  5/8 * arcLineWidth - 5; 

  let labelPos = radius + (arcLineWidth/2) - 2; 

  for (let angle=minVal; angle<=maxVal; angle = angle+10)
  {   
    let angleInDegrees =  (angle-minVal) *  ((stopAngle - startAngle) / (maxVal - minVal)) + startAngle  ;
    let angleInRadians = angleInDegrees * oneDegreeInRadians;

    ctx.rotate(angleInRadians );  
    ctx.moveTo(tickStartPoint, 0 );                   
    ctx.lineTo(tickStartPoint + tickLength, 0 );
    ctx.stroke();

    // draw the label at the right angle.
    // rotate the dial - 90 degree, draw the text at the new top of the dial, then rotate +90.
    // this means we use the - y axis.

    ctx.rotate(90*oneDegreeInRadians); 
    ctx.fillText(angle.toString(), 0, -labelPos );        
    ctx.rotate(-90*oneDegreeInRadians); 

    ctx.rotate(-angleInRadians);  //  this puts the dial back where it was.     
  }


  // draw the pointer

  // map the value to a degree
  let pointerAngleInDegrees =  (dialValue-minVal) *  ((stopAngle - startAngle) / (maxVal - minVal)) + startAngle  ;
  let pointerAngleInRadians = pointerAngleInDegrees * oneDegreeInRadians;

  let pointerLength = radius*0.86;
  let pointerWidth = W/100 * 2; 

  ctx.beginPath();
  ctx.lineWidth = pointerWidth;
  ctx.lineCap = 'round';  
  ctx.moveTo(0,0);
  ctx.rotate(pointerAngleInRadians);
  ctx.lineTo(pointerLength, 0);
  ctx.stroke();
  ctx.rotate(-pointerAngleInRadians);

  // reset the coordinates ready for next time    
  ctx.restore();

}




function drawGraph(canvasID, gMin, gMax, drawLines, t,h )
{

  // Graph Init - draw the graph but do not draw values.

  var c = document.getElementById(canvasID);
  var ctx = c.getContext('2d');
  ctx.clearRect(0, 0, c.width, c.height); 

  var graphWidth  = c.width;
  var graphHeight = c.height; 

  var fontSize = '10px Arial';
  var fontAdjust = 3;
  if (graphHeight < 100) { fontSize = '6px Arial'; fontAdjust = 1;}

  var numySteps = gMax - gMin;
  if (numySteps > 10) { numySteps = numySteps /10; }
  var numxSteps = 20;

  var xStep = graphWidth / numxSteps;
  var yStep = graphHeight / numySteps;


  ctx.lineWidth = 1;
  ctx.strokeStyle = '#e5e5e5';
  ctx.lineCap = 'butt';
      for (let x = 0; x < c.width; x=x+xStep) 
  { 
     ctx.moveTo(x, 0);    ctx.lineTo(x, c.height);     ctx.stroke();
  }

      for (let y = 0; y <= numySteps; y++) 
  { 
     let yPos = y * yStep;
     ctx.moveTo(0, yPos);    ctx.lineTo(c.width,yPos);    ctx.stroke();
  }

  // draw labels    
  ctx.font = fontSize;
  ctx.fillStyle = '#000000';

  // no need to draw the first or the last label
  for (let i = 1; i < numySteps; i++)
  {
       let yPos = c.height - i * yStep;
       let tmp = i * 10;
       tmp = tmp + gMin;
       let txt = tmp.toString();
       ctx.fillText(txt, 2, yPos + fontAdjust);        
  }


  // draw the  values on the graph  
  if (drawLines)
  {
      tempArray.shift();  tempArray[19] = t;
      humdArray.shift();  humdArray[19] = h;

      // Temperature        
      ctx.beginPath();
      ctx.lineWidth = 1;  
      ctx.strokeStyle = '#ff7777';  
      ctx.fillStyle   = '#ff4444';  

      // on the first value we are not coming from an existing point so we just need to move to the coordinates ready to plot value 2.
      let firstValue = true;

      for (let i = 0; i < numxSteps; i++) 
      { 
          if (tempArray[i] != -9999) 
          {
              let tmpVal = tempArray[i] ;
              let yPos = (tmpVal - gMin) * (graphHeight - 0)/( gMax - gMin ) + 0;
              yPos = graphHeight - yPos;
              let xPos = (i*xStep) + xStep;

              // draw the line
              if (firstValue)   {  ctx.moveTo(xPos, yPos);  firstValue = false;  }
              else              {  ctx.lineTo(xPos,yPos);   ctx.stroke();  }

              // draw the dot
              ctx.beginPath();  ctx.arc(xPos, yPos, 3, 0, 2 * Math.PI, false);  ctx.fill();
          }
      }


      // Humidity
      ctx.lineWidth = 1;  
      ctx.strokeStyle = '#7777ff';  
      ctx.fillStyle   = '#4444ff';    
      ctx.beginPath();

      // on the first value we are not coming from an existing point so we just need to move to the coordinates ready to plot value 2.
      firstValue = true;

      for (let i = 0; i < numxSteps; i++) 
      { 
          if (humdArray[i] != -9999) 
          {
              let tmpVal = humdArray[i] ;
              let yPos = (tmpVal - gMin) * (graphHeight - 0)/( gMax - gMin ) + 0;
              yPos = graphHeight - yPos;
              let xPos = (i*xStep) + xStep;

              // draw the line
              if (firstValue)   {  ctx.moveTo(xPos, yPos);  firstValue = false;  }
              else              {  ctx.lineTo(xPos,yPos);   ctx.stroke();  }

              // draw the dot
              ctx.beginPath();  ctx.arc(xPos, yPos, 3, 0, 2 * Math.PI, false);  ctx.fill();
          }
      }

  } // if (! initOnly)
} // function drawGraph



function processReceivedData(evt) 
{
  var data = evt.data;
  console.log(data);

  var tmp = data.split('|');

  // convert the received string in to numbers
  var h = parseInt(tmp[0]);
  var t = parseInt(tmp[1]);
  //updateValues(h,t);
  updateValues(60,20);
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
  drawDial('canvasTemp',  '#5af067', 160, 20, -30,  50, 20);
  drawDial('canvasHumid', '#aaaaff', 160, 20,   0, 100, 60);
  drawGraph('graph', -30, 100, false, t, h);

  //var myVarTime = setInterval(updateTime, 1000); 
}


// arrays to hold the temperature and humidity values.
var tempArray = [ -9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999 ];
var humdArray = [ -9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999 ];

var t = -30;
var h = 0;

document.addEventListener('DOMContentLoaded', init, false);
