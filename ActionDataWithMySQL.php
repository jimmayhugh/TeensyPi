<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<META HTTP-EQUIV="CACHE-CONTROL" CONTENT="NO-CACHE">
<title> Action Data </title>
<link rel="stylesheet" type="text/css" href="style.css"/>
<script type="text/javascript" src="//ajax.googleapis.com/ajax/libs/jquery/1.8.2/jquery.min.js"></script>
<!-- <script type="text/javascript" src="js/jquery.js"></script> -->
<style>
input[type='text'] { font-size: 18px; text-align: center;}
input:focus, textarea:focus{background-color: lightgrey;}
select[type='text'] { font-size: 18px; text-align: center;}
</style>
</head>
  <body>
    <?php 
      include ("header.html");
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
            <?php
              include_once("accessDatabase.php");
              include_once("makeASocket.php");
              $tempOptionStr="";
              $switchOptionStr="";
              $escapedTempName="";
              $escapedTcName="";
              $escapedThName="";

	            $newSocket = makeASocket($service_port, $address);
	            $in = $getEEPROMstatus."\n";
	            socket_write($newSocket, $in, strlen($in));
	            $eepromStatus = socket_read($newSocket, $socBufSize);
	            socket_close($newSocket);
	            // echo "eepromStatus = ".$eepromStatus."<br />";
                            
              if($_POST["update"] === "update")
              {
                $tempAddrQuery = "select * from chipNames where name='".$_POST["tempAddress"]."'";
                $tempAddrResult = mysqli_query($link, $tempAddrQuery);
                $tempAddrObj = mysqli_fetch_object($tempAddrResult);
		            $updateArrayData1 = $_POST["enable"]." 0 ".$tempAddrObj->address." ";
		            
                $tcAddrQuery = "select * from chipNames where name='".$_POST["tcSwitchAddr"]."'";
                $tcAddrResult = mysqli_query($link, $tcAddrQuery);
                $tcAddrObj = mysqli_fetch_object($tcAddrResult);
		            $updateArrayData2 = $_POST["tcTemp"]." ".$_POST["tcDelay"]." ".$tcAddrObj->address." ";
		            
                $thAddrQuery = "select * from chipNames where name='".$_POST["thSwitchAddr"]."'";
                $thAddrResult = mysqli_query($link, $thAddrQuery);
                $thAddrObj = mysqli_fetch_object($thAddrResult);
		            $updateArrayData3 = $_POST["thTemp"]." ".$_POST["thDelay"]." ".$thAddrObj->address." ";
		            
		            $newSocket = makeASocket($service_port, $address);
		            $in = $updateActionArray." ".$_POST["actionCnt"]." 1 ".$updateArrayData1."\n";
		            socket_write($newSocket, $in, strlen($in));
		            $out = socket_read($newSocket, $socBufSize);
		            socket_close($newSocket);
		            $out1 = $out;

		            $newSocket = makeASocket($service_port, $address);
		            $in = $updateActionArray." ".$_POST["actionCnt"]." 2 ".$updateArrayData2."\n";
		            socket_write($newSocket, $in, strlen($in));
		            $out = socket_read($newSocket, $socBufSize);
		            socket_close($newSocket);
		            $out2 = $out;

		            $newSocket = makeASocket($service_port, $address);
		            $in = $updateActionArray." ".$_POST["actionCnt"]." 3 ".$updateArrayData3."\n";
		            socket_write($newSocket, $in, strlen($in));
		            $out = socket_read($newSocket, $socBufSize);
		            socket_close($newSocket);
		            $out3 = $out;

		            $newSocket = makeASocket($service_port, $address);
		            $in = $saveToEEPROM."\n";
		            socket_write($newSocket, $in, strlen($in));
		            $out = socket_read($newSocket, $socBufSize);
		            socket_close($newSocket);
		            sleep(2);
		            $escapedTempName = mysqli_real_escape_string ($link , $_POST["tempName"]);
		            $escapedTcName = mysqli_real_escape_string ($link , $_POST["tcName"]);
		            $escapedThName = mysqli_real_escape_string ($link , $_POST["thName"]);
                $query = "update action set active='".$_POST["enable"]."',tempAddr='".$tempAddrObj->address."',tcAddr='".$tcAddrObj->address."',tcTrigger='".$_POST["tcTemp"]."',tcDelay='".$_POST["tcDelay"]."',thAddr='".$thAddrObj->address."',thTrigger='".$_POST["thTemp"]."',thDelay='".$_POST["thDelay"]."' where id='".$_POST["actionCnt"]."'";
                $result=mysqli_query($link,$query);
/*
                if($result === TRUE)
                {
                  echo "query success";
                }else{
                  echo "query failed";
                }
*/
		            echo "<h2>Action Data Updated</h2>";
		            mysql_free_result($tempAddrResult);
		            mysql_free_result($tcAddrResult);
		            mysql_free_result($thAddrResult);
              }elseif($_POST["restoreall"] === "restoreall"){
          			 echo "<h2>All Action Data Restored</h2>";
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
		              $updateArrayData1 = $row[1]." 0 ".$row[3]." ";
		              $updateArrayData2 = $row[6]." ".$row[7]." ".$row[5]." ";
		              $updateArrayData3 = $row[10]." ".$row[11]." ".$row[9]." ";
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
              }else{
          			echo "<h2>Action Data</h2>";
              }

              $newSocket = makeASocket($service_port, $address);
              $in = "$getMaxChips\n";
              socket_write($newSocket, $in, strlen($in));
              $out = socket_read($newSocket, $socBufSize);
              socket_close($newSocket);
              $maxChipCnt = $out;

              $newSocket = makeASocket($service_port, $address);
              $in = "$getChipCount\n";
              socket_write($newSocket, $in, strlen($in));
              $out = socket_read($newSocket, $socBufSize);
              socket_close($newSocket);

              for($scCnt=0; $scCnt < $maxChipCnt; $scCnt++)
              {
                $newSocket = makeASocket($service_port, $address);
                $in = $showChip.$scCnt."\n";
                socket_write($newSocket, $in, strlen($in));
                $out = socket_read($newSocket, $socBufSize);
                socket_close($newSocket);

                $chipArray = explode(" ", $out);
                $chipAddressArray = explode(",", $chipArray[0]);
                
                if(($chipAddressArray[0] === "0x28") || ($chipAddressArray[0] === "0x12"))
                {
                  if($chipAddressArray[0] === "0x28")
                  {
                    $tempQueryStr = "select * from chipNames where address='".$chipArray[0]."'";
                    $tempQueryResult = mysqli_query($link, $tempQueryStr);
                    $tempQueryRow = mysqli_fetch_row($tempQueryResult);
                    if($tempQueryRow[2] != "")
                    { 
                      $tempOptionStr.="<option value=\"".$tempQueryRow[2]."\">".$tempQueryRow[2]."</option>";
                    }
                    mysqli_free_result($tempQueryResult);
                  }else if($chipAddressArray[0] === "0x12"){
                    $switchQueryStr = "select * from chipNames where address='".$chipArray[0]."'";
                    $switchQueryResult = mysqli_query($link, $switchQueryStr);
                    $switchQueryRow = mysqli_fetch_row($switchQueryResult);
                    if($switchQueryRow[2] != "")
                    { 
                      $switchOptionStr.="<option value=\"".$switchQueryRow[2]."\">".$switchQueryRow[2]."</option>";
                    }
                    mysqli_free_result($switchQueryResult);
                  }
                }
              }
              
              $actionCnt = $_POST["actionCnt"];
              $newSocket = makeASocket($service_port, $address);
              $in = $getActionArray.$actionCnt."\n";
              socket_write($newSocket, $in, strlen($in));
              $out = socket_read($newSocket, $socBufSize);
              socket_close($newSocket);
              $query = "select * from action where id=".$actionCnt;
              $result = mysqli_query($link,$query);
              //echo "result =".$result;

              $actionArray = explode(" ", $out);
              echo "<form method=\"post\" action=\"ActionDataWithMySQL.php\">
                      <table border=\"2\" cellspacing=\"0\" cellpadding=\"10\">";
              echo "<tr>
                      <td align=\"center\" colspan=\"3\">
                        Make your changes and press \"SUBMIT\"<br /><input type=\"submit\" value=\"SUBMIT\">
                      </td>
                    </tr>";
              echo "<tr>
                      <td align=\"center\" colspan=\"3\">
                        <input type=\"hidden\" name=\"update\" value=\"update\">
                        <input type=\"hidden\" name=\"actionCnt\" value=\"".$actionCnt."\">
                          Action Array #".$actionCnt."
                          <br />";
              if($actionArray[0] === "1")
              {
                echo "<font color=\"green\"><strong>ENABLED&nbsp;&nbsp;</strong></font>";
                echo "<input type=\"radio\" name=\"enable\" value=\"1\" checked>Enable&nbsp;&nbsp;</input>";
                echo "<input type=\"radio\" name=\"enable\" value=\"0\">Disable</input>";
                echo "</td></tr>";
              }else{
                echo "<font color=\"red\"><strong>DISABLED&nbsp;&nbsp;</strong></font>";
                echo "<input type=\"radio\" name=\"enable\" value=\"1\">Enable&nbsp;&nbsp;</input>";
                echo "<input type=\"radio\" name=\"enable\" value=\"0\" checked>Disable</input>";
                echo "</td></tr>";
              }
              $tempAddrArray = explode(",", $actionArray[1]);
              if($tempAddrArray[0] === "0x28")
              {
                $tempAddrQueryStr = "select * from chipNames where address='".$actionArray[1]."'";
                $tempAddrQueryResult = mysqli_query($link, $tempAddrQueryStr);
                $tempAddrQueryObj = mysqli_fetch_object($tempAddrQueryResult);
                echo "<tr>
                        <td align=\"center\" colspan=\"3\">
                          <table width=\"100%\" border=\"0\">
                            <tr>
                                <td align=\"center\">Temperature Sensor Booger Name:
                                    <select type=\"text\" name=\"tempAddress\">
                                      <option selected value=\"".$tempAddrQueryObj->name."\">".$tempAddrQueryObj->name."</option>
                                      <option value=\"UNASSIGNED\">UNASSIGNED</option>
                                       ".$tempOptionStr."
                                     </select>
                                  </td>
                                </tr>
                            </table>
                          </td>
                        </tr>";
              }else{
                echo "<tr>
                        <td align=\"center\" colspan=\"3\">";
                 echo"  Temperature Sensor Boogie-Woogie Name:
                            <select type=\"text\" name=\"tempAddress\">
                              <option value=\"UNASSIGNED\">UNASSIGNED</option>
                              ".$tempOptionStr."
                            </select>
                        </td>
                      </tr>";
              }
              echo "<tr>
                      <td align=\"center\" colspan=\"3\">
                        Too Cold Parameters
                      </td>
                    </tr>
                    <tr>
                      <td align=\"center\">
                        Trigger Temp
                      </td>
                      <td align=\"center\">
                        Name:
                      </td>
                      <td align=\"center\">
                        Delay (Seconds)
                      </td>
                    </tr>
                    <tr>
                      <td align=\"center\">
                        <input type=\"text\" size=\"10\" name=\"tcTemp\" value=\"".$actionArray[2]."\">
                      </td>";
              $tcAddressArray = explode(",", $actionArray[3]);
              if($tcAddressArray[0] === "0x12")
              {
                $tcAddrQueryStr = "select * from chipNames where address='".$actionArray[3]."'";
                $tcAddrQueryResult = mysqli_query($link, $tcAddrQueryStr);
                $tcAddrQueryObj = mysqli_fetch_object($tcAddrQueryResult);
                echo "<td align=\"center\">
                        <select type=\"text\" name=\"tcSwitchAddr\">
                          <option selected value=\"".$tcAddrQueryObj->name."\">".$tcAddrQueryObj->name."</option>
                          <option value=\"UNASSIGNED\">UNASSIGNED</option>
                          ".$switchOptionStr."
                        </select>
                      </td>";
              }else{
                echo "<td align=\"center\">
                        <select type=\"text\" name=\"tcSwitchAddr\">
                          <option value=\"UNASSIGNED\">UNASSIGNED</option>
                          ".$switchOptionStr."
                        </select>
                      </td>";
              }          
              echo "<td align=\"center\">
                      <input type=\"text\" size=\"10\" name=\"tcDelay\" value=\"".$actionArray[5]."\">
                    </td>
                  </tr>"; 
              echo "<tr>
                      <td align=\"center\" colspan=\"3\">
                        Too Hot Parameters
                      </td>
                    </tr>
                    <tr>
                      <td align=\"center\">
                        Trigger Temp
                      </td>
                      <td align=\"center\">
                        Name:
                      </td>
                      <td align=\"center\">
                        Delay (Seconds)
                      </td>
                    </tr>";
              echo "<tr>
                      <td align=\"center\">
                        <input type=\"text\" size=\"10\" name=\"thTemp\" value=\"".$actionArray[7]."\">
                        </td>";
              $thAddressArray = explode(",", $actionArray[8]);
              if($thAddressArray[0] === "0x12")
              {
                $thAddrQueryStr = "select * from chipNames where address='".$actionArray[8]."'";
                $thAddrQueryResult = mysqli_query($link, $thAddrQueryStr);
                $thAddrQueryObj = mysqli_fetch_object($thAddrQueryResult);
                echo "<td align=\"center\">
                        <select type=\"text\" name=\"thSwitchAddr\">
                          <option selected value=\"".$thAddrQueryObj->name."\">".$thAddrQueryObj->name."</option>
                          <option value=\"UNASSIGNED\">UNASSIGNED</option>
                          ".$switchOptionStr."
                        </select>
                      </td>";
              }else{
                echo "<td align=\"center\">
                        <select type=\"text\" name=\"thSwitchAddr\">
                          <option value=\"UNASSIGNED\">UNASSIGNED</option>
                          ".$switchOptionStr."
                        </select>
                      </td>";
              }          
              echo "<td align=\"center\">
                      <input type=\"text\" size=\"10\" name=\"thDelay\" value=\"".$actionArray[10]."\"
                    </td>
                  </tr>"; 
              echo "<tr>
                      <td align=\"center\" colspan=\"3\">
                        Make your changes and press \"SUBMIT\"<br />
                        <input type=\"submit\" value=\"SUBMIT\">
                      </td>
                    </tr>";
              echo "</table>
                  </form>";


            ?>
          </div>
        </td>
      </tr>
      <tr>
        <td valign="top" align="left" width="150">
        <?php 
        include ("menu.html");
        ?>
        </td>
      </tr>
    </table>
    <?php
      mysqli_free_result($result);
      mysqli_close($link);
    ?>
  </body>
</html>
 
