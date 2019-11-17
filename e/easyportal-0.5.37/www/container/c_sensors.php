<?php
// ################################## Container Sensors ###################################################

# *** Temperaturen ***
$sensors = shell_exec("$SH_EASYVDR_HW --php --sensors");
$sensors = str_replace(" C", " °C", $sensors);

// *************************************** Container Begin 'Sensors' ****************************************************
container_begin(2, "Info.png", $txt_ue_sensors);
echo "<pre>$sensors</pre>";
container_end();
// *************************************** Container Ende ****************************************************

?>