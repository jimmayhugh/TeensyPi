<?php
$getMaxChips        = "1";
$showChip           = $getMaxChips + 1;
$getChipCount       = $showChip + 1;
$getChipAddress     = $getChipCount + 1;
$getChipStatus      = $getChipAddress + 1;
$setSwitchState     = $getChipStatus + 1;
$getAllStatus       = $setSwitchState + 1;
$getChipType        = $getAllStatus + 1;
$getAllChips        = $getChipType + 1;
$getActionArray     = "A";
$updateActionArray  = "B";
$getActionStatus    = "C";
$getMaxActions      = "D";
$setActionSwitch    = "E";
$saveToEEPROM       = "F";
$getEEPROMstatus    = "G";
$getNewSensors      = "H";
$masterStop         = "I";
$getMaxPids         = "J";
$masterPidStop      = "K";
$getPidStatus       = "L";
$updatePidArray     = "M";
$getPidArray        = "N";

$on  = "1";
$off = "0";
$tooHot =  "H";
$tooCold = "C";
$socBufSize = 2048;

$dummyAddr="0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00";

error_reporting(E_ALL);

  /* Get the port for the WWW service. */
  $service_port = getservbyname('www','tcp');

  /* Get the file address for the target host. */
  $address = "/tmp/teensypi";


function makeASocket($service_port, $address)
{
  /* Create a TCP/IP socket. */
  $socket = socket_create(AF_UNIX, SOCK_STREAM, 0);
  if ($socket === false) {
      echo "socket_create() failed: reason: " . socket_strerror(socket_last_error()) . "\n";
  }
  $result = socket_connect($socket, $address, $service_port);
  if ($result === false) {
      echo "socket_connect() failed.\nReason: ($result) " . socket_strerror(socket_last_error($socket)) . "\n";
  }
  return $socket;
}
?>

