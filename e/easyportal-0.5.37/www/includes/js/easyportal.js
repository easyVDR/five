/*

		easyportal.js v0.1
		by nogood (www.easy-vdr.de)

*/


// --------------------------------------------------------------------------------------
// Loaderanzeige
function wait(a) {
	//DIVToMitte(a)
	document.getElementById(a).style.display='block';
}

function endWait(a) {
	document.getElementById(a).style.display='none';
}
// --------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------
// Auf Upates Prüfen
function CheckUpdates() {
  wait('WaitForUpdates');
  var myAjax = new Ajax.Request(
    "includes/home_version.inc.php?action=ckupdates",
    { method: 'get', onComplete: show_Updates }
  );
}

function show_Updates( originalRequest ) {
  $('version').innerHTML = originalRequest.responseText;
  endWait('WaitForUpdates')
}
// --------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------
// Platten
function platten() {
  wait('WaitPlatten');
  var myAjax = new Ajax.Request(
    "includes/platten.inc.php",
    { method: 'get', onComplete: show_platten }
  );
}

function show_platten( originalRequest ) {
   $('platten').innerHTML = originalRequest.responseText;
   endWait('WaitPlatten');
}
// --------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------
// Dienste
function dienste() {
  wait('WaitReloadDienste');
  var myAjax = new Ajax.Request(
    "includes/dienste.inc.php",
    { method: 'get', onComplete: show_dienste }
  );
}

function show_dienste( originalRequest ) {
   $('dienste').innerHTML = originalRequest.responseText;
   endWait('WaitReloadDienste');
}
// --------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------
function startvdr() {
  wait('WaitStartVdr');
  var myAjax = new Ajax.Request(
    "functions/startvdr.php",
    { method: 'get', onComplete: reload_info }
  );
}

function reload_info( originalRequest ) {
  platten();
  dienste();
  endWait('WaitStartVdr');
}
// --------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------
// Aktivierung/Deaktivierung von all_plugins.inc.php
function AllEditPlugin(action, plugin, logfile) {
  wait('WaitEditPlugin');
  var myAjax = new Ajax.Request(
      "functions/plugin.php?action=" + action + "&plugin=" + plugin + "&logfile=" + logfile,
      { method: 'get', onComplete: AllPlugins}
  );
}

function AllPlugins() {
  endWait('WaitEditPlugin');
  wait('WaitPlugins');
  var myAjax = new Ajax.Request(
    "includes/all_plugins.inc.php?restart=yes",
    { method: 'get', onComplete: show_AllPlugins }
  );
}

function show_AllPlugins( originalRequest ) {
  $('all_plugins').innerHTML = originalRequest.responseText;
  endWait('WaitPlugins');
  endWait('WaitEditPlugin');
}
// --------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------
// loaded_plugins.inc.php

function LoadedPlugins() {
  wait('WaitPlugins');
  var myAjax = new Ajax.Request(
    "includes/loaded_plugins.inc.php",
    { method: 'get', onComplete: show_LoadedPlugins }
  );
}

function show_LoadedPlugins( originalRequest ) {
  $('loaded_plugins').innerHTML = originalRequest.responseText;
  endWait('WaitPlugins');
}

// --------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------
// history_plugins.inc.php

function EditPluginHistory(action, plugin, logfile) {
  wait('WaitEditPlugin');
  var myAjax = new Ajax.Request(
      "functions/plugin.php?action=" + action + "&plugin=" + plugin + "&logfile=" + logfile,
      { method: 'get', onComplete: PluginHistory }
  );
}

function PluginHistory() {
	endWait('WaitEditPlugin');
	wait('WaitLogbuch');
  var myAjax = new Ajax.Request(
    "includes/plugin_history.inc.php",
    { method: 'get', onComplete: show_PluginHistory }
  );
}

function show_PluginHistory( originalRequest ) {
  $('plugin_history').innerHTML = originalRequest.responseText;
  endWait('WaitEditPlugin');
  endWait('WaitLogbuch');
}
// --------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------
// Logbuch löschen
function DelVerlauf() {
	wait('WaitLogbuch');
  var myAjax = new Ajax.Request(
    "functions/dellog.php",
    { method: 'get', onComplete: PluginHistory }
  );
}
// --------------------------------------------------------------------------------------