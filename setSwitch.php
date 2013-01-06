<?php

  include_once("makeASocket.php");

  $newSocket = makeASocket($service_port, $address);
  $in = $setSwitchState.$_GET["data"]."\n";
  socket_write($newSocket, $in, strlen($in));
  $chipX = socket_read($newSocket, 2048);
  socket_close($newSocket);
  $result=trim($chipX);
  if($result === "N")
  {
    echo TRUE;
  }else if($result === "F"){
    echo FALSE;
  }

?>
