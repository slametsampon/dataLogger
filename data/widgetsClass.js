// ===========================================  DIAL  =========================================

class Dial{
    constructor(canvasID, color,
        startAngle, stopAngle, minVal, maxVal,
        lowAlarm, highAlarm, unit) {
        
        this.canvasID = canvasID;
        this.color = color;
        this.startAngle = startAngle;
        this.stopAngle = stopAngle;
        this.minVal = minVal;
        this.maxVal = maxVal;
        this.lowAlarm = lowAlarm;
        this.highAlarm = highAlarm;
        this.unit = unit;       
    }
    
    drawBody() {
        let oneDegreeInRadians = Math.PI/180;
        if (this.stopAngle < this.startAngle) { this.stopAngle = this.stopAngle + 360;}

        // map the highAlarm to a degree
        let highAlarmAngle =  (this.highAlarm - this.minVal) *  ((this.stopAngle - this.startAngle) / (this.maxVal - this.minVal)) + this.startAngle  ;
        let arcHighAlarmAngleInRadians =  oneDegreeInRadians * highAlarmAngle;

        // map the lowAlarm to a degree
        let lowAlarmAngle =  (this.lowAlarm - this.minVal) *  ((this.stopAngle - this.startAngle) / (this.maxVal - this.minVal)) + this.startAngle  ;
        let arcLowAlarmAngleInRadians =  oneDegreeInRadians * lowAlarmAngle;

        let arcStartAngleInRadians =  oneDegreeInRadians * (this.startAngle-5);
        let arcStopAngleInRadians  =  oneDegreeInRadians * (this.stopAngle+5); 


        let c = document.getElementById(this.canvasID);
        let ctx = c.getContext('2d');
        ctx.clearRect(0, 0, c.width, c.height);
        ctx.save();

        let H = c.height;
        let W = c.width;

        let arcLineWidth = W / 5;
        ctx.translate(W/2, W/2);        // move coordinates 0,0 to the centre of the canvas

        // draw arc
        ctx.beginPath();
        let radius = W/2 - (arcLineWidth/2) - (W/100);      
        ctx.lineWidth = arcLineWidth;
        ctx.lineCap = 'butt';
        ctx.strokeStyle = this.color;
        ctx.arc(0, 0, radius, arcStartAngleInRadians, arcStopAngleInRadians, false);
        ctx.stroke();

        // draw High Alarm arc
        ctx.beginPath();
        ctx.strokeStyle = '#ff0000';
        ctx.arc(0, 0, radius, arcHighAlarmAngleInRadians, arcStopAngleInRadians, false);
        ctx.stroke();

        // draw Low Alarm arc
        ctx.beginPath();
        ctx.strokeStyle = '#fcfc00';
        ctx.arc(0, 0, radius, arcStartAngleInRadians, arcLowAlarmAngleInRadians, false);
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

        let labelPos = radius + (arcLineWidth / 2) - 2;
        
        let dialStep = 10;
        let incAngle = (this.maxVal - this.minVal) / dialStep;
        if (incAngle <= 5) { incAngle = 5; }
        else { incAngle = 10;}

        for (let angle = this.minVal; angle <= this.maxVal; angle = angle + incAngle){   
            let angleInDegrees =  (angle - this.minVal) *  ((this.stopAngle - this.startAngle) / (this.maxVal - this.minVal)) + this.startAngle  ;
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
    }

    drawPointer(dialValue, alarm) {
        let oneDegreeInRadians = Math.PI/180;
        let c = document.getElementById(this.canvasID);
        let ctx = c.getContext('2d');

        let H = c.height;
        let W = c.width;

        let arcLineWidth = W / 5;
        let radius = W/2 - (arcLineWidth/2) - (W/100);      
        
        // map the value to a degree
        let pointerAngleInDegrees =  (dialValue - this.minVal) *  ((this.stopAngle - this.startAngle) / (this.maxVal - this.minVal)) + this.startAngle  ;
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

        //display value
        let str = dialValue.toString() + this.unit;
        let strDim = ctx.measureText(str);
        let strLength = strDim.width;
        const ext = 10;
        let xOff = -(ext + strLength / 2);
        let yOff = H / 4 - ext/2;

        //Alarm indicator
        ctx.fillStyle = '#0b9106';
        if (alarm > 0)ctx.fillStyle = '#ff0000';
        ctx.fillRect(xOff, yOff, strLength + 2*ext, 20);

        ctx.fillStyle = '#333333';
        ctx.font = '15px Arial';
        ctx.fillText(str, 0, H / 4);

        // reset the coordinates ready for next time    
        ctx.restore();
    }    
}

class Trending{
    constructor(canvasID, rangeMin, rangeMax) {
        this.canvasID = canvasID;
        this.rangeMin = rangeMin;
        this.rangeMax = rangeMax;

        let numySteps = this.rangeMax - this.rangeMin;
        if (numySteps > 10) { numySteps = numySteps /10; }
        let numxSteps = 24;

        this.numySteps = numySteps;
        this.numxSteps = numxSteps;
    }

