<?php
function exec_enabled() {
  $disabled = explode(', ', ini_get('disable_functions'));
  return !in_array('exec', $disabled);
}

$result = exec_enabled();
echo $result."\n";
?>
