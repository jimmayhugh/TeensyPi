<?php
  include_once("/var/www/htdocs/accessDatabase.php");
  include_once("/var/www/htdocs/makeASocket.php");

  $newSocket = makeASocket($service_port, $address);
  $in = "$getActionStatus\n";
  socket_write($newSocket, $in, strlen($in));
  $out = socket_read($newSocket, $socBufSize);
  socket_close($newSocket);

  $actionTime = date("U");
  $actionArray = explode(";", $out);
  for($x=0; $x < count($actionArray); $x++)
  {
    $chipArray = explode(",", $actionArray[$x]);
    if($chipArray[0] === '0')
    {
      continue;
    }
    $actionID = $x;
    $actionTemp = $chipArray[1];
    if($chipArray[2] === 'F')
    {
      $actionTC = "OFF";
    }else if($chipArray[2] === 'N'){
      $actionTC = "ON";
    }else{
      $actionTC = "NONE";
    }
    if($chipArray[3] === 'F')
    {
      $actionTH = "OFF";
    }else if($chipArray[3] === 'N'){
      $actionTH = "ON";
    }else{
      $actionTH = "NONE";
    }
    $actionTooCold = $chipArray[4];
    $actionTooHot = $chipArray[5];
//    echo "Action #".$x." = ".$actionArray[$x]."\n";
    $query = "INSERT INTO actionGraph VALUES('".$actionID."','".$actionTime."','".$actionTemp."','".$actionTooCold."','".$actionTC."','".$actionTooHot."','".$actionTH."')";
//    echo $query."\n";
    mysqli_query($link, $query);
  }
  mysqli_close($link);
?>
