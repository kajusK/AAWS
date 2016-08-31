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
	public static function getUV() {
		return Db::latest("uv");
	}

	public static function getMaxUV() {
		return Db::getMax("uv", 0);
	}

	public static function getSunrise() {
		return 0;
	}

	public static function getSunset() {
		return 0;
	}
}
