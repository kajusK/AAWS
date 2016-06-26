<?php
/**
 * Weather station UI
 *
 * @copyright 2016 Jakub Kaderka
 * @license GNU General Public License, version 2; see LICENSE
 */
defined("IN_APP") or die("Unauthorized access");

class Rain
{
	public static function getIntensity() {
		return Db::latest("rain");
	}

	public static function lastHour() {
		return Db::latest("rain", "rain");
	}

	public static function sinceMidnight() {
		return Db::getSum("rain", mktime(0,0,0), "rain");
	}

	public static function thisMonth() {
		return Db::getSum("rain", mktime(0,0,0,date("n"), 1), "rain");
	}

	public static function thisYear() {
		return Db::getSum("rain", time(0,0,0,1,1), "rain");
	}

	public static function getLatestDate() {
		return Db::getLatestDate();
	}
}

