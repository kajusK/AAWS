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
	//returns relative pressure
	public static function getCurrent() {
		$press = Db::latest("pressure");
		return round($press, 1);
	}

	public static function getTendency() {
		return Db::tendency("pressure");
	}

	public static function getMax() {
		return Db::getMax("pressure", 0);
	}

	public static function getMin() {
		return Db::getMin("pressure", 0);
	}
}

