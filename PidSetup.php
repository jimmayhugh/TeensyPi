<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<META HTTP-EQUIV="CACHE-CONTROL" CONTENT="NO-CACHE">
<?php
        include_once("accessDatabase.php");
        include_once("makeASocket.php");
        $pidCnt = $_POST["pidCnt"];
        echo "<title> PID#".$pidCnt." Data </title>";
?>
<link rel="stylesheet" type="text/css" href="style.css"/>
<script type="text/javascript" src="//ajax.googleapis.com/ajax/libs/jquery/1.8.2/jquery.min.js"></script>
<!-- <script type="text/javascript" src="js/jquery.js"></script> -->
<style>
input[type='text'] { font-size: 25px; text-align: center;}
input:focus, textarea:focus{background-color: lightgrey;}
select[type='text'] { font-size: 25px; text-align: center;}
option[type='text'] { font-size: 25px; text-align: center;}
</style>
</head>
  <body>
    <?php 
      include ("header.html");
      $bodyStr = "";
    ?> 
    <!-- Table for Main Body -->
    <table width="100%" border="1" cellspacing="0" cellpadding="1">
      <tr>
        <td valign="top" align="left" width="150">
        <?php 
        include ("menu.html");
        ?>
        </td>
      </tr>
      <tr>
        <td align="center" border="1">
          <div id="container">
            <h2> PID Setup ID# <?php echo $pidCnt; ?><h2>
            <?php
              $newSocket = makeASocket($service_port, $address);
              $pidIn = $getPidArray.$pidCnt."\n";
//              echo "pidIn = ".$pidIn."<br />";
              socket_write($newSocket, $pidIn, strlen($pidIn));
              $pidOut = socket_read($newSocket, $socBufSize);
