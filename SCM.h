/*
  Simply Connection Manager (SCM)
  Wifi connection portal
  Modified for use with IOT Monitor and 40x20 LCD
  
  /    - root - show connection details + Scan btn + Exit btn + Reset btn
  /ss  - start scan. Show scanning screen. Auto refresh
  /sc  - scan - show available networks + rescan btn + Save & Connect btn
  /st  - show connection details + Exit btn, Reset btn
  /sb  - process form data: ssid & password. Try to connect to the provided ssid. Show info page. Links to /st + reset.
  /ex  - exit. Show msg connection manager closed.
  /rs  - restart. Show device is restarting. Restart the ESP8266

*/




boolean const SCM_DEBUG = true;
boolean const SCM_LCD = true;

long SCM_START_TIME = 0;

String SCM_REQUEST = "";
int SCM_n = 0;
String  SCM_APip = "192.168.4.1";
char const  SCM_TITLE[] = "Connection Manager  ";  // max 20 characters for 4 x 20 LCD

long SCM_START = 0;
long SCM_END = 0;

char const SCM_HEADER[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
char const SCM_HEADR2[] = "HTTP/1.1 200 OK\r\nCache-Control: no-cache,no-store,max-age=0,must-revalidate\r\nPragma: no-cache\r\nContent-Type: text/html\r\n\r\n";

// The HTML and javascript can be minimized.
char const SCM_HTML_TOP[] = R"( 
<!DOCTYPE html>
<html  lang='en'>
<head>
<meta charset='utf-8'>
<meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no'>
<title>Connection Manager</title>
<style>
body     { font-family: Tahoma, Geneva, sans-serif;}
#outer   { padding: 10px; }
#wrapper { margin: auto;  width: 90%; font-size: 1.0em;   padding: 0.5em 0.6em 0 0;  }
@media   ( min-width:1200px)                      {#wrapper{margin: auto;width: 30%} }
@media   ( min-width:768px) and (max-width:1200px){#wrapper{margin: auto;width: 50%} }
h1       { width: 100%; text-align: center; font-size: 1.5em; margin: 0.2em 0 1em 0; }
#msg     { margin: 0 0 2em 0; font-size: 1.1em; line-height: 1.4;}
#msg p   { margin: 0 0 0.2em 0;}

#scroll                       { margin: 2em 0 2em 0; padding: 10px 0 10px 0;  border-bottom: 1px solid LightGray;   border-top: 1px solid LightGray;  font-size: 90%;}
#scroll div                   { margin: 0 0 0 0 ; padding: 5px 0 5px 0px;}
#scroll div span:nth-child(1) { color: blue; text-decoration: underline;  cursor: pointer;}
#scroll div span:nth-child(2) { float:right; text-align: left;  min-width: 60px;}
.locked  { background: url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==') no-repeat right center; background-size: 1em}      

#f1      { margin: 1em 0 1em 0;} 
#f1  > * { display: block;}
label    { font-size: 1.2em;}
label>*  { display: inline;}
#s, #p   { width: 98%; font-size: 1.25em;  padding: 5px 3px 5px 3px; border: 1px solid blue;}
#s       { margin-bottom: 0.5em;}
#p       { margin-bottom: 1.5em;}

.but1 { width: 100%; font-size: 1.5em; margin: 0.75em 0 0 0; padding: 6px 0 6px; border-radius: 6px; border: black solid 1px; background-color: #EEEEEE; display: block;
        text-align: center; text-decoration: none;   -webkit-appearance: button;     -moz-appearance: button;     appearance: button;       }
.but2 { width: 90%; font-size: 1.25em; margin: 0 0 0 0; padding: 4px 0 4px; border-radius: 4px; border: black solid 1px; background-color: #EEEEEE;
        text-align: center; text-decoration: none;   -webkit-appearance: button;     -moz-appearance: button;     appearance: button;       }
.b    { font-weight: bold;}
.bbb { color: black;  }
.rrr  { color: red;  }
.menBut { color: black;  float:left;  }
.rscBut { color: black;  float:right;   }

</style>
</head>
<body>
<div id='outer'>
<div id='wrapper'>
<h1>Connection Manager</h1>
)";

    
char const SCM_HTML_BOT[] = R"( 
</div> <!-- wrapper -->
</div> <!-- outer -->
</body>
<script>
function f(e)
{
  document.getElementById('s').value=e.innerText||e.textContent;
  document.getElementById('p').focus();
  return false;
}
function chTxt(e)
{
    e.value = 'Scanning ... please wait';
    e.innerHTML = 'Scanning ... please wait';
}
function chTxt2(e)
{
    e.value = 'Scanning';
    e.innerHTML = 'Scanning';
}
</script>   
</html> 
)";



String SCM_HTML_FORM = R"( 
  <form id='f1' method='post' action='http://{i}/sb'  enctype='text/plain'>
  <label>SSID</label>
  <input id='s' name='s' length='32' >
  <label>Password</label>
  <input id='p' name='p' type='password' length='64'  >
  <button class='but1' id='sub' type='submit'>SAVE & CONNECT</button>
  </form>
)";

String SCM_HTML_NET_INFO = "";
String SCM_HTML_MENU = "";
String SCM_HTML_NET_LIST = ""; 
String SCM_ADD_HTML = "";

void SCM_MAKE_CON_INFO()
{
  SCM_HTML_NET_INFO = R"(
  <div id='msg'>
  <table>
    <tr> <td colspan='2' class='b'>Connection Portal</td>  </tr>
    <tr> <td>SSID:</td>       <td>{s1}</td>  </tr>
    <tr> <td>password:</td>   <td>{p1}</td>  </tr>
    <tr> <td>IP:</td>         <td>{i1}</td>  </tr>
    <tr> <td>&nbsp;</td>      <td></td>      </tr>
    <tr> <td colspan='2' class='b'>Local Network</td>  </tr>
    <tr> <td>SSID </td>       <td>{s}</td>  </tr>
    <tr> <td>IP:</td>         <td>{i}</td>  </tr>
    <tr> <td>&nbsp;</td>      <td></td>     </tr>
    <tr> <td>Saved SSID:</td> <td>{v}</td>  </tr>
    </table>
  </div>
  )";
  
  if (WiFi.status()==WL_CONNECTED ) 
  {     
      SCM_HTML_NET_INFO.replace( "{s}", WiFi.SSID() ) ;   
      SCM_HTML_NET_INFO.replace( "{i}", WiFi.localIP().toString() ) ;  
  }
  else
  {
     SCM_HTML_NET_INFO.replace( "{s}", " Not connected" );   
     SCM_HTML_NET_INFO.replace( "{i}", " Not connected" );  
  } 
  if (WiFi.SSID() == "" )  { SCM_HTML_NET_INFO.replace( "{v}", " No saved SSID");  }        else  { SCM_HTML_NET_INFO.replace( "{v}", WiFi.SSID() );  }

  SCM_HTML_NET_INFO.replace("{s1}", APssid );
  SCM_HTML_NET_INFO.replace("{p1}", APpass );
  SCM_HTML_NET_INFO.replace("{i1}", SCM_APip );
}


