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
      include ("head.html");
    ?> 
    <!-- Table for Main Body -->
    <table width="100%" border="0" cellspacing="0" cellpadding="1">
      <tr>
        <td valign="top" align="left" width="150">
        <?php 
        include ("menu.html");
        ?>
        </td>
      </tr>
      <tr>
        <td align="center">
          <div id="container">
            <?php
              include_once("accessDatabase.php");
              include_once("makeASocket.php");
              $tempOptionStr="<option value=\"".$dummyAddr."\">".$dummyAddr."</option>";
              $switchOptionStr="<option value=\"".$dummyAddr."\">".$dummyAddr."</option>";

	            $newSocket = makeASocket($service_port, $address);
	            $in = $getEEPROMstatus."\n";
	            socket_write($newSocket, $in, strlen($in));
	            $eepromStatus = socket_read($newSocket, $socBufSize);
	            socket_close($newSocket);
	            // echo "eepromStatus = ".$eepromStatus."<br />";
                            
              if($_POST["update"] === "update")
              {
		            $updateArrayData1 = $_POST["enable"]." 0 ".$_POST["tempAddress"]." ";
		            $updateArrayData2 = $_POST["tcTemp"]." ".$_POST["tcDelay"]." ".$_POST["tcSwitchAddr"]." ";
		            $updateArrayData3 = $_POST["thTemp"]." ".$_POST["thDelay"]." ".$_POST["thSwitchAddr"]." ";
		            
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
                $query = "update action set active='".$_POST["enable"]."',tempName='".$escapedTempName."',tempAddr='".$_POST["tempAddress"]."',tcName='".$escapedTcName."',tcAddr='".$_POST["tcSwitchAddr"]."',tcTrigger='".$_POST["tcTemp"]."',tcDelay='".$_POST["tcDelay"]."',thName='".$escapedThName."',thAddr='".$_POST["thSwitchAddr"]."',thTrigger='".$_POST["thTemp"]."',thDelay='".$_POST["thDelay"]."' where id='".$_POST["actionCnt"]."'";
//                echo "query = ".$query;
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
               }elseif($_POST["restore"] === "restore"){
          			 echo "<h2>Action Data Restored</h2>";
                 $query = "SELECT * FROM action WHERE id =".$_POST["actionCnt"];
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
                $row = mysqli_fetch_array($result);
                // echo $row[0].", ".$row[1].", ".$row[2].", ".$row[3]."<br />";
                // echo $row[4].", ".$row[5].", ".$row[6].", ".$row[7]."<br />";
                // echo $row[8].", ".$row[9].", ".$row[10].", ".$row[11]."<br />";
		            $updateArrayData1 = $row[1]." 0 ".$row[3]." ";
		            $updateArrayData2 = $row[6]." ".$row[7]." ".$row[5]." ";
		            $updateArrayData3 = $row[10]." ".$row[11]." ".$row[9]." ";
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

              $tempCnt = $_POST["tempCnt"];
              
              $newSocket = makeASocket($service_port, $address);
              $in = "$getMaxChips\n";
              socket_write($newSocket, $in, strlen($in));
              $out = socket_read($newSocket, $socBufSize);
              socket_close($newSocket);
              $maxChipCnt = $out;
              echo "<p>There are ".$maxChipCnt." chip arrays, with";

              $newSocket = makeASocket($service_port, $address);
              $in = "$getChipCount\n";
              socket_write($newSocket, $in, strlen($in));
              $out = socket_read($newSocket, $socBufSize);
              socket_close($newSocket);
              echo " ".$out." chip arrays that have a chip installed.<p>";

/*              echo "<table border=\"2\" cellspacing=\"0\" cellpadding=\"10\">
                      <tr>
                        <td align=\"center\">
                          Chip #
                        </td>
                        <td align=\"center\">
                          Address
                        </td>
                        <td align=\"center\">
                          Status
                        </td>
                      </tr>";
*/
              for($scCnt=0; $scCnt < $maxChipCnt; $scCnt++)
              {
                $newSocket = makeASocket($service_port, $address);
                $in = $showChip.$scCnt."\n";
                socket_write($newSocket, $in, strlen($in));
                $out = socket_read($newSocket, $socBufSize);
                socket_close($newSocket);
                // echo "<tr><td align=\"center\" colspan=\"3\">".$out."</td></tr>";
                $chipArray = explode(" ", $out);
                $chipAddressArray = explode(",", $chipArray[0]);
                if(($chipAddressArray[0] === "0x28") || ($chipAddressArray[0] === "0x12"))
                {
/*                  echo "<tr>
                          <td align=\"center\">
                            ".$scCnt."
                          </td>
                          <td align=\"center\">
                            ".$chipArray[0]."
                          </td>
                          <td align=\"center\">";
*/
                  if($chipAddressArray[0] === "0x28")
                  {
                    $tempOptionStr.="<option value=\"".$chipArray[0]."\">".$chipArray[0]."</option>";
/*                    echo $chipArray[1]."&deg;
                          </td>
                        </tr>";
*/
                  }else if($chipAddressArray[0] === "0x12"){
                    $switchOptionStr.="<option value=\"".$chipArray[0]."\">".$chipArray[0]."</option>";
/*                    if($chipArray[1] === "N" || $chipArray[1] === "78")
                    {
                      echo "ON";
                    }else if($chipArray[1] === "F" || $chipArray[1] === "70"){
                      echo "OFF";
                    }else{
                      echo $chipArray[1];
                    }
                    echo "</td>
                        </tr>";
*/
                  }
                }
              }
//              echo "</table>";
              
              if(is_numeric($tempCnt) === TRUE)
              {
                $newSocket = makeASocket($service_port, $address);
                $in = $getChipAddress.$tempCnt."\n";
                socket_write($newSocket, $in, strlen($in));
                $out = socket_read($newSocket, $socBufSize);
                socket_close($newSocket);
                $chipAddress = $out;
                $actionCnt = $tempCnt;
              }else{
              		$actionCnt = $_POST["actionCnt"];
              		if(is_numeric($actionCnt) === FALSE)
              		{
              		  $actionCnt = "0";
              		}
              }
              
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
                        Make your changes and press \"SUBMIT\" <input type=\"submit\" value=\"SUBMIT\">
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
//              echo "<tr><td colspan=\"4\">actionArray = ".$out."<br />actionCnt = ".$actionCnt."<br />tempCnt = ".$tempCnt."<br />chipAddress = ".$chipAddress."<br />actionArray[0] = ".$actionArray[0]."<br />actionArray[1] = ".$actionArray[1]."<br />actionArray[2] = ".$actionArray[2]."<br />actionArray[3] = ".$actionArray[3]."<br />actionArray[4] = ".$actionArray[4]."<br />actionArray[5] = ".$actionArray[5]."<br />actionArray[6] = ".$actionArray[6]."<br />actionArray[7] = ".$actionArray[7]."<br />actionArray[8] = ".$actionArray[8]."<br />actionArray[9] = ".$actionArray[9]."<br />actionArray[10] = ".$actionArray[10]."<br />actionArray[11] = ".$actionArray[11]."</td></tr>\n";
              
              if(is_numeric($tempCnt))
              {
                $tempAddrArray = explode(",", $chipAddress);
                echo "<tr>
                        <td>
                          Temperature Sensor Name:
                        </td>
                        <td>
                          <input type=\"text\" size=\"25\" name=\"tempName\" value=\"".$temperatureName."\">
                         </td>
                       </tr>";
                if($tempAddrArray[0] === "0x28")
                {
                  echo "<tr>
                          <td align=\"center\" colspan=\"3\">
                            Temperature Sensor Address =
                            <select type=\"text\" name=\"tempAddress\">
                              <option selected value=\"".$chipAddress."\">".$chipAddress."</option>
                              ".$tempOptionStr."
                            </select>
                          </td>
                        </tr>";
                }else{
                  echo "<tr>
                          <td align=\"center\" colspan=\"3\">
                            Temperature Sensor Address =
                            <select type=\"text\" name=\"tempAddress\">
                              <option selected value=\"".$dummyAddr."\">".$dummyAddr."
                              ".$tempOptionStr."
                             </select>
                          </td>
                        </tr>";
                }
              }else{
                $tempAddrArray = explode(",", $actionArray[1]);
                if($tempAddrArray[0] === "0x28")
                {
                  mysqli_field_seek($result,0);
                  $finfo = mysqli_fetch_object($result);
                  $temperatureName = $finfo->tempName;
                  //echo "temperatureName=".$temperatureName;
                  echo "<tr>
                          <td align=\"center\" colspan=\"3\">
                            <table width=\"100%\" border=\"0\">
                              <tr>
                                <td>Temperature Sensor Name:</td>
                                  <td>
                                    <input type=\"text\" size=\"25\" name=\"tempName\" value=\"".$temperatureName."\">
                                  </td>
                                </tr>
                                <tr>
                                  <td>Temperature Sensor Address:</td>
                                    <td>
                                      <select type=\"text\" name=\"tempAddress\">
                                        <option selected value=\"".$actionArray[1]."\">".$actionArray[1]."</option>
                                        ".$tempOptionStr."
                                       </select>
                                    </td>
                                  </tr>
                              </table>
                            </td>
                          </tr>";
                }else{
                  echo "<tr>
                          <td align=\"center\" colspan=\"3\">
                            Temperature Sensor Name:
                            <input type=\"text\" size=\"25\" name=\"tempName\" value=\"".$temperatureName."\">
                            <br />
                            Temperature Sensor Address = 
                              <select type=\"text\" name=\"tempAddress\">
                                <option selected value=\"".$dummyAddr."\">".$dummyAddr."</option>
                                ".$tempOptionStr."
                               </select>
                          </td>
                        </tr>";
                }
              }
              $tooColdSwitchName = $finfo->tcName;
              echo "<tr>
                      <td align=\"center\" colspan=\"3\">
                        Too Cold Parameters
                      </td>
                    </tr>";
              echo "<tr>
                      <td align=\"center\">
                        Trigger Temp
                      </td>
                      <td align=\"center\">
                        Name:
                        <input type=\"text\" size=\"25\" name=\"tcName\" value=\"".$tooColdSwitchName."\">
                      </td>
                      <td align=\"center\">
                        Delay (Seconds)
                      </td>
                    </tr>";
              echo "<tr>
                      <td align=\"center\">
                        <input type=\"text\" size=\"10\" name=\"tcTemp\" value=\"".$actionArray[2]."\">
                      </td>";
              echo "<td align=\"center\">
                      Address:
                      <select type=\"text\" name=\"tcSwitchAddr\">
                        <option selected value=\"".$actionArray[3]."\">
                          ".$actionArray[3]."
                         </option>
                          ".$switchOptionStr."
                      </select>
                    </td>";
              echo "<td align=\"center\">
                      <input type=\"text\" size=\"10\" name=\"tcDelay\" value=\"".$actionArray[5]."\">
                    </td>
                  </tr>"; 
              $tooHotSwitchName = $finfo->thName;
              echo "<tr>
                      <td align=\"center\" colspan=\"3\">
                        Too Hot Parameters
                      </td>
                    </tr>";
              echo "<tr>
                      <td align=\"center\">
                        Trigger Temp
                      </td>
                      <td align=\"center\">
                        Name:
                        <input type=\"text\" size=\"25\" name=\"thName\" value=\"".$tooHotSwitchName."\">
                      </td>
                      <td align=\"center\">
                        Delay (Seconds)
                      </td>
                    </tr>";
              echo "<tr>
                      <td align=\"center\">
                        <input type=\"text\" size=\"10\" name=\"thTemp\" value=\"".$actionArray[7]."\">
                        </td>";
              echo "<td align=\"center\">
                      Address:
                      <select type=\"text\" name=\"thSwitchAddr\">
                        <option selected value=\"".$actionArray[8]."\">
                          ".$actionArray[8]."
                        </option>
                        ".$switchOptionStr."
                      </select>
                     </td>";
              echo "<td align=\"center\">
                      <input type=\"text\" size=\"10\" name=\"thDelay\" value=\"".$actionArray[10]."\"
                    </td>
                  </tr>"; 
              echo "<tr>
                      <td align=\"center\" colspan=\"3\">
                        Make your changes and press \"SUBMIT\"
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
 
