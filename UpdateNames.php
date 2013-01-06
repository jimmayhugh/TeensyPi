<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
    <title> Sensor /Switch Setup </title>
    <link rel="stylesheet" type="text/css" href="style.css"/>
    <script type="text/javascript" src="//ajax.googleapis.com/ajax/libs/jquery/1.8.2/jquery.min.js">
    </script>
    <style>
      input[type='text'] { font-size: 18px; text-align: center;}
      input:focus, textarea:focus{background-color: lightgrey;}
      select[type='text'] { font-size: 18px; text-align: center;}
    </style>
  </head>
  <body>
    <?php 
      include_once("makeASocket.php");
      include_once("accessDatabase.php");
      include_once("header.html");
      $h2Header = "<font color=\"blue\">Update Names</font>"; 
    ?> 
<!-- Table for Main Body -->
    <table width="100%" border="0" cellspacing="0" cellpadding="2">
      <tr>
        <td>
          <?php 
            include ("menu.html");
            echo "<br />";
            if($_POST["update"] === "update")
            {
              $h2Header="<font color=\"red\">Names Updated</font>";
              for($updateCnt=0; $updateCnt<$_POST["maxChipCnt"]; $updateCnt++)
              {
                $chipAddress = $_POST["address$updateCnt"];
                $result = mysqli_query($link,$query);
                $escapedName = mysqli_real_escape_string ($link , $_POST["name$updateCnt"]);
                $query = "update chipNames set address='".$chipAddress."', name='".$escapedName."' where id='".$updateCnt."'";
                $result = mysqli_query($link,$query);
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
            }
            echo "
              </td>
            </tr>
            <tr>
              <td align=\"center\" colspan=\"2\">
                <h2>".$h2Header."</h2>
              </td>
              </tr>
              <tr>
                <td>";
            
            $newSocket = makeASocket($service_port, $address);
            $in = "$getChipCount\n";
            socket_write($newSocket, $in, strlen($in));
            $out = socket_read($newSocket, $socBufSize);
            socket_close($newSocket);
            $maxChipCnt = trim($out);
            echo "<br />
                  <form method=\"post\" action=\"UpdateNames.php\">
                    <input type=\"hidden\" name=\"update\" value=\"update\">
                    <input type=\"hidden\" name=\"maxChipCnt\" value=\"".$maxChipCnt."\">
                    <table border=\"2\" cellpadding=\"2\" cellspacing=\"2\">
                    <tr>
                      <td align=\"center\" colspan=\"2\"><font size=\"5\"><strong>There are ".$out."
                        arrays that have a chip installed.</strong></font></td>
                    </tr>
                    <tr>
                      <td colspan=\"2\"align=\"center\">
                        <font size=\"5\"><strong>
                          Make your changes and press \"SUBMIT\"
                        </strong></font> 
                        <br /> 
                        <input type=\"submit\" value=\"SUBMIT\">
                      </td>
                    </tr>
                    <tr>
                      <td align=\"center\"><font size=\"5\"><strong>Address</strong></font></td>
                      <td align=\"center\"><font size=\"5\"><strong>Name</strong></font></td>
                    </tr>";
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
                $query = "select name from chipNames where address='".$chipArray[0]."'";
                $result = mysqli_query($link,$query);
                if(mysqli_num_rows($result) > 0)
                {
                  $finfo = mysqli_fetch_object($result);
                  if($finfo->name != "UNASSIGNED")
                  {
                    $chipName = $finfo->name;
                  }else{
                    $chipname = "";
                  }
                }else{
                  $chipName = "";
                }
                echo "  <tr>
                          <td align=\"center\">
                            ".$chipArray[0]."
                            <input type=\"hidden\" name=\"address".$scCnt."\" value=\"".$chipArray[0]."\">
                          </td>
                          <td align=\"center\">
                            <input type=\"text\" size=\"25\" name=\"name".$scCnt."\" value=\"".$chipName."\">
                          </td>
                        </tr>";
               }
             echo "     <tr>
                          <td colspan=\"2\"align=\"center\">
                            <font size=\"5\"><strong>
                              Make your changes and press \"SUBMIT\" 
                            </strong></font> 
                            <br /> 
                            <input type=\"submit\" value=\"SUBMIT\">
                          </td>
                        </tr>
                      </table>
                    </form";
           include ("menu.html");
           mysqli_close($link);
          ?>
        </td>
      </tr>
    </table>
  </body>
</html>

