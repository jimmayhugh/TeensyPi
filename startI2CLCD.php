<?php

function startI2CLCD($match)
{
  exec("/var/www/htdocs/$match > /dev/null 2>/dev/null &");
}

?>
