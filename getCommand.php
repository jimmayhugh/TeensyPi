<?php

  include_once("makeASocket.php");

  $newSocket = makeASocket($service_port, $address);
  echo "Number of Arguments:".$argc."\n";

  for($argCnt = 0; $argCnt < $argc; $argCnt++)
  {
    echo $argv[$argCnt]."\n";
  }

  if($argc > 1)
  {
    $in = $argv[1]."\n";
  }
  else
  {
    die("Need Command\n");
  }
  socket_write($newSocket, $in, strlen($in));
  while ($chipX = socket_read($newSocket, 2048))
  {
    echo $chipX;
  }
  echo "\n";
  socket_close($newSocket);
?>
