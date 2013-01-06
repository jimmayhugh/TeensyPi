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
    <table width="100%" border="0" cellspacing="0" cellpadding="2">
      <tr>
        <td>
          <?php 
            include ("menu.html");
          ?>
        </td>
      </tr>
      <tr>
        <td  align="center" colspan="6">
          <table width="100%" border="1" cellspacing="0" cellpadding="2">
            <tr>
              <td align="center" border="2" colspan="6">
                <h2>Sensor / Switch Setup</h2>
              </td>
            </tr>
            <tr>
              <td>
                <table width="100%" border="1" cellspacing="0" cellpadding="2">
                  <tr>
                    <td align="center" border="2" width="50%">
                      <font color="red">
                        USE THIS BUTTON WITH EXTREME CARE!!
                        <br />
                      </font>
                      <form method="post" action="index.php">
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
                    <td align="center" border="2">
                      <form method="post" action="ActionStatus.php">
                        <input type="hidden" name="restoreall" value="restoreall">
                        <input type="submit" value="RESTORE ALL">
                      </form>
                    </td>
                  </tr>
                </table>
              </td>
            </tr>
          </table>      
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
