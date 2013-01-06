<?php

  include_once("makeASocket.php");
  $tempStr="";
  $switchStr="";
  $unusedStr="";
  $newSocket = makeASocket($service_port, $address);
  $in = $getAllStatus."\n";
  socket_write($newSocket, $in, strlen($in));
  $chipX = socket_read($newSocket, 2048);
  socket_close($newSocket);

  $status=explode(",", $chipX);
  $count = count($status);

  for($x=0;$x<$count;$x++)
  {
    if(is_numeric($status[$x]) === TRUE)
    {
      $tempStr .= "<td align=\"center\">Temp ".$x."<br /><br /><font size=\"10\"><strong>".$status[$x]."&deg;</strong></font></td>";
    }else if(($trimStr = trim($status[$x])) === "N"){
      $switchStr .= "<td align=\"center\">Switch ".$x."<br /><br /><font size=\"10\" color=\"green\"><strong>ON</strong></font></td>";
    }else if(($trimStr = trim($status[$x])) === "F"){
      $switchStr .= "<td align=\"center\">Switch ".$x."<br /><br /><font size=\"10\" color=\"red\"><strong>OFF</strong></font></td>";
    }else if(($trimStr = trim($status[$x])) === "Z"){
      $unusedStr .= "<td align=\"center\">Unused ".$x."<br /><br />UNUSED</td>";
    }
  }

  echo "<table width=\"95%\" align=\"center\" border=\"2\"><tr>$tempStr</tr></table><table width=\"95%\" align=\"center\" border=\"2\"><tr>$switchStr</tr></table><table width=\"95%\" align=\"center\" border=\"2\"><tr>$unusedStr</tr></table>";

?>