//              echo "pidOut = ".$pidOut."<br />";
              socket_close($newSocket);
              if($pidOut === "Z")
              {
                echo "Invalid Return<br />";
              }else{
                $pidArray = explode(" ", $pidOut);
                
                $pidEnabledStr        = trim($pidArray[0]);
                $pidTempStr           = trim($pidArray[1]);
                $pidTempStatusStr     = trim($pidArray[2]);
                $pidSetPointStr       = trim($pidArray[3]);
                $pidSwitchStr         = trim($pidArray[4]);
                $pidSwitchStatusStr   = trim($pidArray[5]);
                $pidKpStr             = trim($pidArray[6]);
                $pidKiStr             = trim($pidArray[7]);
                $pidKdStr             = trim($pidArray[8]);
                $pidDirectionStr      = trim($pidArray[9]);
                $pidWindowSizeStr     = trim($pidArray[10]);

                $query = "select * from chipNames where address='".$pidTempStr."'";
                $result = mysqli_query($link,$query);
                if($result)
                {
                  $pidObj = mysqli_fetch_object($result);
                  $pidTempName = $pidObj->name;
                }else{
                  $pidTempName = "UNASSIGNED";
                }
                mysqli_free_result($result);
                
                $query = "select * from chipNames where address='".$pidSwitchStr."'";
                $result = mysqli_query($link,$query);
                if($result)
                {
                  $pidObj = mysqli_fetch_object($result);
                  $pidSwitchName = $pidObj->name;
                }else{
                  $pidSwitchName = "UNASSIGNED";
                }
                mysqli_free_result($result);
              }
              
            ?>
          </div>
        </td>
      </tr>
      <tr>
            <?php
              $bodyStr .= 
              "<div id=\"pid".$pidCnt."\">
                <td valign=\"top\" align=\"center\">
                  <table border=\"1\" width=\"50%\" cellspacing=\"0\" cellpadding=\"10\">
                    <tr>
                      <td align=\"center\" colspan=\"4\">
                        <div style=\"vertical-align:middle; min-height:50px;\">
                          <font size=\"5\" color=\"blue\"><strong>
                           PID# ".$pidCnt." Setup
                          </strong></font>
                        <form name=\"pidSetup\" method=\"post\" action=\"PidStatus.php\">
                          <input type=\"hidden\" name=\"pidSetup\" value=\"pidSetup\">
                          <input type=\"hidden\" name=\"pidCnt\" value=\"".$pidCnt."\">
                          <input  style=\" font-size : 25px\" type=\"submit\" value=\"MODIFY\">
                      </td>
                    </tr>
                    <tr>
                      <td align=\"center\" colspan=\"4\">";
              if($pidEnabledStr === "0")
              {
                $bodyStr .= 
                   "
                      <input type=\"radio\" name=\"pidEnable\" value=\"1\"><font size=\"5\">Enable</font></input>
                      <input type=\"radio\" name=\"pidEnable\" value=\"0\" checked><font size=\"5\" color=\"red\"><strong>DISABLED</strong></font></input>
                    ";
              }else if($pidEnabledStr === "1"){
                $bodyStr .= 
                  "
                      <input type=\"radio\" name=\"pidEnable\" value=\"1\" checked><font size=\"5\" color=\"green\"><strong>ENABLED</strong></font></input>
                      <input type=\"radio\" name=\"pidEnable\" value=\"0\"><font size=\"5\">Disable</font></input>
                  ";
              }
              
              $bodyStr .= 
                 "</td>
               </tr>
               <tr>
                <td align=\"center\" colspan=\"4\">
                  <div style=\"vertical-align:middle; min-height:50px;\">
                    <font size=\"5\" color=\"blue\"><strong>
                      Temperature Chip
                      <select style=\" font-size : 25px\" name=\"pidTempAddr\">
                        <option value=\"".$pidTempStr."\" selected>".$pidTempName."</option>";
                        if($pidTempStr !== $dummyAddr)
                        {
                          $bodyStr .=
                          "<option value=\"".$dummyAddr."\">UNASSIGNED</option>";
                        }
                      $query = "select * from chipNames";
                      $result = mysqli_query($link,$query);
                      while($pidObj = mysqli_fetch_object($result))
                      {
                        $addrStrArray = explode(",", $pidObj->address);
                        if($addrStrArray[0] === "0x28")
                        {
                          $bodyStr .= "<option value=\"".$pidObj->address."\">".$pidObj->name."</option>";
                        }
                      }
                      mysqli_free_result($result);
              $bodyStr .= 
                 "
                   </select>
                    </strong></font>
                  </div>
                </td>
               </tr>
               <tr>
                <td align=\"center\" colspan=\"4\">
                  <div style=\"vertical-align:middle; min-height:50px;\">
                    <font size=\"5\" color=\"blue\"><strong>
                      Set Point
                      <input type=\"text\" name=\"pidSetPoint\" size=\"10\" maxlength=\"10\" value=\"".$pidSetPointStr."\"></input>
                    </strong></font>
                  </div>
                </td>
               </tr>
               <tr>
                <td align=\"center\" colspan=\"4\">
                  <div style=\"vertical-align:middle; min-height:50px;\">
                    <font size=\"5\" color=\"blue\"><strong>
                      Switch Chip
                      <select style=\" font-size : 25px\" name=\"pidSwitchAddr\">
                        <option value=\"".$pidSwitchStr."\" selected>".$pidSwitchName."</option>";
                        if($pidSwitchStr !== $dummyAddr)
                        {
                          $bodyStr .=
                          "<option value=\"".$dummyAddr."\">UNASSIGNED</option>";
                        }
                      $query = "select * from chipNames";
                      $result = mysqli_query($link,$query);
                      while($pidObj = mysqli_fetch_object($result))
                      {
                        $addrStrArray = explode(",", $pidObj->address);
                        if($addrStrArray[0] === "0x12")
                        {
                          $bodyStr .= "<option value=\"".$pidObj->address."\">".$pidObj->name."</option>";
                        }
                      }
                      mysqli_free_result($result);
              $bodyStr .= 
                 "
                   </select>
                    </strong></font>
                  </div>
                </td>
                  ";
                
              $bodyStr .= "
              </tr>
              <tr>
                <td align=\"center\" colspan=\"4\">
                  <div style=\"vertical-align:middle; min-height:50px;\">
                    <font size=\"5\" color=\"blue\"><strong>PID Variables<br />
                      Kp:&nbsp;<input type=\"text\" size=\"10\" maxlength=\"10\" name=\"pidKp\" value=\"".$pidKpStr."\"></input><br />
                      Ki:&nbsp;<input type=\"text\" size=\"10\" maxlength=\"10\" name=\"pidKi\" value=\"".$pidKiStr."\"></input><br />
                      Kd:&nbsp;<input type=\"text\" size=\"10\" maxlength=\"10\" name=\"pidKd\" value=\"".$pidKdStr."\"></input>
                    </strong></font>
                  </div>
              ";
              
              $bodyStr .= "
                </td>
              </tr>
              <tr>  
                <td align=\"center\" colspan=\"4\">
                <font size=\"5\" color=\"blue\"><strong>
                  Direction<br />
                </strong></font>";
              if($pidDirectionStr === "0")
              {
                $bodyStr .= 
                   "
                      <input type=\"radio\" name=\"pidDirection\" value=\"0\" checked><font size=\"5\" color=\"red\"><strong>FORWARD</strong></font></input>
                      <input type=\"radio\" name=\"pidDirection\" value=\"1\"><font size=\"5\" >Reverse</font></input>
                    ";
              }else if($pidDirectionStr === "1"){
                $bodyStr .= 
                  "
                      <input type=\"radio\" name=\"pidDirection\" value=\"0\"><font size=\"5\">Forward</font></input>
                      <input type=\"radio\" name=\"pidDirection\" value=\"1\" checked><font size=\"5\" color=\"blue\"><strong>REVERSE</strong></font></input>
                  ";
              }
              
              $bodyStr .="</div>";
              $bodyStr .= "
                </td>
              </tr>
              <tr>
                <td align=\"center\" colspan=\"4\">
                  <div style=\"vertical-align:middle; min-height:50px;\">
                    <font size=\"5\" color=\"blue\"><strong>PID Window Size (Milliseconds)<br />
                      <input type=\"text\" size=\"6\" maxlength=\"6\" name=\"pidWindowSize\"value=\"".$pidWindowSizeStr."\"></input>
                    </font>
                    </strong></font>
                  </div>";
              $bodyStr .= "
                      </td>
                    </tr>
                    <tr>
                      <td align=\"center\" colspan=\"4\">
                        <div style=\"vertical-align:middle; min-height:50px;\">
                            <input type=\"hidden\" name=\"pidCnt\" value=\"".$pidCnt."\">
                            <input  style=\" font-size : 25px\" type=\"submit\" value=\"MODIFY\">
                          </form><br />
                          <font size=\"5\" color=\"blue\"><strong>
                           PID# ".$pidCnt." Setup
                          </strong></font>
                      </td>
                    </tr>
                  </table>
                </td>
              </div>";
              echo $bodyStr;
            ?>
        </td>
      <tr>
        <td valign="top" align="left" width="150">
        <?php 
        include ("menu.html");
        ?>
        </td>
      </tr>
    </table>
    <?php
      mysqli_close($link);
    ?>
  </body>
</html>
 
