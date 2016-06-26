<?php
/**
 * Weather station UI
 *
 * @copyright 2016 Jakub Kaderka
 * @license GNU General Public License, version 2; see LICENSE
 */
defined("IN_APP") or die("Unauthorized access");

class Config
{
	private static $config = false;

	public static function get($class, $option) {
		if (!self::$config) {
			self::$config = parse_ini_file("config.ini", true);
			if (!self::$config)
				Err::fatal("Unable to parse config file", 500);
		}

		if (!isset(self::$config[$class][$option]))
			return false;
		return self::$config[$class][$option];
	}
}