void SCM_MAKE_MENU( )
{
  SCM_HTML_MENU = "";
  SCM_HTML_MENU = "<a href='http://";
  SCM_HTML_MENU += SCM_APip;
  SCM_HTML_MENU += "/sc' class='but1 bbb' onclick='chTxt(this)' id='sb'>Scan networks</a>\n";

  SCM_HTML_MENU += "<a href='http://";
  SCM_HTML_MENU += SCM_APip;
  SCM_HTML_MENU += "/ex' class='but1 bbb'>Exit</a>\n" ;        
  SCM_HTML_MENU += "<a href='http://";
  SCM_HTML_MENU += SCM_APip;
  SCM_HTML_MENU += "/rs' class='but1 rrr'>Reset</a>\n";
}

  
int scanNetworksAndSort()
{
  if (SCM_DEBUG) { Serial.println(F("Searching for local networks")); } 
  if (SCM_LCD)
  {
      lcd.clear();
      lcd.setCursor(0, 0);  lcd.print(SCM_TITLE);
      lcd.setCursor(0, 2);  lcd.print(F("Searching for"));
      lcd.setCursor(0, 3);  lcd.print(F("networks "));
  }

  SCM_n = WiFi.scanNetworks();

  if (SCM_DEBUG) { Serial.println(F("Scan done"));   } 
  if (SCM_DEBUG) { Serial.print(SCM_n); Serial.println(F(" networks found")); } 
  
 
  int networkCount = 0;
  int minQuality = -90;
  boolean dup[SCM_n];
  
  if (SCM_n == 0) 
  {    
      SCM_HTML_NET_LIST = "<p>No networks found</p>";
      return 0;
  }
  else
  {
      int quality[SCM_n];
    //  int qual[SCM_n];     
      float t = 0; 
      for (int i = 0; i < SCM_n; ++i) 
      {
          if (WiFi.RSSI(i) <= -100)     { quality[i] = 0;   } 
          else if (WiFi.RSSI(i) >= -50) { quality[i] = 100; } 
          else                          { quality[i] = 2 * (WiFi.RSSI(i)+ 100);    }
      }

      // sort by RSSI.  RSSI values are negative and the closet to zero is strongest
      // simple bubble sort
      int ind[SCM_n];       for(int i=0; i<SCM_n; i++)   { ind[i] = i; }
      for(int i=0; i<SCM_n; i++) 
      {
          for(int j=0; j<(SCM_n-(i+1)); j++) 
          {
              if( WiFi.RSSI( ind[j] ) < WiFi.RSSI( ind[j+1] ) )  { int tmp = ind[j]; ind[j] = ind[j+1];  ind[j+1] = tmp;      }
          }
      }
    
      
      // find duplicate SSIDs
      boolean dup[SCM_n];      for(int i=0; i<SCM_n; i++) {  dup[ind[i]] = false;  }
      for(int i=0; i<SCM_n; i++) 
      {   
          for (int j = i + 1; j < SCM_n; j++)   
          {  
            if (WiFi.SSID( ind[i]) == WiFi.SSID( ind[j]) ) { dup[ind[j]] = true; }   
          }  
      }
      int numDups = 0;
      for(int i=0; i<SCM_n; i++) { if(dup[ind[i]] == true)  { numDups++; }  }

      if (SCM_DEBUG) { Serial.print(F("number of duplicates = ")); Serial.println(numDups);   } 
      if (SCM_DEBUG) { Serial.println(F("Sorted by RSSI with duplicates removed"));  } 

      if (SCM_DEBUG)
      {
          int num = 0;
          for(int i=0; i<SCM_n; i++) 
          {
              if ( dup[ ind[i] ] != true )
              {
                   if ( WiFi.RSSI(i) > minQuality) 
                  {   
                    num++;
                    Serial.print(num);   Serial.print(F(": "));   if ( num<10)    { Serial.print(F(" ")); }
                    Serial.print( WiFi.SSID( ind[i]) );       
                    int len = WiFi.SSID(ind[i]).length();     for (int x = 0; x < 40-len; ++x) { Serial.print(F(" ")); } 
                    Serial.print(F("("));  Serial.print(WiFi.RSSI( ind[i] ));  Serial.print(F(")\t"));   
                    if ( quality[ ind[i] ]  < 100) { Serial.print(F(" "));}
                    Serial.print( quality[ ind[i] ]);   Serial.print(F("%\t ENC: "));       
                    Serial.print(WiFi.encryptionType( ind[i] ) );     
                    Serial.print(F("\n"));
                  }
              } // if ( dup[ ind[i] ] != true )
          } // loop
      }


      SCM_HTML_NET_LIST = "<table width='100%'><tr>\n<td width='50%'><a href='http://";
      SCM_HTML_NET_LIST += SCM_APip;
      SCM_HTML_NET_LIST += "' class='but2 menBut'>Menu</a></td>\n<td><a href='http://";
      SCM_HTML_NET_LIST += SCM_APip;
      SCM_HTML_NET_LIST += "/sc' class='but2 rscBut' onclick='chTxt2(this)'>Rescan</a></td>\n</table>\n";
      SCM_HTML_NET_LIST += "<div id='scroll'>\n";

      int num = 0;
      for(int i=0; i<SCM_n; i++) 
      {
          if ( dup[ ind[i] ] != true )
          {
              if ( WiFi.RSSI(i) > minQuality) 
              {             
                num++;
                SCM_HTML_NET_LIST += " <div> <span onclick='f(this)'>";
                SCM_HTML_NET_LIST += WiFi.SSID(ind[i]);
                SCM_HTML_NET_LIST += "</span> <span";
                if ( WiFi.encryptionType(ind[i]) == 7 ) { SCM_HTML_NET_LIST +=  " class='open'"; } else { SCM_HTML_NET_LIST +=  " class='locked'"; }
                
                SCM_HTML_NET_LIST += ">";
                SCM_HTML_NET_LIST += quality[ ind[i] ];
                SCM_HTML_NET_LIST += "%</span> </div>\n";
              }
          } // if ( dup[ ind[i] ] != true )
      } // loop

      SCM_HTML_NET_LIST += "</div>\n\r";

 
      if (SCM_LCD)
      {
          lcd.clear();
          lcd.setCursor(0, 0);  lcd.print(SCM_TITLE);
          if (SCM_n == 0)     { lcd.setCursor(0, 2);  lcd.print("No networks found"); }
          else                { lcd.setCursor(0, 2);  lcd.print(num); lcd.print(" networks found"); }
      }

  } // else SCM_n==0
  
  return networkCount;
}





