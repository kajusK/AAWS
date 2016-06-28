<?php
/**
 * Weather station UI
 *
 * @copyright 2016 Jakub Kaderka
 * @license GNU General Public License, version 2; see LICENSE
 */

define("IN_APP", true);
define("VERSION", "0.1");

require_once "config.php";
require_once "error.php";

if (Config::get("general", "debug"))
	error_reporting(-1);
else
	error_reporting(0);

require_once "lang.php";
require_once "db.php";
Db::init(Config::get("db", "type"));

function get_link($path, $lang=false)
{
	if (Lang::getLang() != Config::get("general","default_lang") && !$lang)
		$lang = Lang::getLang();
	$lang_param = $lang ? "?lang=".$lang : "";

	if ($path == false || $path == "index")
		return "./".$lang_param;

	if (strlen($lang_param))
		return "./".$lang_param."&amp;page=".$path;
	return "./?page=".$path;
}

//set timezone
if (date_default_timezone_set(Config::get("general", "timezone")) == false) {
	date_default_timezone_set("UTC");
	error_log("WARN: UTC timezone used");
}

Lang::select();
//get page to display
$page = isset($_GET['page']) ? $_GET['page'] : "index";
if (Lang::isFallbacked() || !is_file("view/".$page.".phtml") || $page == "404") {
	Err::statusCode(404);
	$page = "404";
}

require_once "weather/weather.php";
switch($page) {
case "index":
	$data = Weather::generate();
	break;

case "records":
	$data = Weather::records();
	break;
}

if (isset($data))
	extract($data);

#load the page
require "view/"."head.phtml";
require "view/".$page.".phtml";
require "view/"."foot.phtml";
