<?php
/**
 * Weather station UI
 *
 * @copyright 2016 Jakub Kaderka
 * @license GNU General Public License, version 2; see LICENSE
 */
defined("IN_APP") or die("Unauthorized access");

class Press
{
	//returns relative pressure - Prel = Pabs + elevation/8.3
	public static function getCurrent() {
		return Db::latest("pressure") + Config::get("station", "elevation")/8.3;
	}

	public static function getTendency() {
		return Db::tendency("pressure");
	}
}

