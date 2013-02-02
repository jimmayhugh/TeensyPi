<?php
  include_once('/var/www/htdocs/PHP_GnuPlot.php'); 
  include_once("/var/www/htdocs/accessDatabase.php");
  $hotTest = "";
  $coldTest = "";
  $pTemp = new GNUPlot();
  $pTemp->setDimLabel(x, "Hours");
  $pTemp->setDimLabel(y, "Degrees");
  $pTemp->set("terminal png size 1280,400"); 
  $id = $_POST["graphId"];
  $query = "SELECT time,temp,tcTemp,tcSwitch,thTemp,thSwitch FROM actionGraph WHERE id='".$_POST["graphId"]."'";
//  echo $query."\n";
  $result = mysqli_query($link, $query);
  $rowCnt = mysqli_num_rows($result);

// set line colors
  $pTemp->set("style line 1 linewidth 2 linecolor rgb \"#000\""); 
  $pTemp->set("style line 2 linecolor rgb \"red\""); 
  $pTemp->set("style line 3 linecolor rgb \"blue\""); 

// assign line values
  $tempData = new PGData('Temperature');
  $thData   = new PGData('Too Hot Value');
  $tcData   = new PGData('Too Cold Value');

  $oldTime = NULL; 
  while ($obj = mysqli_fetch_object($result))
  {
    if($hotTest ==="")
    {
      $hotTest = $obj->temp;
    }else if($obj->temp > $hotTest){
      $hotTest = $obj->temp;
    } 
    if($coldTest ==="")
    {
      $coldTest = $obj->temp;
    }else if($obj->temp < $coldTest){
      $coldTest = $obj->temp;
    } 
    if($obj->thSwitch === "ON")
    {
      $thVal = 1;
    }else if($obj->thSwitch === "OFF"){
      $thVal = -1;
    }else{
      $thVal = 0;
    }
    if($obj->tcSwitch === "ON")
    {
      $tcVal = 1;
    }else if($obj->thSwitch === "OFF"){
      $tcVal = -1;
    }else{
      $tcVal = 0;
    }
    if($oldTime != NULL)
    {
      $secCnt = ($obj->time) - $oldTime;
      $tempData->addDataEntry(array($obj->temp, $secCnt ));
      $thData->addDataEntry(array($obj->thTemp, $secCnt ));
      $tcData->addDataEntry(array($obj->tcTemp, $secCnt ));
    }else{
      $tempData->addDataEntry(array($obj->temp, 0));
      $thData->addDataEntry(array($obj->thTemp, 0));
      $tcData->addDataEntry(array($obj->tcTemp, 0));
      $oldTime = $obj->time;
    }
    if($coldTest > $obj->tcTemp)
    {
      $coldTest = $obj->tcTemp;
    }
    if($hotTest < $obj->thTemp)
    {
      $hotTest = $obj->thTemp;
    }
  }
//  echo "Select returned ".$rowCnt." rows.\n";
  mysqli_close($link);
  $tempTitle = "Action ".$id." Temperature Graph";
  $pTemp->setTitle($tempTitle); 
  $pTemp->plotData( $tempData, 'lines', '($2/3600):1','','ls 1'); 
  $pTemp->plotData( $thData, 'lines', '($2/3600):1','','ls 2'); 
  $pTemp->plotData( $tcData, 'lines', '($2/3600):1','','ls 3'); 
  $pTemp->setRange('y', $coldTest-1, $hotTest+1); 
  $pTemp->export('/var/www/htdocs/pTemp.png');
  $pTemp->close(); 
?>
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
        <td valign="top" align="center" width="100%">
          <h2>Action #<?php echo $_POST["graphId"]; ?> Graph</h2>
        </td>
      </tr>
      <tr>
        <td>
          <img src="pTemp.png">
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
  </body>
</html>
