<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title> Sensor / Action Status </title>
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
    $("#container").load("updateActionStatusWithMySQL.php");
   var refreshId = setInterval(function() {
      $("#container").load('updateActionStatusWithMySQL.php');
   }, 500);
   $.ajaxSetup({ cache: false });
});
</script>
</head>
  <body>
    <?php 
      include_once("accessDatabase.php");
      include_once("makeASocket.php");
      include_once("header.html");
    ?> 
    <!-- Table for Main Body -->
    <table width="100%" border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td>
        <?php 
            include ("menu.html");
            if($_POST["restoreall"] === "restoreall")
            {
              $h2Str = "<h2>All Action Data Restored</h2>";
              $query = "SELECT * FROM action";
              // echo "query = ".$query."<br />";
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
              while($row = mysqli_fetch_array($result))
              {
                // echo $row[0].", ".$row[1].", ".$row[2].", ".$row[3]."<br />";
                // echo $row[4].", ".$row[5].", ".$row[6].", ".$row[7]."<br />";
                // echo $row[8].", ".$row[9].", ".$row[10].", ".$row[11]."<br />";
		            $updateArrayData1 = $row[1]." 0 ".$row[2]." ";
		            $updateArrayData2 = $row[4]." ".$row[5]." ".$row[3]." ";
		            $updateArrayData3 = $row[7]." ".$row[8]." ".$row[6]." ";
		            $newSocket = makeASocket($service_port, $address);
		            $in = $updateActionArray." ".$row[0]." 1 ".$updateArrayData1."\n";
		            socket_write($newSocket, $in, strlen($in));
		            $out = socket_read($newSocket, $socBufSize);
		            socket_close($newSocket);
		            $out1 = $out;

		            $newSocket = makeASocket($service_port, $address);
		            $in = $updateActionArray." ".$row[0]." 2 ".$updateArrayData2."\n";
		            socket_write($newSocket, $in, strlen($in));
		            $out = socket_read($newSocket, $socBufSize);
		            socket_close($newSocket);
		            $out2 = $out;

		            $newSocket = makeASocket($service_port, $address);
		            $in = $updateActionArray." ".$row[0]." 3 ".$updateArrayData3."\n";
		            socket_write($newSocket, $in, strlen($in));
		            $out = socket_read($newSocket, $socBufSize);
		            socket_close($newSocket);
		            $out3 = $out;
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
              include ("updateActionStatusWithMySQL.php");
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
