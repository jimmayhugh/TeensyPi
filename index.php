<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title> Sensor /Switch Update </title>
<link rel="stylesheet" type="text/css" href="style.css"/>
<script type="text/javascript" src="//ajax.googleapis.com/ajax/libs/jquery/1.8.2/jquery.min.js"></script>
<!-- <script type="text/javascript" src="js/jquery.js"></script> -->
<script>
 $(document).ready(function() {
    $("#container").load("updateStatus.php");
   var refreshId = setInterval(function() {
      $("#container").load('updateStatus.php');
   }, 1000);
   $.ajaxSetup({ cache: false });
});
</script>
</head>
<body>
<?php
  include("header.html");
?> 
<!-- Table for Main Body -->
<table width="100%" border="0" cellspacing="0" cellpadding="2">
  <tr>
    <td>
    <?php 
      include ("menu.html");
    ?>
    </td>
  </tr>
  <tr>
    <td  align="center" colspan="6">
      <table width="100%" border="1" cellspacing="0" cellpadding="2">
        <tr>
          <td align="center" border="2" colspan="6">
            <h2>Sensor / Switch Status</h2>
          </td>
        </tr>
        </table>      
    </td>
  </tr>  
  <tr>
    <td>
      <div id="container">
        <?php
          include ("updateStatus.php"); 
        ?>
      </div>
    </td>
  </tr>
  <tr>
    <td>
    <?php 
    include ("menu.html");
    ?>
    </td>
  </tr>
</table>
</body></html>