    drawFrame() {
        // Graph Init - draw the graph but do not draw values.

        let c = document.getElementById(this.canvasID);
        let ctx = c.getContext('2d');
        ctx.clearRect(0, 0, c.width, c.height); 

        let graphWidth  = c.width;
        let graphHeight = c.height; 

        let fontSize = '10px Arial';
        let fontAdjust = 3;
        if (graphHeight < 100) { fontSize = '6px Arial'; fontAdjust = 1;}

        let xStep = graphWidth / this.numxSteps;
        let yStep = graphHeight / this.numySteps;


        ctx.lineWidth = 1;
        ctx.strokeStyle = '#e5e5e5';
        ctx.lineCap = 'butt';
            for (let x = 0; x < c.width; x=x+xStep) 
        { 
            ctx.moveTo(x, 0);    ctx.lineTo(x, c.height);     ctx.stroke();
        }

            for (let y = 0; y <= this.numySteps; y++) 
        { 
            let yPos = y * yStep;
            ctx.moveTo(0, yPos);    ctx.lineTo(c.width,yPos);    ctx.stroke();
        }

        // draw labels    
        ctx.font = fontSize;
        ctx.fillStyle = '#000000';

        // no need to draw the first or the last label
        for (let i = 1; i < this.numySteps; i++)
        {
            let yPos = c.height - i * yStep;
            let tmp = i * 10;
            tmp = tmp + this.rangeMin;
            let txt = tmp.toString();
            ctx.fillText(txt, 2, yPos + fontAdjust);        
        }        
    }

    drawLine(t, h) {
        let c = document.getElementById(this.canvasID);
        let ctx = c.getContext('2d');

        let graphWidth  = c.width;
        let graphHeight = c.height; 

        let xStep = graphWidth / this.numxSteps;
        let yStep = graphHeight / this.numySteps;

        tempArray.shift(); tempArray[23] = t;
        humdArray.shift();  humdArray[23] = h;

        // Temperature        
        ctx.beginPath();
        ctx.lineWidth = 1;  
        ctx.strokeStyle = '#ff7777';  
        ctx.fillStyle   = '#ff4444';  

        // on the first value we are not coming from an existing point so we just need to move to the coordinates ready to plot value 2.
        let firstValue = true;

        for (let i = 0; i < this.numxSteps; i++) 
        { 
            if (tempArray[i] != -9999) 
            {
                let tmpVal = tempArray[i] ;
                let yPos = (tmpVal - this.rangeMin) * (graphHeight - 0)/( this.rangeMax - this.rangeMin ) + 0;
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

        for (let i = 0; i < this.numxSteps; i++) 
        { 
            if (humdArray[i] != -9999) 
            {
                let tmpVal = humdArray[i] ;
                let yPos = (tmpVal - this.rangeMin) * (graphHeight - 0)/( this.rangeMax - this.rangeMin ) + 0;
                yPos = graphHeight - yPos;
                let xPos = (i*xStep) + xStep;

                // draw the line
                if (firstValue)   {  ctx.moveTo(xPos, yPos);  firstValue = false;  }
                else              {  ctx.lineTo(xPos,yPos);   ctx.stroke();  }

                // draw the dot
                ctx.beginPath();  ctx.arc(xPos, yPos, 3, 0, 2 * Math.PI, false);  ctx.fill();
            }
        }      
    }
}
