<?php
$username="teensypi";
$password="teensypi";
$database="teensypi";

$link = mysqli_connect("localhost",$username,$password);
@mysqli_select_db($link,$database) or die( "Unable to select database");

?>
