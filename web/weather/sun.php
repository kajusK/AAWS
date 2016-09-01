<?php
/**
 * Weather station UI
 *
 * @copyright 2016 Jakub Kaderka
 * @license GNU General Public License, version 2; see LICENSE
 */
defined("IN_APP") or die("Unauthorized access");


class Sun
{
	private static $sun = false;

	private static function DMStoDEC($str) {
		$items = preg_split("/[\s'°]+/", $str);
		$res = $items[0] + ($items[1]*60 + $items[2])/3600;
		if ($items[3] != 'N' && $items[3] != 'E')
			return -$res;
		return $res;
	}

	private static function getSunInfo() {
		$latitude = self::DMStoDEC(Config::get("station", "latitude"));
		$longitude = self::DMStoDEC(Config::get("station", "longitude"));

		self::$sun = date_sun_info(time(), $latitude, $longitude);
	}

	public static function getUV() {
		return Db::latest("uv");
	}

	public static function getMaxUV() {
		return Db::getMax("uv", 0);
	}

	public static function getSunrise() {
		if (!self::$sun)
			self::getSunInfo();
		return date("H:i", self::$sun['sunrise']);
	}

	public static function getSunset() {
		if (!self::$sun)
			self::getSunInfo();
		return date("H:i", self::$sun['sunset']);
	}
}
