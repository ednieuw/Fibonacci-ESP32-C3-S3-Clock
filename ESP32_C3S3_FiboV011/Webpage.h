const char* PARAM_INPUT_1 = "input1";
const char index_html[]  = 
R"rawliteral(
<!DOCTYPE HTML><html><head>
<title>Nano ESP32 Word clock</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<meta name="google" content="notranslate" />
<style type="text/css">
  .auto-style2 { text-align: center;  }
  .auto-style3 { font-family: Verdana, Geneva, Tahoma, sans-serif; background-color: #FFFFFF; color: #FF0000;  }
  .auto-style11{ border-left: 0px solid #000000;	border-right: 0px solid #000000;
	             border-top: thin solid #000000;	border-bottom: thin solid #000000;
	             font-family: Verdana, Arial, Helvetica, sans-serif;  }
  .style1 {font-size: smaller}
  .style2 {font-size: small}
  .style3 {	font-family: "Courier New", Courier, mono;	font-weight: bold;}
  .style9 {color: #0066CC}
  .style10 {font-family: "Courier New", Courier, mono; font-weight: bold; color: #0066CC; }
  .auto-style1 {
    font-family: Verdana, Geneva, Tahoma, sans-serif;
    background-color: #FFFFFF;
  }
  .auto-style4 {
    font-size: smaller;
  }
  </style>
</head>
<body>
<table style="width: auto" class="auto-style11">
<tr>
<td colspan="3" class="auto-style2">
<span class="auto-style3"><a href="https://github.com/ednieuw/Fibonacci-ESP32-C3-S3-Clock">ESP32-Fibonacci clock</a> </span>
<tr>
<td width="123" style="width: 108px"> <strong>A</strong> SSID</td>
<td width="98" style="width: 98px"><strong>B</strong> Password</td>
<td width="157" style="width: 125px"><strong>C</strong> BLE beacon</td>
</tr>
<tr>
<td colspan="3"><strong>E</strong> Set Time zone <span class="auto-style4 style1"> E<-02>2 or E<+01>-1</span></td>
</tr>
<tr>
<td colspan="3"><strong>H</strong> Selftest</td>
</tr>
<tr>
<td colspan="3"><strong>N</strong> Display Off between Nhhhh (N2208)</td>
</tr>
<tr>
<td colspan="3"><strong>O</strong> Display toggle On/Off</td>
</tr>
<tr>
<td colspan="3"> <div align="center" class="style10">Display colour choice</div></td>
</tr>
<tr>  <td colspan="3">  <span class="auto-style1"></span> 
<strong>Q</strong> Display colour choice <span class="auto-style4">(Q0-Q9)</span>   </tr>
<tr>
<td colspan="3" style="height: 24px"> <div align="center" class="style9"> <span class="style3">Light intensity setting</span> 
<span class="style1">(0-255) </span></div></td>
</tr>
<tr>
<td style="width: 108px"><strong>S</strong> Slope </td>
<td style="width: 98px"><strong>L</strong> Min </td>
<td style="width: 125px"><strong>M</strong> Max </td>
</tr>
<tr>
<td colspan="3" style="height: 24px"> <div align="center" class="style10">Communication</div></td>
</tr>
<tr>
<td style="width: 108px"><strong>W</strong> WIFI</td>
<td style="width: 98px"> <strong>X</strong> NTP </td>
<td style="width: 125px"><strong>CCC</strong> BLE</td>
</tr>
<tr>
<td style="width: 108px"><strong>R</strong> Reset</td>
<td style="width: 98px"><strong>@</strong> Restart </td>
<td style="width: 125px"><strong>+</strong> Fast BLE</td>
</tr>
<tr>
<td style="width: 108px"><a href="https://github.com/ednieuw/Fibonacci-ESP32-C3-S3-Clock/blob/main/ESP32C3S3%20Fibonacci%20manual.pdf">Manual</a></td>
<td style="width: 98px">&nbsp;</td>
<td style="width: 125px"><a href="https://www.ednieuw.nl" class="style2">ednieuw.nl</a></td>
</tr></td>
</table>
<form action="/get">
<strong>     
<input name="submit" type="submit" class="auto-style3" style="height: 22px" value="Send">
</strong>&nbsp;
<input type="text" name="input1" style="width: 272px"></form><br>
    
<br>
</body></html>
)rawliteral";
