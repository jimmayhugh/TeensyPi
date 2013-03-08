<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title> PID Status </title>
<link rel="stylesheet" type="text/css" href="style.css"/>
<style type="text/css" rel="stylesheet">
  #masterStop {
                background-color: #f00;
                font-size: 100%;
                width: 20em;
                height: 5em;
                font-weight: bold;
              }
</style>
<script type="text/javascript" src="//ajax.googleapis.com/ajax/libs/jquery/1.8.2/jquery.min.js"></script>
<!-- <script type="text/javascript" src="js/jquery.js"></script> -->
<script>
 $(document).ready(function() {
    $("#container").load("updatePidStatusWithMySQL.php");
   var refreshId = setInterval(function() {
      $("#container").load('updatePidStatusWithMySQL.php');
   }, 1000);
   $.ajaxSetup({ cache: false });
});
</script>
</head>
  <body>
    <?php 
      include_once("accessDatabase.php");
      include_once("makeASocket.php");
      include_once("header.html");

      if(isset($_POST["masterPidStop"]) && $_POST["masterPidStop"] === "masterPidStop")
      {
        $newSocket = makeASocket($service_port, $address);
        $in = $masterPidStop."\n";
        socket_write($newSocket, $in, strlen($in));
        $chipX = socket_read($newSocket, $socBufSize);
        socket_close($newSocket);
      }
      
      if(isset($_POST["pidEnable"]) && ($_POST["pidEnable"] === "pidEnable" ) && isset($_POST["pidCnt"]))
      {  
        $newSocket = makeASocket($service_port, $address);
        $in = $setPidArray." ".$_POST["pidCnt"]." 1\n";
        socket_write($newSocket, $in, strlen($in));
        $eepromStatus = socket_read($newSocket, $socBufSize);
        socket_close($newSocket);
      }
      
      if(isset($_POST["pidDisable"]) && ($_POST["pidDisable"] === "pidDisable" ) && isset($_POST["pidCnt"]))
      {  
        $newSocket = makeASocket($service_port, $address);
        $in = $setPidArray." ".$_POST["pidCnt"]." 0\n";
        socket_write($newSocket, $in, strlen($in));
        $eepromStatus = socket_read($newSocket, $socBufSize);
        socket_close($newSocket);
      }
    ?>
    
    <!-- Table for Main Body -->
    <table width="100%" border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td>
        <?php 
            include ("menu.html");
            if(isset($_POST["pidSetup"]) && $_POST["pidSetup"] === "pidSetup")
            {
//              echo "<br />pidCnt=".$_POST["pidCnt"]."<br />pidEnable=".$_POST["pidEnable"]."<br />pidTempAddr=".$_POST["pidTempAddr"]."<br />pidSetPoint=".$_POST["pidSetPoint"]."<br />pidSwitchAddr=".$_POST["pidSwitchAddr"]."<br />pidKp=".$_POST["pidKp"]."<br />pidKi=".$_POST["pidKi"]."<br />pidKd=".$_POST["pidKd"]."<br />pidDirection=".$_POST["pidDirection"]."<br />pidWindowSize=".$_POST["pidWindowSize"];
              $updatePidStr = "M ".$_POST["pidCnt"]." ".$_POST["pidEnable"]." ".$_POST["pidTempAddr"]." ".$_POST["pidSetPoint"]." ".$_POST["pidSwitchAddr"]." ".$_POST["pidKp"]." ".$_POST["pidKi"]." ".$_POST["pidKd"]." ".$_POST["pidDirection"]." ".$_POST["pidWindowSize"]."\n";
//              echo "<br />updatePidStr = ".$updatePidStr."<br />";
              $newSocket = makeASocket($service_port, $address);
              $pidIn = $updatePidStr;
//              echo "pidIn = ".$pidIn.", length = ".strlen($pidIn)."<br />";
              socket_write($newSocket, $pidIn, strlen($pidIn));
              $pidOut = socket_read($newSocket, $socBufSize);
//              echo "pidOut = ".$pidOut."<br />";
              socket_close($newSocket);

		          $newSocket = makeASocket($service_port, $address);
		          $in = $saveToEEPROM."\n";
		          socket_write($newSocket, $in, strlen($in));
		          $out = socket_read($newSocket, $socBufSize);
		          socket_close($newSocket);
		          sleep(2);

              $query = "UPDATE pid SET enabled='".$_POST["pidEnable"]."',tempAddr='".$_POST["pidTempAddr"]."',setpoint='".$_POST["pidSetPoint"]."',switchAddr='".$_POST["pidSwitchAddr"]."',kp='".$_POST["pidKp"]."',ki='".$_POST["pidKi"]."',kd='".$_POST["pidKd"]."',direction='".$_POST["pidDirection"]."',windowSize='".$_POST["pidWindowSize"]."' WHERE id='".$_POST["pidCnt"]."'";
//              echo $query."<br />";
              $result=mysqli_query($link, $query);
/*
              if($result === FALSE)
              {
                echo "query failed";
              }else{
                echo "query success";
              }
              echo "<br />result = ".$result."<br />";
*/
            }
            if(isset($_POST["restoreall"]) && $_POST["restoreall"] === "restoreall")
            {
              $h2Str = "<h2>All PID Data Restored</h2>";
              $query = "SELECT * FROM pid";
//              echo "query = ".$query."<br />";
              $result=mysqli_query($link,$query);
/*              
              if($result === FALSE)
              {
                echo "query failed";
              }else{
                echo "query success";
              }
              echo "<br />result = ".$result."<br />";
*/              
              while($pidObj = mysqli_fetch_object($result))
              {
		            $updatePidStr = $pidObj->id." ".$pidObj->enabled." ".$pidObj->tempAddr." ".$pidObj->setpoint." ".$pidObj->switchAddr." ".$pidObj->kp." ".$pidObj->ki." ".$pidObj->kd." ".$pidObj->direction." ".$pidObj->windowSize."\n";
		            $newSocket = makeASocket($service_port, $address);
		            $in = $updatePidArray.$updatePidStr;
		            socket_write($newSocket, $in, strlen($in));
		            $out = socket_read($newSocket, $socBufSize);
		            socket_close($newSocket);
		            $out1 = $out;

		          }

		          $newSocket = makeASocket($service_port, $address);
		          $in = $saveToEEPROM."\n";
		          socket_write($newSocket, $in, strlen($in));
		          $out = socket_read($newSocket, $socBufSize);
		          socket_close($newSocket);
		          sleep(2);
            }        
          ?>
        </td>
      </tr>
      <tr>
        <td valign ="top"">
          <div id="container">
            <?php
              include ("updatePidStatusWithMySQL.php");
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
  </body>
</html>
