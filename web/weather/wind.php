<?php
/**
 * Weather station UI
 *
 * @copyright 2016 Jakub Kaderka
 * @license GNU General Public License, version 2; see LICENSE
 */
defined("IN_APP") or die("Unauthorized access");

require_once "weather.php";

class Wind
{
	public static function getSpeed() {
		return Db::latest("wind_speed");
	}

	public static function getDirection() {
		return Db::latest("wind_direction");
	}

	public static function getGusts() {
		return Db::latest("wind_gusts");
	}
}
