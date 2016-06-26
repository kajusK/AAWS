<?php
/**
 * Weather station UI
 *
 * @copyright 2016 Jakub Kaderka
 * @license GNU General Public License, version 2; see LICENSE
 */
defined("IN_APP") or die("Unauthorized access");

class Humidity
{
	public static function getCurrent() {
		return Db::latest("humidity");
	}

	public static function getTendency() {
		return Db::tendency("humidity");
	}
}

