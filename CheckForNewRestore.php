<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
    <title> Sensor /Switch Setup </title>
    <link rel="stylesheet" type="text/css" href="style.css"/>
    <script type="text/javascript" src="//ajax.googleapis.com/ajax/libs/jquery/1.8.2/jquery.min.js">
    </script>
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
    include_once("makeASocket.php");
    include_once("accessDatabase.php");
    include_once("header.html"); 
  ?> 
<!-- Table for Main Body -->
    <table width="100%" border="2" cellspacing="0" cellpadding="2">
      <tr>
        <td>
          <?php 
            include ("menu.html");
            if(isset($_POST["checknew"]) && $_POST["checknew"] === "checknew")
            {
              $newSocket = makeASocket($service_port, $address);
              $in = $getNewSensors."\n";
              socket_write($newSocket, $in, strlen($in));
              $eepromStatus = socket_read($newSocket, $socBufSize);
              socket_close($newSocket);
            }
          ?>
        </td>
      </tr>
      <tr>
        <td  align="center" colspan="6">
          <table width="100%" border="1" cellspacing="0" cellpadding="2">
            <tr>
              <td align="center" colspan="6">
                <h2>Check For New Chips / Restore All Action Settings</h2>
              </td>
            </tr>
            <tr>
              <td>
                <table width="100%" border="1" cellspacing="0" cellpadding="2">
                  <tr>
                    <td align="center" width="50%">
                      <font color="red">
                        USE THIS BUTTON WITH EXTREME CARE!!
                        <br />
                      </font>
                      <form method="post" action="CheckForNewRestore.php">
                        <input type="hidden" name="checknew" value="checknew">
                        <input type="submit" value="Scan For New Chips">
                      </form>
                      <font color="red">
                        USING THIS BUTTON TURNS OFF ALL SWITCHES AND CLEARS ALL ACTIONS!!
                        <br />
                        USE THIS BUTTON WITH EXTREME CARE!!
                        <br />              
                      </font>      
                    </td>
                    <td align="center">
                      <?php
                        $checkDBActiveStr = "SELECT * from `action` WHERE `tempAddr` != \"0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00\"";
                        $result = mysqli_query($link, $checkDBActiveStr);
                        $actionCnt = mysqli_num_rows($result);
                        $checkDBActiveStr = "SELECT * from `pid` WHERE `tempAddr` != \"0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00\"";
                        $result = mysqli_query($link, $checkDBActiveStr);
                        $pidCnt = mysqli_num_rows($result);
                        if(($pidCnt > 0) || ($actionCnt > 0))
                        {
                          $restoreAllStr = 
                            "
                              <form method=\"post\" action=\"ActionStatus.php\">
                                <input type=\"hidden\" name=\"restoreall\" value=\"restoreall\">
                                <input type=\"submit\" value=\"RESTORE ALL\">
                              </form>
                            ";
                        }else{
                          $restoreAllStr = 
                            "
                              <form method=\"post\" action=\"ActionStatus.php\">
                                <input type=\"hidden\" name=\"restoreall\" value=\"restoreall\">
                                <input type=\"submit\" value=\"RESTORE ALL\" disabled>
                              </form>
                            ";
                        }
                        echo $restoreAllStr;
                      ?>
                    </td>
                  </tr>
                </table>
              </td>
            </tr>
          </table>      
        </td>
      </tr>  
      <tr>
        <td align="center" colspan="6">
          <?php
            if(isset($_POST["dbclear"]) && $_POST["dbclear"] === "dbclear")
            {
              $dbClearStr = "UPDATE `action` SET `active`='0', `tempAddr`='0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', `tcAddr`='0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', `tcTrigger`='-255', `tcDelay`='0', `thAddr`='0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', `thTrigger`='255', `thDelay`='0', `lcd`='0', `rgb`='0' WHERE 1";
              $result = mysqli_query($link, $dbClearStr);
              $dbClearStr = "UPDATE `chipNames` SET `address`='0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00',`name`='UNASSIGNED' WHERE 1";
              $result = mysqli_query($link, $dbClearStr);
              $dbClearStr = "UPDATE `pid` SET `enabled`='0', `tempAddr`='0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', `setpoint`='70', `switchAddr`='0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', `kp`='0', `ki`='0', `kd`='0', `direction`='0', `windowSize`='5000' WHERE 1";
              $result = mysqli_query($link, $dbClearStr);
              $dbClearStr = "DELETE FROM `actionGraph` WHERE 1";
              $result = mysqli_query($link, $dbClearStr);
              $dbClearStr = "DELETE FROM `pidGraph` WHERE 1";
              $result = mysqli_query($link, $dbClearStr);
              
            }
          ?>
          <font color="red">
            USE THIS BUTTON WITH EXTREME CARE!!
            <br />              
            USING THIS BUTTON RESETS ALL DATABASE VALUES!!
            <br />
          </font>
          <form method="post" action="CheckForNewRestore.php">
            <input type="hidden" name="dbclear" value="dbclear">
            <input type="submit" value="RESET DATABASE VALUES">
          </form>
          <font color="red">
            USING THIS BUTTON RESETS ALL DATABASE VALUES!!
            <br />
            USE THIS BUTTON WITH EXTREME CARE!!
            <br />
          </font>              
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
  </body>
</html>
