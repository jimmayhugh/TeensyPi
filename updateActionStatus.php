<?php

  include_once("makeASocket.php");
 
  $newSocket = makeASocket($service_port, $address);
  $in = $getMaxActions."\n";
  socket_write($newSocket, $in, strlen($in));
  $chipX = socket_read($newSocket, $socBufSize);
  socket_close($newSocket);
  $actionNum = trim($chipX);

  $newSocket = makeASocket($service_port, $address);
  $in = $getActionStatus."\n";
  socket_write($newSocket, $in, strlen($in));
  $chipX = socket_read($newSocket, $socBufSize);
  socket_close($newSocket);
 
  $chipY = explode(";", $chipX);
  
  $bodyStr="<table width=\"100%\" border=\"2\" cellspacing=\"0\" cellpadding=\"5\">\n<tr>\n<td align=\"center\" colspan=\"5\">\n<h2>Sensor / Action Status</h2>\n</td>\n</tr>\n<tr>\n";
  
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
    $bodyStr.= "<div id=\"action".$x."\"><td align=\"center\">\n<table border=\"1\" width=\"25%\" cellspacing=\"0\" cellpadding=\"10\">\n";
      $bodyStr .= "<tr><td align=\"center\" colspan=\"4\">";
    if($trimStr === "0")
    {
        $bodyStr .= "<font color=\"red\"><strong>DISABLED</strong></font><br /><form method=\"post\" action=\"ActionData.php\"><input type=\"hidden\" name=\"actionCnt\" value=\"".$x."\"><input type=\"submit\" value=\"ASSIGN\"></form>";
    }else if($trimStr === "1"){
        $bodyStr .= "<font color=\"green\"><strong>ENABLED</strong></font><br /><form method=\"post\" action=\"ActionData.php\"><input type=\"hidden\" name=\"actionCnt\" value=\"".$x."\"><input type=\"submit\" value=\"ASSIGN\"></form>";
    }else{
        $bodyStr .= "<font color=\"yellow\"><strong>UNKNOWN = ".$trimStr."</strong></font><br /><form method=\"post\" action=\"ActionData.php\"><input type=\"hidden\" name=\"actionCnt\" value=\"".$x."\"><input type=\"submit\" value=\"ASSIGN\"></form>";
    }
      $bodyStr .= "</td></tr>\n";
      $bodyStr .= "<tr><td align=\"center\" colspan=\"4\">\n";
    if($trimStr1 >= -76 && $trimStr1 < 215)
    {
        $bodyStr .= "<div style=\"vertical-align:middle; min-height:50px;\"><font size=\"10\"><strong>".$trimStr1."&deg;</strong></font></div>";
    }else{
        $bodyStr .= "<div style=\"vertical-align:middle; min-height:50px;\"><font color=\"red\" size=\"5\"><strong>UNUSED</strong></font></div>";
    }
      $bodyStr .= "</td></tr>\n";
      $bodyStr .= "<tr>";
    if($trimStr2 === "N")
    {
        $bodyStr .= "
          <td align=\"center\" style=\"text-align:center; vertical-align:middle; min-width:100px\">
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
          </style>
          Too Cold<br />".$trimStr4."&deg;<br \>Switch ".$x."
          <font size=\"10\" color=\"green\"><strong>ON</strong></font>
          <br />
          <div id=\"ajaxtc".$x."on\" style=\"text-align:center; vertical-align:middle; min-width:100px\">
            <span>ON</span>
          </div>
          <div id=\"ajaxtc".$x."off\" style=\"text-align:center; vertical-align:middle; min-width:100px\">
            <span>OFF</span>
          </div>
          </td>
          </div>";
    }else if($trimStr2 === "F"){
        $bodyStr .= "
        <td align=\"center\" style=\"text-align:center; vertical-align:middle; min-width:100px\">
          <div id=\"tcswitch".$x."\">\n<script>\njQuery(document).ready(function(){
          jQuery('#ajaxtc".$x."off').click(function(event){
            jQuery.ajax({
              type: \"GET\",
              url: \"setActionSwitch.php\",
              data: 'data=".$x.$tooCold."F',
              cache: false
              });
          });
        });
        
        jQuery(document).ready(function(){
          jQuery('#ajaxtc".$x."on').click(function(event){
            jQuery.ajax({
              type: \"GET\",
              url: \"setActionSwitch.php\",
              data: 'data=".$x.$tooCold."N',
              cache: false
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
        </style>
          Too Cold<br />".$trimStr4."&deg;<br \>Switch ".$x."
          <font size=\"10\" color=\"red\"><strong>OFF</strong></font>
          <br />
          <div id=\"ajaxtc".$x."on\" style=\"text-align:center; vertical-align:middle; min-width:100px\">
            <span>ON</span>
          </div>
          <div id=\"ajaxtc".$x."off\" style=\"text-align:center; vertical-align:middle; min-width:100px\">
            <span>OFF</span>
          </div>
        </div>";
    }else{
        $bodyStr .= "
          <td align=\"center\" style=\"text-align:center; vertical-align:middle; min-width:100px\">
            <div>
              <font color=\"red\"><strong>UNASSIGNED</stront</font>
            </div>
          </td>";
    }
      $bodyStr .= "</td>";
    if($trimStr3 === "N")
    {
      $bodyStr .= "
        <td align=\"center\" style=\"text-align:center; vertical-align:middle; min-width:100px\">
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
        </style>
          Too Hot<br />".$trimStr5."&deg;<br \>Switch ".$x."
          <font size=\"10\" color=\"green\"><strong>ON</strong></font>
          <br />
          <div id=\"ajaxth".$x."on\" style=\"text-align:center; vertical-align:middle; min-width:100px\">
            <span>ON</span>
          </div>
          <div id=\"ajaxth".$x."off\" style=\"text-align:center; vertical-align:middle; min-width:100px\">
            <span>OFF</span>
          </div>
        </td>
      </div>";
    }else if($trimStr3 === "F"){
      $bodyStr .= "
      <td align=\"center\" style=\"text-align:center; vertical-align:middle; min-width:100px\">
      <div id=\"thswitch".$x."\">
        <script>
          jQuery(document).ready(function(){
            jQuery('#ajaxth".$x."off').click(function(event){
              jQuery.ajax({
                type: \"GET\",
                url: \"setActionSwitch.php\",
                data: 'data=".$x.$tooHot."F',
                cache: false
              });
            });
          });

          jQuery(document).ready(function(){
            jQuery('#ajaxth".$x."on').click(function(event){
              jQuery.ajax({
                type: \"GET\",
                url: \"setActionSwitch.php\",
                data: 'data=".$x.$tooHot."N',
                cache: false
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
      </style>
        Too Hot<br />".$trimStr5."&deg;<br \> Switch ".$x."
        <font size=\"10\" color=\"red\"><strong>OFF</strong></font>
        <br />
          <div id=\"ajaxth".$x."on\" style=\"text-align:center; vertical-align:middle; min-width:100px\">
            <span>ON</span>
          </div>
          <div id=\"ajaxth".$x."off\" style=\"text-align:center; vertical-align:middle; min-width:100px\">
            <span>OFF</span>
          </div>
</td>
</div>";
    }else{
        $bodyStr .= "
          <td align=\"center\" style=\"text-align:center; vertical-align:middle; min-width:100px\">
            <div>
              <font color=\"red\"><strong>UNASSIGNED</stront</font>
            </div>
          </td>";
    }
      $bodyStr .= "</td></tr>\n";
      $bodyStr .= "</table></td></div>\n";
    if($x === 3 || $x === 7){  $bodyStr .= "</tr><tr>\n";}
  }
  $bodyStr .= "</tr>\n</table>";

  echo $bodyStr;
?>
