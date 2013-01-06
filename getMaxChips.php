<?php

  include_once("makeASocket.php");

  $newSocket = makeASocket($service_port, $address);
  $in = $getMaxChips."\n";
  socket_write($newSocket, $in, strlen($in));
  while ($chipX = socket_read($newSocket, 2048))
  {
    echo $chipX;
  }
  socket_close($newSocket);
?>
