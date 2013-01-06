<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title> Chip Data </title>
<link rel="stylesheet" type="text/css" href="style.css"/>
<script type="text/javascript" src="js/jquery.js"></script>
</head>
  <body>
    <?php 
      include ("header.html");
    ?> 
    <!-- Table for Main Body -->
    <table width="100%" border="0" cellspacing="0" cellpadding="1">
      <tr>
        <td align="center">
          <h2>Action Update</h2>
            <?php
              echo "<p>actionCnt = ".$_POST["actionCnt"]."</p>";
              echo "<p>enable = ".$_POST["enable"]."</p>";
              echo "<p>tempAddress = ".$_POST["tempAddress"]."</p>";
              echo "<p>tcTemp = ".$_POST["tcTemp"]."</p>";
              echo "<p>tcSwitchAddr = ".$_POST["tcSwitchAddr"]."</p>";
              echo "<p>tcDelay = ".$_POST["tcDelay"]."</p>";
              echo "<p>thTemp = ".$_POST["thTemp"]."</p>";
              echo "<p>thSwitchAddr = ".$_POST["thSwitchAddr"]."</p>";
              echo "<p>thDelay = ".$_POST["thDelay"]."</p>";
              $updateArrayData1 = $_POST["enable"]." 0 ".$_POST["tempAddress"]." ";
              $updateArrayData2 = $_POST["tcTemp"]." ".$_POST["tcDelay"]." ".$_POST["tcSwitchAddr"]." ";
              $updateArrayData3 = $_POST["thTemp"]." ".$_POST["thDelay"]." ".$_POST["thSwitchAddr"]." ";
              echo "<p>".$updateArrayData1." ".$updateArrayData2." ".$updateArrayData3."</p>";
              echo "<p>Sending Data</p>";
              include_once("makeASocket.php");

              $newSocket = makeASocket($service_port, $address);
              $in = $updateActionArray." ".$_POST["actionCnt"]." 1 ".$updateArrayData1."\n";
              socket_write($newSocket, $in, strlen($in));
              $out = socket_read($newSocket, $socBufSize);
              socket_close($newSocket);
              $out1 = $out;

              $newSocket = makeASocket($service_port, $address);
              $in = $updateActionArray." ".$_POST["actionCnt"]." 2 ".$updateArrayData2."\n";
              socket_write($newSocket, $in, strlen($in));
              $out = socket_read($newSocket, $socBufSize);
              socket_close($newSocket);
              $out2 = $out;

              $newSocket = makeASocket($service_port, $address);
              $in = $updateActionArray." ".$_POST["actionCnt"]." 3 ".$updateArrayData3."\n";
              socket_write($newSocket, $in, strlen($in));
              $out = socket_read($newSocket, $socBufSize);
              socket_close($newSocket);
              $out3 = $out;

              echo "<p>Return = ".$out1.$out2.$out3."</p>";
            ?> 
          <div id="container">
          </div>
        </td>
      </tr>
    </table>
  </body>
</html>
