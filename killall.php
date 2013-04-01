<?php

function killall($match) {
    if($match=='') return 'no pattern specified';
    $match = escapeshellarg($match);
    exec("ps x|grep $match|grep -v grep|awk '{print $1}'", $output, $ret);
    if($ret) return 'you need ps, grep, and awk installed for this to work';
    while(list(,$t) = each($output)) {
        if(preg_match('/^([0-9]+)/', $t, $r)) {
            system('kill '. $r[1], $k);
            if(!$k) $killed = 1;
        }
    }
    if($killed) {
        return '';
    } else {
        return "$match: no process killed";
    }
}

?>
