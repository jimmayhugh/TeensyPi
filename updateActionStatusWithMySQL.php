<?php

  include_once("makeASocket.php");
  include_once("accessDatabase.php");
  $temperatureName = "";
  $thName = "";
  $tcName = "";

  if($_POST["masterStop"] === "masterStop")
  {
    $newSocket = makeASocket($service_port, $address);
    $in = $masterStop."\n";
    socket_write($newSocket, $in, strlen($in));
    $eepromStatus = socket_read($newSocket, $socBufSize);
    socket_close($newSocket);
  }
  
  $newSocket = makeASocket($service_port, $address);
  $in = $getEEPROMstatus."\n";
  socket_write($newSocket, $in, strlen($in));
  $eepromStatus = socket_read($newSocket, $socBufSize);
  socket_close($newSocket);
  // echo "eepromStatus = ".$eepromStatus."<br />";
 
  $newSocket = makeASocket($service_port, $address);
  $in = $getMaxActions."\n";
  socket_write($newSocket, $in, strlen($in));
  $chipX = socket_read($newSocket, $socBufSize);
  socket_close($newSocket);
  $actionNum = trim($chipX);
  
  $chipYcount = 0;
  do
  {
  $newSocket = makeASocket($service_port, $address);
  $in = $getActionStatus."\n";
  socket_write($newSocket, $in, strlen($in));
  $chipX = socket_read($newSocket, $socBufSize);
  socket_close($newSocket);
 
  $chipY = explode(";", $chipX);
  $chipYcount = count($chipY);
//  echo $chipX."<br />".$chipYcount."   ".$actionNum;
  }while($chipYcount < $actionNum);

  $bodyStr="<table width=\"100%\" border=\"2\" cellspacing=\"0\" cellpadding=\"5\">
              <tr>
                <td align=\"center\" colspan=\"5\">
                  <h2>Sensor / Action Status</h2>
                </td>
               <tr>
                <td align=\"center\" colspan=\"5\">
                  </form>
                  <font color=\"red\">
                    USE THIS BUTTON ONLY WHEN NECESSARY!!
                    <br />
                    </font>
                  <form method=\"post\" action=\"ActionStatus.php\">
                    <input type=\"hidden\" name=\"masterStop\" value=\"masterStop\">
                    <input type=\"submit\" id=\"masterStop\" value=\"Stop All Actions\">
                  </form>
                  <font color=\"red\">
                    USE THIS BUTTON ONLY WHEN NECESSARY!!
                    <br />
                    </font>
                      ";

  if(trim($eepromStatus) == "FALSE")
  {
    $bodyStr.="<form method=\"post\" action=\"ActionDataWithMySQL.php\">
                   <input type=\"hidden\" name=\"restoreall\" value=\"restoreall\">
                    <input type=\"submit\" value=\"RESTORE ALL\">
                  </form>";
  }
    $bodyStr.="</td>
              </tr>
              <tr>";
  
