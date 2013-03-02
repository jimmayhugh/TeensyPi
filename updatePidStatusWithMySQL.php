<?php

  include_once("makeASocket.php");
  include_once("accessDatabase.php");
  $pidTempName = "";
  $pidSwitchName = "";

  
  $newSocket = makeASocket($service_port, $address);
  $in = $getEEPROMstatus."\n";
  socket_write($newSocket, $in, strlen($in));
  $eepromStatus = socket_read($newSocket, $socBufSize);
  socket_close($newSocket);
  // echo "eepromStatus = ".$eepromStatus."<br />";
 
  $newSocket = makeASocket($service_port, $address);
  $in = $getMaxPids."\n";
  socket_write($newSocket, $in, strlen($in));
  $chipX = socket_read($newSocket, $socBufSize);
  socket_close($newSocket);
  $pidNum = trim($chipX);
//  echo "maxPIDs =".$pidNum."<br />";
  $chipYcount = 0;
  $newSocket = makeASocket($service_port, $address);
  $in = $getPidStatus."\n";
  socket_write($newSocket, $in, strlen($in));
  $chipX = socket_read($newSocket, $socBufSize);
  socket_close($newSocket);
 
  $chipY = explode(";", $chipX);
  $chipYcount = count($chipY);
/*  
  for($x=0;$x<$chipYcount;$x++)
  {
    echo $chipY[$x]."<br />";
  }
*/  
  $bodyStr="<table width=\"100%\" border=\"2\" cellspacing=\"0\" cellpadding=\"5\">
              <tr>
                <td align=\"center\" colspan=\"5\">
                  <h2> PID STATUS </h2>
                </td>
               <tr>
                <td align=\"center\" colspan=\"5\">
                  </form>
                  <font color=\"red\">
                    USE THIS BUTTON ONLY WHEN NECESSARY!!
                    <br />
                    </font>
                  <form method=\"post\" action=\"PidStatus.php\">
                    <input type=\"hidden\" name=\"masterPidStop\" value=\"masterPidStop\">
                    <input type=\"submit\" id=\"masterStop\" value=\"Stop All Pids\">
                  </form>
                  <font color=\"red\">
                    USE THIS BUTTON ONLY WHEN NECESSARY!!
                    <br />
                    </font>
                      ";
/*
  if(trim($eepromStatus) == "FALSE")
  {
    $bodyStr.="<form method=\"post\" action=\"PidStatus.php\">
                   <input type=\"hidden\" name=\"restoreall\" value=\"restoreall\">
                    <input type=\"submit\" value=\"RESTORE ALL\">
                  </form>";
  }
*/
    $bodyStr.="</td>
              </tr>
              <tr>";
  