int simpleConnectionManager(const char *APssid, const char *APpass, long timeOutTime)
{
  // return value: 
  //    -1 = no connection attempt. 
  //    0 = no networks. 
  //    1 = have networks but connection failed. 
  //    2 = connected to local network. 
  //    9 = timed out.
  
  SCM_START_TIME = millis();
  boolean SCM_activity = false;
  
  
  if(SCM_DEBUG) { Serial.println(F("\n\nSimple Connection Manager"));   }

  int returnReply = -1;
  String POST_SSID = "";
  String POST_PASS = "";

  boolean requestError = false;

  SCM_HTML_FORM.replace("{i}", SCM_APip );
  
  WiFi.mode(WIFI_AP_STA);
  WiFi.disconnect();
  delay(100);

  if (APpass == "" ) { WiFi.softAP(APssid); } else { WiFi.softAP(APssid, APpass); }
  
  if (SCM_DEBUG)
  {
     Serial.print(F("\nLocalIP = "));     Serial.println(WiFi.localIP()); 
     Serial.print(F("APIP = "));          Serial.println(WiFi.softAPIP() );
     Serial.print(F("APWiFi status = ")); Serial.println(WiFi.status());   
  }


  if (SCM_LCD)
  {
      lcd.clear();
      lcd.setCursor(0, 0);  lcd.print(SCM_TITLE);
      lcd.setCursor(0, 1);  lcd.print(F("SSID:")); lcd.print(APssid);
      lcd.setCursor(0, 2);  lcd.print(F("IP:"));   lcd.print(WiFi.softAPIP() );
      lcd.setCursor(0, 3);  lcd.print(F("PW:"));   lcd.print(APpass);
  }

  server.begin();
  if (SCM_DEBUG) { Serial.println(F("SCM server started"));  } 

   boolean finished = false;
   while ( ! finished )
   {
    
    // Check if a client has connected
    WiFiClient client = server.available();     

    // if a client has connected get the request and see what type it is
   if (client)  
   {  
        SCM_activity = true;
        if (SCM_LCD) { lcd.setCursor(17, 3);  lcd.print(F("   ")); }
        
        SCM_START = millis();
        requestError = false;
        String SCM_REQUEST = client.readStringUntil('\r');
        if (SCM_DEBUG) { Serial.print(F("\nRequest = "));  Serial.println(SCM_REQUEST);  } 

        int SCM_URI_START = SCM_REQUEST.indexOf(" ")  ;
        int SCM_URI_END   = SCM_REQUEST.indexOf(" ",SCM_URI_START+1 );

        if (SCM_URI_START != -1 && SCM_URI_END != -1)
        { 
              String SCM_URI    = SCM_REQUEST.substring(SCM_URI_START+1, SCM_URI_END);
              SCM_URI.toLowerCase();
              if (SCM_DEBUG) { Serial.print(F("URI = "));   Serial.println(SCM_URI);  }
            
              if  ( SCM_URI == "/" )   // serve info page with the scan btn
              {    
                  if (SCM_DEBUG) {  Serial.println(F("Main page")); }  
                  SCM_MAKE_CON_INFO();
                  SCM_MAKE_MENU();
                  client.flush();
                  client.print( SCM_HEADER );
                  client.print( SCM_HTML_TOP ); 
                  client.print( SCM_HTML_NET_INFO );
                  client.print( SCM_HTML_MENU );
                  client.print( SCM_HTML_BOT );
                  delay(5);
                 // client.stop(); 
              }
      
      
              else if  ( SCM_URI == "/favicon.ico" )  
              {
                  if (SCM_DEBUG) {  Serial.println(F("favicon 404 page"));  }  
                  client.flush();
                  client.print( F("HTTP/1.1 404 Not Found\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n" ));
                  client.print( F( "<html><body>Not Found</body></html>" ));
                  delay(5);
              }
      
      
              // scan for networks 
              else if  ( SCM_URI == "/sc" )   
              {
                  if (SCM_DEBUG) { Serial.println(F("Network list page")); } 
                  int numNetworks = scanNetworksAndSort();
                  if ( numNetworks == 0) { returnReply = 0;}
                  client.flush();
                  client.print( SCM_HEADR2 );
                  client.print( SCM_HTML_TOP ); 
                  client.print( SCM_HTML_NET_LIST );
                  client.print( SCM_HTML_FORM );
                  client.print( SCM_HTML_BOT );
                  delay(5);
                  //client.stop();
                  
              }
      
              // exit SCM
              else if  ( SCM_URI == "/ex" )   
              {
                  if (SCM_DEBUG) { Serial.println(F("EXIT")); }
                  client.flush();
                  client.print( SCM_HEADER );
                  client.print( SCM_HTML_TOP );
                  client.print( F("<br>\n<p>Connection Manager has been closed.</p>" ));
                  client.print( SCM_HTML_BOT );
                  delay(5);
                  finished  = true; 
              }
      
      
              // reset the ESP8266
              else if  ( SCM_URI == "/rs" )   
              {
                  if (SCM_DEBUG) { Serial.println(F("Reset page")); } 
                  client.flush();
                  client.print( SCM_HEADER );
                  client.print( SCM_HTML_TOP );
                  client.print( F("<div id='msg'>\n<p>Restarting.</p>\n<br>\n<p>If there is a saved SSID the device will try to connect to the saved SSID.</p>\n</div>\n")) ;               
                  client.print( SCM_HTML_BOT );
                  delay(1000);
                  ESP.reset();
              }
      
              // process the connection form
              else if  ( SCM_URI == "/sb" )   
              {
                 if ( SCM_REQUEST.indexOf("POST") != -1  ) 
                 {
                      if (SCM_DEBUG) { Serial.println(F("Form submission")); } 
                       boolean done = false;
                      boolean haveSSID = false;
                      boolean havePASS = false;
      
                      // need an error out
                      
                      while (!done)  
                      //  iterate through the lines of the request until we have the ssid and the password
                      // even if the ssid and password are empty there will still be the s= and p=
                      {
                          int len = 0;
                          SCM_REQUEST =  client.readStringUntil('\r');
                          client.readStringUntil('\n');                // clear the end character
      
                          if ( SCM_REQUEST.indexOf("s=") != -1 ) 
                          {
                              int pos = SCM_REQUEST.indexOf("s=") + 2 ;  
                              int len = SCM_REQUEST.length();
                              POST_SSID = SCM_REQUEST.substring(pos,len);
                              haveSSID = true;
                          }
            
                          if ( SCM_REQUEST.indexOf("p=") != -1 ) 
                          {     
                              int pos = SCM_REQUEST.indexOf("p=") + 2;
                              int len = SCM_REQUEST.length();
                              POST_PASS = SCM_REQUEST.substring(pos,len);
                              havePASS = true;
                          }
                          if (havePASS && haveSSID) { done = true;}
            
                      } // while !done
      
           
                      // see if a password is required
                      int SCM_ENC = 0;
                      for(int i=0; i<SCM_n; i++) {    if( POST_SSID == WiFi.SSID(i) )  { SCM_ENC = WiFi.encryptionType(i);   }       }
                      boolean SCM_PASS_OK = false;
                        
                      if ( SCM_ENC == 7)   { POST_PASS = ""; SCM_PASS_OK = true;   }
                      if ( (SCM_ENC == 2 ||  SCM_ENC == 4 || SCM_ENC == 5 || SCM_ENC == 8) && ( POST_PASS.length() > 7 ))  { SCM_PASS_OK = true; }
            
                      if ( SCM_PASS_OK )
                      {
                          String SCM_HTML_MSG = "";
                          SCM_HTML_MSG += "<div id='msg'>\n";
                          SCM_HTML_MSG += "<p>Trying to connect to <span class='b'>";
                          SCM_HTML_MSG += POST_SSID;
                          SCM_HTML_MSG += "</span>. If the connection is successful the network credentials will be automatically saved.</p>\n<br>\n";
                          SCM_HTML_MSG += "<p>Wait a few seconds and <a href='http://";
                          SCM_HTML_MSG += SCM_APip;
                          SCM_HTML_MSG += "'>click here</a>. The connection page is still open but due to how the ESP8266 works you may need to reconnect your device.</p>\n";
                          
                          SCM_HTML_MSG += "<br>\n<p>If the refresh fails reconnect to <span class='b'>";
                          SCM_HTML_MSG += APssid;
                          SCM_HTML_MSG += "</span> and go to <span class='b'>http://";  
                          SCM_HTML_MSG += SCM_APip;
                          SCM_HTML_MSG += "</span></p>\n<br><br>\n";
                          SCM_HTML_MSG += "<p>You can force a reset by <a href='http://";
                          SCM_HTML_MSG += SCM_APip;
                          SCM_HTML_MSG += "/rs'>clicking here</a>. After the device restarts, if there is a saved ssid, it should auto-connect.</p>\n";
                          SCM_HTML_MSG += "</div>\n";
                         
                          client.flush();
                          client.print( SCM_HEADER );
                          client.print( SCM_HTML_TOP ); 
                          client.print( SCM_HTML_MSG );
                          client.print( SCM_HTML_BOT );
                          delay(5);
                          if (SCM_DEBUG) { Serial.println(F("Connected page")); } 
                          //client.stop();

                                  
                          // connect to local network.
                          if (SCM_LCD)
                          {
                              lcd.clear();
                              lcd.setCursor(0, 0);  lcd.print(SCM_TITLE);
                              lcd.setCursor(0, 1);  lcd.print(F("Connecting to"));
                              lcd.setCursor(0, 2);  lcd.print(POST_SSID);
                              lcd.setCursor(0, 3);
                          }

                          if (SCM_DEBUG) { Serial.print(F("connecting to "));  Serial.println(POST_SSID); } 
                          WiFi.begin(POST_SSID, POST_PASS); 
            
                          int count =0;
                          while (WiFi.status() != WL_CONNECTED   && count < 20    ) 
                          {
                              if (SCM_DEBUG) { Serial.print(F(".")); }
                              if (SCM_LCD)   { lcd.print(".");       }
                              count++;     
                              delay(500);                  
                          }
                          if (SCM_DEBUG) { Serial.print(F("\n")); }
            
                          if (WiFi.status() == WL_CONNECTED )
                          {
                              if (SCM_DEBUG) { Serial.print(F("connected to ")); Serial.print(WiFi.SSID());      } 
                              if (SCM_DEBUG) { Serial.print(F(" IP = "));        Serial.println(WiFi.localIP()); } 
                              //SCM_debug(F(" IP = "));  SCM_debug( WiFi.localIP().toString() );  SCM_debug(F("\n"));
                              if (SCM_LCD)
                              {
                                  lcd.clear();
                                  lcd.setCursor(0, 0);  lcd.print(SCM_TITLE);
                                  lcd.setCursor(0, 1);  lcd.print("Connected to");
                                  lcd.setCursor(0, 2);  lcd.print( WiFi.SSID() );
                                  lcd.setCursor(0, 3);  lcd.print( WiFi.localIP());
                               }
                              returnReply = 2;
                              delay(4000);
                          }
                          else  // not connected
                          {
                              if (SCM_DEBUG) { Serial.print(F("Cannot connect to ")); Serial.println(POST_SSID);  } 
                              if (SCM_LCD)
                              {
                                  lcd.clear();
                                  lcd.setCursor(0, 0);  lcd.print(SCM_TITLE);
                                  lcd.setCursor(0, 1);  lcd.print("Cannot connect to");
                                  lcd.setCursor(0, 2);  lcd.print( POST_SSID );
                              }
                              returnReply = 1;
                              delay(4000);
                          }
                          
                      } // passok
                      else // pass not ok
                      {
                            
                            client.flush();
                            client.print( SCM_HEADR2 );
                            client.print( SCM_HTML_TOP ); 
                            client.print( SCM_HTML_NET_LIST );
                            client.print( F("<p style='color:red;'>ERROR!. SSID or password is incorrect. Please retry.</p>") );
                            client.print( SCM_HTML_FORM );
                            client.print( SCM_HTML_BOT );
                            delay(5);
                            //client.stop();
                            if (SCM_DEBUG) { Serial.println(F("Error! Network list page resent")); } 
                      }
      
                 }  // if ( SCM_REQUEST.indexOf("POST") != -1  )
                 
                 else
                 {
                   if (SCM_DEBUG) { Serial.println(F("Request = sb but not POST: Request ignored"));  } 
                   SCM_ADD_HTML = "<p style='color:red;'>The error be strange. You went where you cannot go. &nbsp; Please don't do it again.</p>";
                   requestError = true;
                 }
      
               
              } // else if  ( SCM_URI == "/sb" ) 
      
      
              else    // request URI not recognised
              { 
                if (SCM_DEBUG) { Serial.println(F("Request not recognised\n"));  } 
                SCM_ADD_HTML = "<p style='color:red;'>Not sure what you did but you weren't supposed to do it! &nbsp; Please try not to do it again.</p>";
                requestError = true;
              }
      
      
              if(requestError )
              {
                requestError  = false;
                SCM_MAKE_CON_INFO();
                SCM_MAKE_MENU();
                client.flush();
                client.print( SCM_HEADER );
                client.print( SCM_HTML_TOP ); 
                client.print( SCM_ADD_HTML ); 
                client.print( SCM_HTML_NET_INFO );
                client.print( SCM_HTML_MENU );
                client.print( SCM_HTML_BOT );
                delay(5);
                //client.stop();  
              }

  
        } // if ( SCM_REQUEST!="" ) 
        else { if (SCM_DEBUG) { Serial.println(F("Enpty request: ignored")); }     }  
         
        SCM_REQUEST = "";
        SCM_END = millis();
        if (SCM_DEBUG) { Serial.print(F("Time Taken = "));  Serial.print(SCM_END-SCM_START);  Serial.println(F("ms"));   } 
        
    } // if (client)   


    if (SCM_activity == false)
    {
      if (SCM_LCD)   
      {   
         long t = ( timeOutTime - (millis() - SCM_START_TIME )) / 1000;       

         lcd.setCursor(19, 3);  
         if ( t> 9 )     { lcd.setCursor(18, 3);  }
         else if (t> 99) { lcd.setCursor(17, 3);  }
         lcd.print(t);
      }
       if ( millis() - SCM_START_TIME >= timeOutTime) { returnReply = 9; finished = true;}
    }

   
    
 } // while (notFinished)

  return returnReply;
     
} // int simpleConnectionManager(const char *APssid, const char *APpass)