//  for($x=0;$x<5;$x++)
  for($x=0;$x<$actionNum;$x++)
  {
    
    $chipXArray = explode(",", $chipY[$x]);
    
    $trimStr = trim($chipXArray[0]);
    $trimStr1 = trim($chipXArray[1]);
    $trimStr2 = trim($chipXArray[2]);
    $trimStr3 = trim($chipXArray[3]);
    $trimStr4 = trim($chipXArray[4]);
    $trimStr5 = trim($chipXArray[5]);
    
    $query = "select * from action where id=".$x;
    $result = mysqli_query($link,$query);
    if($result != NULL && mysqli_num_rows($result) > 0)
    {
      $finfo = mysqli_fetch_row($result);
      $temperatureAddress = $finfo[2];
      $tooHotAddress = $finfo[6];
      $tooColdAddress = $finfo[3];
//      $bodyStr .= "<br />action = ".$x."&nbsp;&nbsp;&nbsp;".$temperatureAddress."&nbsp;&nbsp;&nbsp;".$tooHotAddress."&nbsp;&nbsp;&nbsp;".$tooColdAddress."<br />";
      mysqli_free_result($result);
    }
    
    $query = "select * from chipNames where address='".$temperatureAddress."'";
    $result = mysqli_query($link,$query);
    if($result != NULL && mysqli_num_rows($result) > 0)
    {
      $finfo = mysqli_fetch_row($result);
      $temperatureName = $finfo[2];
      mysqli_free_result($result);
    }
    if($temperatureName == ""){$temperatureName = "Temp Sensor ".$x;}
    
    $query = "select * from chipNames where address='".$tooHotAddress."'";
    $result = mysqli_query($link,$query);
    if($result != NULL && mysqli_num_rows($result) > 0)
    {
      $finfo = mysqli_fetch_row($result);
      $thName = $finfo[2];
      mysqli_free_result($result);
    }
    if($thName == ""){$thName = "Hot Switch ".$x;}

    $query = "select * from chipNames where address='".$tooColdAddress."'";
    $result = mysqli_query($link,$query);
    if($result != NULL && mysqli_num_rows($result) > 0)
    {
      $finfo = mysqli_fetch_row($result);
      $tcName = $finfo[2];
      mysqli_free_result($result);
    }
    if($tcName == ""){$tcName = "Cold Switch ".$x;}

    $bodyStr.= 
    "<div id=\"action".$x."\">
      <td valign=\"top\" align=\"center\">
        <table border=\"1\" width=\"25%\" cellspacing=\"0\" cellpadding=\"10\">
          <tr>
            <td align=\"center\" colspan=\"4\">";
    if($trimStr === "0")
    {
      $bodyStr .= 
        "<font color=\"red\"><strong>DISABLED</strong></font><br />";
    }else if($trimStr === "1"){
      $bodyStr .= 
        "<font color=\"green\"><strong>ENABLED</strong></font><br />";
    }else{
      $bodyStr .=
        "<font color=\"yellow\"><strong>UNKNOWN = ".$trimStr."</strong></font><br />";
    }
    $bodyStr .= 
        "<form method=\"post\" action=\"ActionDataWithMySQL.php\">
           <input type=\"hidden\" name=\"actionCnt\" value=\"".$x."\">
           <input type=\"submit\" value=\"ASSIGN\">
         </form>";
    $bodyStr .= 
       "</td>
     </tr>
     <tr>
      <td align=\"center\" colspan=\"4\">";
    if($trimStr1 >= -76 && $trimStr1 < 215)
    {
      $bodyStr .=
        "<div style=\"vertical-align:middle; min-height:50px;\">".$temperatureName."<br />
          <font size=\"10\"><strong>".$trimStr1."&deg;</strong></font>
         </div>";
    }else{
      $bodyStr .=
        "<div style=\"vertical-align:middle; min-height:50px;\">
          <font color=\"red\" size=\"5\"><strong>UNUSED</strong></font>
         </div>";
    }
    $bodyStr .= 
      "</td>
      </tr>
      <tr>
        <td align=\"center\" style=\"text-align:center; vertical-align:middle; min-width:150px\">
          <div id=\"tcswitch".$x."\">
            <script>
              jQuery(document).ready(function(){
                jQuery('#ajaxtc".$x."off').click(function(event){
                  jQuery.ajax({
                    type: \"GET\",
                    url: \"setActionSwitch.php\",
                    data: 'data=".$x.$tooCold."F',
                    cache: false,
                    });
                });
              });
              
              jQuery(document).ready(function(){
                jQuery('#ajaxtc".$x."on').click(function(event){
                  jQuery.ajax({
                    type: \"GET\",
                    url: \"setActionSwitch.php\",
                    data: 'data=".$x.$tooCold."N',
                    cache: false,
                    });
                });
              });
              
          </script>
          <style>
          #ajax{cursor:pointer;}
          #ajaxtc".$x."on{margin-bottom:20px;}
          #ajaxtc".$x."on span{display:block;min-width:75px;padding:3px;background-color:#00FF00;color:#000;}
          #ajaxtc".$x."on span:hover{display:block;min-width:75px;padding:3px;background-color:#000;color:#FFFF00;}
          #ajaxtc".$x."off{margin-bottom:20px;}
          #ajaxtc".$x."off span{display:block;min-width:75px;padding:3px;margin-bottom:20px;background-color:#FF0000;color:#000;}
          #ajaxtc".$x."off span:hover{display:block;min-width:75px;padding:3px;background-color:#000;color:#FFFF00;}
          </style>";
    if($trimStr2 === "N")
    {
        $bodyStr .= "
          Too Cold<br />".$trimStr4."&deg;<br \>".$tcName."<br />
          <font size=\"10\" color=\"green\"><strong>ON</strong></font>
          <br />
          <div id=\"ajaxtc".$x."on\" style=\"text-align:center; vertical-align:middle; min-width:150px\">
            <span>ON</span>
          </div>
          <div id=\"ajaxtc".$x."off\" style=\"text-align:center; vertical-align:middle; min-width:150px\">
            <span>OFF</span>
          </div>
          </td>
          </div>";
    }else if($trimStr2 === "F"){
        $bodyStr .= "
          Too Cold<br />".$trimStr4."&deg;<br \>".$tcName."<br />
          <font size=\"10\" color=\"red\"><strong>OFF</strong></font>
          <br />
          <div id=\"ajaxtc".$x."on\" style=\"text-align:center; vertical-align:middle; min-width:150px\">
            <span>ON</span>
          </div>
          <div id=\"ajaxtc".$x."off\" style=\"text-align:center; vertical-align:middle; min-width:150px\">
            <span>OFF</span>
          </div>
        </div>";
    }else{
        $bodyStr .= "
            <div>
              <font color=\"red\"><strong>UNASSIGNED</stront</font>
            </div>";
    }
      $bodyStr .= 
        "</td>
        <td align=\"center\" style=\"text-align:center; vertical-align:middle; min-width:150px\">
        <div id=\"thswitch".$x."\">
          <script>
            jQuery(document).ready(function(){
              jQuery('#ajaxth".$x."off').click(function(event){
               jQuery.ajax({
                 type: \"GET\",
                 url: \"setActionSwitch.php\",
                 data: 'data=".$x.$tooHot."F',
                 cache: false,
                });
              });
            });

          jQuery(document).ready(function(){
            jQuery('#ajaxth".$x."on').click(function(event){
              jQuery.ajax({
                type: \"GET\",
                url: \"setActionSwitch.php\",
                data: 'data=".$x.$tooHot."N',
                cache: false,
                });
            });
          });
        </script>
        <style>
          #ajax{cursor:pointer;}
          #ajaxth".$x."on{margin-bottom:20px;}
          #ajaxth".$x."on span{display:block;min-width:75px;padding:3px;background-color:#00FF00;color:#000;}
          #ajaxth".$x."on span:hover{display:block;min-width:75px;padding:3px;background-color:#000;color:#FFFF00;}
          #ajaxth".$x."off{margin-bottom:20px;}
          #ajaxth".$x."off span{display:block;min-width:75px;padding:3px;margin-bottom:20px;background-color:#FF0000;color:#000;}
          #ajaxth".$x."off span:hover{display:block;min-width:75px;padding:3px;background-color:#000;color:#FFFF00;}
        </style>";
    if($trimStr3 === "N")
    {
      $bodyStr .= "
          Too Hot<br />".$trimStr5."&deg;<br \>".$thName."<br />
          <font size=\"10\" color=\"green\"><strong>ON</strong></font>
          <br />
          <div id=\"ajaxth".$x."on\" style=\"text-align:center; vertical-align:middle; min-width:150px\">
            <span>ON</span>
          </div>
          <div id=\"ajaxth".$x."off\" style=\"text-align:center; vertical-align:middle; min-width:150px\">
            <span>OFF</span>
          </div>
        </td>
      </div>";
    }else if($trimStr3 === "F"){
      $bodyStr .= "
        Too Hot<br />".$trimStr5."&deg;<br \>".$thName."<br />
        <font size=\"10\" color=\"red\"><strong>OFF</strong></font>
        <br />
          <div id=\"ajaxth".$x."on\" style=\"text-align:center; vertical-align:middle; min-width:150px\">
            <span>ON</span>
          </div>
          <div id=\"ajaxth".$x."off\" style=\"text-align:center; vertical-align:middle; min-width:150px\">
            <span>OFF</span>
          </div>
</td>
</div>";
    }else{
        $bodyStr .= "
          <div>
            <font color=\"red\"><strong>UNASSIGNED</stront</font>
          </div>";
    }
      $bodyStr .= "</td></tr>\n";
      $bodyStr .= "</table></td></div>\n";
    if($x === 2 || $x === 5 || $x === 8){  $bodyStr .= "</tr><tr>\n";}
  }
  $bodyStr .= "</tr>\n</table>";

  echo $bodyStr;
  mysqli_close($link);

?>
