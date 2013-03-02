<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title> Chip Data </title>
<link rel="stylesheet" type="text/css" href="style.css"/>
<script type="text/javascript" src="js/jquery.js"></script>
<style>
input[type='text'] { font-size: 18px; text-align: center;}
input:focus, textarea:focus{background-color: lightgrey;}
</style>
</head>
  <body>
    <?php 
      include ("header.html");
    ?> 
    <!-- Table for Main Body -->
    <table width="100%" border="0" cellspacing="0" cellpadding="1">
      <tr>
        <td align="center">
          <h2>Action Data</h2>
          <div id="container">
            <?php
              include_once("makeASocket.php");

              $actionCnt = 1;
              $newSocket = makeASocket($service_port, $address);
              $in = $getActionArray.$actionCnt."\n";
              socket_write($newSocket, $in, strlen($in));
              $out = socket_read($newSocket, $socBufSize);
              socket_close($newSocket);
              $actionArray = explode(" ", $out);
              echo "<form method=\"post\" action=\"ActionUpdate.php\"><table border=\"2\" cellspacing=\"0\" cellpadding=\"10\">";
              echo "<tr><td align=\"center\" colspan=\"3\">Make your changes and press \"SUBMIT\" <input type=\"submit\" value=\"SUBMIT\"></td></tr>";
              echo "<tr><td align=\"center\" colspan=\"3\"><input type=\"hidden\" name=\"actionCnt\" value=\"".$actionCnt."\">Action Array #".$actionCnt."<br />";
              if($actionArray[0] === TRUE)
              {
                echo "<font color=\"green\"><strong>ENABLED&nbsp;&nbsp;</strong></font>";
                echo "<input type=\"radio\" name=\"enable\" value=\"Enable\" checked>Enable&nbsp;&nbsp;</input>";
                echo "<input type=\"radio\" name=\"enable\" value=\"Disable\">Disable</input>";
                echo "</td></tr>";
              }else{
                echo "<font color=\"red\"><strong>DISABLED&nbsp;&nbsp;</strong></font>";
                echo "<input type=\"radio\" name=\"enable\" value=\"Enable\">Enable&nbsp;&nbsp;</input>";
                echo "<input type=\"radio\" name=\"enable\" value=\"Disable\" checked>Disable</input>";
                echo "</td></tr>";
              }
              echo "<tr><td align=\"center\" colspan=\"3\">Temperature Sensor Address = <input type=\"text\" size=\"50\" name=\"tempAddress\" value=\"".$actionArray[1]."\"></td></tr>";
              echo "<tr><td align=\"center\" colspan=\"3\">Too Cold Parameters</td></tr>";
              echo "<tr><td align=\"center\">Trigger Temp</td><td align=\"center\">Switch Address</td><td align=\"center\">Delay (Microseconds)</td></tr>";
              echo "<tr><td align=\"center\"><input type=\"text\" size=\"10\" name=\"tcTemp\" value=\"".$actionArray[2]."\"></td>";
              echo "<td align=\"center\"><input type=\"text\" size=\"50\" name=\"tcSwitchAddr\" value=\"".$actionArray[3]."\"</td>";
              echo "<td align=\"center\"><input type=\"text\" size=\"10\" name=\"tcDelay\" value=\"".$actionArray[5]."\"></td></tr>"; 
              echo "<tr><td align=\"center\" colspan=\"3\">Too Hot Parameters</td></tr>";
              echo "<tr><td align=\"center\">Trigger Temp</td><td align=\"center\">Switch Address</td><td align=\"center\">Delay (Microseconds)</td></tr>";
              echo "<tr><td align=\"center\"><input type=\"text\" size=\"10\" name=\"thTemp\" value=\"".$actionArray[7]."\"></td>";
              echo "<td align=\"center\"><input type=\"text\" size=\"50\" name=\"thSwitchAddr\" value=\"".$actionArray[8]."\"</td>";
              echo "<td align=\"center\"><input type=\"text\" size=\"10\" name=\"thDelay\" value=\"".$actionArray[10]."\"</td></tr>"; 
              echo "<tr><td align=\"center\" colspan=\"3\">Make your changes and press \"SUBMIT\" <input type=\"submit\" value=\"SUBMIT\"></td></tr>";
              echo "</table></form>";

              $newSocket = makeASocket($service_port, $address);
              $in = "$getMaxChips\n";
              socket_write($newSocket, $in, strlen($in));
              $out = socket_read($newSocket, $socBufSize);
              socket_close($newSocket);
              $maxChipCnt = $out;
              echo "<p>There are ".$maxChipCnt." chip arrays, with";

              $newSocket = makeASocket($service_port, $address);
              $in = "$getChipCount\n";
              socket_write($newSocket, $in, strlen($in));
              $out = socket_read($newSocket, $socBufSize);
              socket_close($newSocket);
              echo " ".$out." chip arrays being used.<p>";

              echo "<table border=\"2\" cellspacing=\"0\" cellpadding=\"10\"><tr><td align=\"center\">Chip #</td><td align=\"center\">Address</td><td align=\"center\">Status</td></tr>";
              for($scCnt=0; $scCnt < $maxChipCnt; $scCnt++)
              {
                $newSocket = makeASocket($service_port, $address);
                $in = $showChip.$scCnt."\n";
                socket_write($newSocket, $in, strlen($in));
                $out = socket_read($newSocket, $socBufSize);
                socket_close($newSocket);
                $chipArray = explode(" ", $out);
                $chipAddressArray = explode(",", $chipArray[0]);
                if(($chipAddressArray[0] === "0x28") || ($chipAddressArray[0] === "0x12"))
                {
                  echo "<tr><td align=\"center\">".$scCnt."</td><td align=\"center\">".$chipArray[0]."</td><td align=\"center\">";
                  if($chipAddressArray[0] === "0x28")
                  {
                    echo $chipArray[1]."&deg;</td></tr>";
                    
                  }else if($chipAddressArray[0] === "0x12"){
                    if($chipArray[1] === "78")
                    {
                      echo "ON";
                    }else if($chipArray[1] === "70"){
                      echo "OFF";
                    }else{
                      echo "UNKNOWN";
                    }
                    echo "</td></tr>";
                  }else if($chipAddressArray[0] === "0x00"){
                    echo "UNUSED";
                  }else{
                    echo "<font color=\"red\"><strong>UNKNOWN</strong></font>";
                  }
                }
              }
              echo "</table>";
            ?>
          </div>
        </td>
      </tr>
    </table>
  </body>
</html>