//  for($x=0;$x<5;$x++)
  for($x=0;$x<$pidNum;$x++)
  {
    
    $chipXArray = explode(",", $chipY[$x]);
    
    $pidEnabledStr    = trim($chipXArray[0]);
    $pidTempStr       = trim($chipXArray[1]);
    $pidSetPointStr   = trim($chipXArray[2]);
    $pidSwitchStr     = trim($chipXArray[3]);
    $pidKpStr         = trim($chipXArray[4]);
    $pidKiStr         = trim($chipXArray[5]);
    $pidKdStr         = trim($chipXArray[6]);
    $pidDirectionStr  = trim($chipXArray[7]);
    $pidWindowSizeStr = trim($chipXArray[8]);
    
    $query = "select * from pid where id=".$x;
    $result = mysqli_query($link,$query);
    if($result != NULL && mysqli_num_rows($result) > 0)
    {
//      echo "selected ".mysqli_num_rows($result)." rows<br />";
      $pidObj = mysqli_fetch_object($result);
      $pidTempAddress = $pidObj->tempAddr;
      $pidSwitchAddress = $pidObj->switchAddr;
//      echo "pid".$x." = ".$pidObJ->tempAddr."&nbsp;&nbsp;".$pidObJ->switchAddr."<br />";
//      echo "pid".$x." = ".$pidTempAddress."&nbsp;&nbsp;".$pidSwitchAddress."<br />";
      mysqli_free_result($result);
    }else{
      echo "Address Query Failed<br />";
    }
    
    $query = "select * from chipNames where address='".$pidTempAddress."'";
    $result = mysqli_query($link,$query);
    if($result != NULL && mysqli_num_rows($result) > 0)
    {
      $pidObj = mysqli_fetch_object($result);
      $pidTempName = $pidObj->name;
      mysqli_free_result($result);
    }
    if($pidTempAddress == ""){$pidTempAddress = "PID".$x." Temp ";}
    
    $query = "select * from chipNames where address='".$pidSwitchAddress."'";
    $result = mysqli_query($link,$query);
    if($result != NULL && mysqli_num_rows($result) > 0)
    {
      $finfo = mysqli_fetch_row($result);
      $pidSwitchName = $finfo[2];
      mysqli_free_result($result);
    }
    if($pidSwitchName == "UNASSIGNED"){$pidSwitchName = "PID".$x." Switch ";}

    $bodyStr.= 
    "<div id=\"pid".$x."\">
      <td valign=\"top\" align=\"center\">
        <table border=\"1\" width=\"100%\" cellspacing=\"0\" cellpadding=\"10\">
          <tr>
            <td align=\"center\" colspan=\"4\">
              <form name=\"pidInfo\" method=\"post\" action=\"PidSetup.php\">
                <input type=\"hidden\" name=\"pidCnt\" value=\"".$x."\">
                <input type=\"submit\" value=\"MODIFY\">
              </form>
           </td>
          <tr>
            <td align=\"center\" colspan=\"4\">";
    if($pidEnabledStr === "0")
    {
      $bodyStr .= 
         "<font color=\"red\"><strong>DISABLED</strong></font>
              <form name=\"pidInfo\" method=\"post\" action=\"PidStatus.php\">
                <input type=\"hidden\" name=\"pidCnt\" value=\"".$x."\">
                <input type=\"hidden\" name=\"pidEnable\" value=\"pidEnable\">
                <input type=\"submit\" value=\"ENABLE\">
              </form>";
     }else if($pidEnabledStr === "1"){
      $bodyStr .= 
        "<font color=\"green\"><strong>ENABLED</strong></font>
              <form name=\"pidInfo\" method=\"post\" action=\"PidStatus.php\">
                <input type=\"hidden\" name=\"pidCnt\" value=\"".$x."\">
                <input type=\"hidden\" name=\"pidDisable\" value=\"pidDisable\">
                <input type=\"submit\" value=\"DISABLE\">
              </form>";
    }else{
      $bodyStr .=
        "<font color=\"yellow\"><strong>UNKNOWN = ".$pidEnabledStr."</strong></font><br />";
    }
    
    if($pidEnabledStr === "1")
    {
      $bodyStr.=
         "<form method=\"post\" action=\"plotPidData.php\">
           <input type=\"hidden\" name=\"pidGraphId\" value=\"".$x."\">
           <input type=\"submit\" value=\"PIDGRAPH\">
         </form>";
    }

    $bodyStr .= 
       "</td>
     </tr>
<!--
     <tr>
       <td align=\"center\" colspan=\"4\">
         <div style=\"vertical-align:middle; min-height:50px;\">
           <font size=\"5\" color=\"blue\"><strong>"
            .$pidTempName."
           </strong></font>
       </td>
     </tr>
-->     
     <tr>
      <td align=\"center\" colspan=\"4\">";
    if($pidTempStr >= -76 && $pidTempStr < 215)
    {
      if($pidTempStr > $pidSetPointStr)
      {
        $fontColor = "red";

      }else if($pidTempStr < $pidSetPointStr){

        $fontColor = "blue";
       
      }else if($pidTempStr === $pidSetPointStr){

        $fontColor = "green";
      
      }
      
      $bodyStr .=
        "<div style=\"vertical-align:middle; min-height:85px; max-height:80px;\">
           <font size=\"5\" color=\"".$fontColor."\"><strong>
           ".$pidTempName."
           </strong></font><br />
          <font size=\"10\" color=\"".$fontColor."\"><strong>".$pidTempStr."&deg;</strong></font>
         </div>";
    }else{
      $bodyStr .=
        "<div style=\"vertical-align:middle; min-height:85px;\">
          <font color=\"red\" size=\"5\"><strong>UNASSIGNED</strong></font>
         </div>";
    }
    $bodyStr .= 
       "</td>
     </tr>
     <tr>
      <td align=\"center\" colspan=\"4\">
        <div style=\"vertical-align:middle; min-height:50px;\">
          <font size=\"5\" color=\"blue\"><strong>
            Set Point<br />".$pidSetPointStr."
          </strong></font>
        </div>
      </td>
     </tr>
     <tr>
      <td align=\"center\" colspan=\"4\">";
    if($pidSwitchStr === "N")
    {
        $bodyStr .=
           "<div style=\"vertical-align:middle; min-height:85px; max-height:85px;\">
              <font size=\"5\" color=\"blue\"><strong>
                ".$pidSwitchName."
              </strong></font><br />              
              <font size=\"10\" color=\"green\"><strong>ON</strong></font>
            </div>";
    }else if($pidSwitchStr === "F"){
        $bodyStr .= 
           "<div style=\"vertical-align:middle; min-height:85px; max-height:85px;\">
              <font size=\"5\" color=\"blue\"><strong>
                ".$pidSwitchName."
              </strong></font><br />              
              <font size=\"10\" color=\"red\"><strong>OFF</strong></font>
            </div>";
    }else{
        $bodyStr .= "
            <div style=\"vertical-align:middle; min-height:85px; max-height:90px;\">
              <font size=\"5\" color=\"red\"><strong>UNASSIGNED</strong></font>
            </div>";
    }
    $bodyStr .= "
      </td>
    </tr>
    <tr>
      <td align=\"center\" colspan=\"4\">
        <div style=\"vertical-align:middle; min-height:50px;\">
          <font size=\"5\" color=\"blue\"><strong>PID Variables<br />
          Kp:&nbsp;".$pidKpStr."<br />Ki:&nbsp;".$pidKiStr."<br />Kd:&nbsp;".$pidKdStr."
          </strong></font>
        </div>
    ";
    $bodyStr .="
      </td>
    </tr>
    <tr>
      <td align=\"center\" colspan=\"4\">
        <div style=\"vertical-align:middle; min-height:50px;\">
          <font size=\"5\" color=\"blue\"><strong>Direction<br /></strong></font>";
          if($pidDirectionStr === "0")
          {
            $bodyStr .="<font size=\"5\" color=\"red\"><strong>Forward</strong></font>";
          }else{
            $bodyStr .="<font size=\"5\" color=\"blue\"><strong>Reverse</strong></font>";
          }
    $bodyStr .="</div>";
    $bodyStr .= "
      </td>
    </tr>
    <tr>
      <td align=\"center\" colspan=\"4\">
        <div style=\"vertical-align:middle; min-height:50px;\">
          <font size=\"5\" color=\"blue\"><strong>PID Window Size<br />".$pidWindowSizeStr."</strong></font>
        </div>";
    $bodyStr .= "</td></tr>\n";
    $bodyStr .= "</table></td></div>\n";
    if($x === 3 || $x === 7) {  $bodyStr .= "</tr><tr>\n";}
  }
  $bodyStr .= "</tr>\n</table>";

  echo $bodyStr;
  mysqli_close($link);

?>
