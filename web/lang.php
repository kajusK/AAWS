<?php
/**
 * Weather station UI
 *
 * @copyright 2016 Jakub Kaderka
 * @license GNU General Public License, version 2; see LICENSE
 */
defined("IN_APP") or die("Unauthorized access");

class Lang
{
	private static $lang = "en";
	private static $lang_list = false;
	private static $fallbacked = false;
	private static $translations = array();

	//select language to use and load translations
	public static function select() {
		self::$lang = self::getDefault();

		if (isset($_GET["lang"]))
			self::$lang = $_GET["lang"];

		if (!self::isPresent(self::$lang) || !self::isEnabled(self::$lang)) {
			self::$fallbacked = true;
			self::$lang = self::getDefault();
			if (!self::isPresent(self::$lang))
				self::$lang = "en";
		}

		require_once("lang/".self::$lang.".php");
		self::$translations = $translation;
	}

	public static function get($item) {
		if (!isset(self::$translations[$item]))
			return "Not translated";
		return self::$translations[$item];
	}

	//get currently selected language
	public static function getLang() {
		return self::$lang;
	}

	public static function getDefault() {
		$enabled = Config::get("general", "lang_enabled");
		return explode(',', $enabled)[0];
	}

	//return all enabled languages in array["short"] = "full name"
	public static function getLangs() {
		if (self::$lang_list)
			return self::$lang_list;

		self::$lang_list = array();
		foreach(scandir("./lang") as $name) {
			if ($name[0] == "." || !self::isEnabled($name[0]))
				continue;
			//TODO: There should be a better way, this is waste
			//of memory and time...
			include "./lang/".$name;
			self::$lang_list[explode(".", $name)[0]] = $lang_name;
		}
		return self::$lang_list;
	}

	//is the $lang in translations?
	public static function isPresent($lang) {
		return is_file("lang/".$lang.".php");
	}

	public static function isEnabled($lang) {
		if (strpos(Config::get("general", "lang_enabled"), $lang) !== false)
			return true;
		return false;
	}

	//fallbacked to default language?
	public static function isFallbacked() {
		return self::$fallbacked;
	}
}
