<?php

  include_once("makeASocket.php");
  include_once("accessDatabase.php");
  $tempStr="";
  $switchStr="";
  $unusedStr="";
  $switchjStr="";
  $maxTempPerLine = 6;


  $newSocket = makeASocket($service_port, $address);
  $in = $getAllStatus."\n";
  socket_write($newSocket, $in, strlen($in));
  $chipX = socket_read($newSocket, $socBufSize);
  socket_close($newSocket);

  // echo $chipX;
  $status=explode(",", $chipX);
  $count = count($status);

  
  for($x=0,$y=0,$z=0;$x<$count;$x++)
  {   
    $query = "select * from chipNames where id='".$x."'";
    $result = mysqli_query($link, $query);
    $row = mysqli_fetch_row($result);
    mysqli_free_result($result);
    if(is_numeric($status[$x]) === TRUE)
    {
      $tempStr .=
      "<td align=\"center\">
        Temp ".$x."
        <br /><br />";
      $tempStr .= $row[2];
      $tempStr .=
      "<br /><br />
        <font size=\"10\">
          <strong>
            ".$status[$x]."&deg;
          </strong>
        </font>
        <br /><br />
       </td>";
       $y++;
       if($y == $maxTempPerLine)
       {
         $tempStr .= "
          </tr><tr>";
         $y = 0;
       }
    }else if((($trimStr = trim($status[$x])) === "N") || (($trimStr = trim($status[$x])) === "F") ){
      $switchStr .= 
      "<div id=\"switch".$x."\">
        <script>
          jQuery(document).ready(function(){
            jQuery('#ajax".$x."off').click(function(event){
              jQuery.ajax({
                type: \"GET\",
                url: \"setSwitch.php\",
                data: 'data=".$x."F',
                cache: false,
              });
            });
          });

          jQuery(document).ready(function(){
            jQuery('#ajax".$x."on').click(function(event){
              jQuery.ajax({
                type: \"GET\",
                url: \"setSwitch.php\",
                data: 'data=".$x."N',
                cache: false,
              });
            });
          });
</script>
  <style>
    #ajax{cursor:pointer;}
    #ajax".$x."on{margin-bottom:20px;}
    #ajax".$x."on span{display:block;max-width:150px;padding:3px;background-color:#00FF00;color:#000;}
    #ajax".$x."on span:hover{padding:3px;background-color:#000;color:#FFFF00;}
    #ajax".$x."off{margin-bottom:20px;}
    #ajax".$x."off span{display:block;max-width:150px;padding:3px;margin-bottom:20px;background-color:#FF0000;color:#000;}
    #ajax".$x."off span:hover{padding:3px;background-color:#000;color:#FFFF00;}
  </style>";
    if($trimStr === "N"){
      $switchStr .=
      "<td align=\"center\">
        Switch ".$x."
        <br /><br />";
      $switchStr .= $row[2];
      $switchStr .=
      "<br /><br />
      <font size=\"10\" color=\"green\">
          <strong>
            ON
          </strong>
        </font>";
    }else if($trimStr === "F"){
      $switchStr .=
      "<td align=\"center\">
        Switch ".$x."
        <br /><br />";
      $switchStr .= $row[2];
      $switchStr .=
        "<br /><br />
        <font size=\"10\" color=\"red\">
          <strong>
            OFF
          </strong>
        </font>";
    }
    $switchStr .= 
      "<br /><br />
      <div id=\"ajax".$x."on\"><span>ON</span></div>
      <div id=\"ajax".$x."off\"><span>OFF</span></div>
      </td>";
    $switchStr .=
      "</div>"
      ;
    $z++;
    if($z == $maxTempPerLine)
    {
      $switchStr .= "
        </tr><tr>";
      $z = 0;
    }
   }
  }

  echo "<table width=\"100%\" align=\"center\" border=\"2\">\n<tr>\n$tempStr</tr>\n</table>\n\n<table width=\"100%\" align=\"center\" border=\"2\">\n<tr>\n$switchStr</tr>\n</table>\n\n<!-- <table width=\"100%\" align=\"center\" border=\"2\">\n<tr>\n$unusedStr</tr>\n</table> -->";
  mysqli_close($link);

?>
