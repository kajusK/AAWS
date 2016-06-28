<?php
/**
 * Weather station UI
 *
 * @copyright 2016 Jakub Kaderka
 * @license GNU General Public License, version 2; see LICENSE
 */
defined("IN_APP") or die("Unauthorized access");

class Temp
{
	public static function getCurrent() {
		return Db::latest("temp");
	}

	public static function getTendency() {
		return Db::tendency("temp");
	}

	public static function getDewPoint() {
		$temp = self::getCurrent();
		$rh = Humidity::getCurrent();

		//https://en.wikipedia.org/wiki/Dew_point#Calculating_the_dew_point
		$log = log($rh/100*pow(M_E, 17.67*$temp/(243.5+$temp)));
		return round(243.5*$log/(17.67-$log), 1);
	}

	public static function max24h() {
		return Db::getMax("temp", time() - 86400);
	}

	public static function min24h() {
		return Db::getMin("temp", time() - 86400);
	}

	public static function avg24h() {
		return round(Db::getAvg("temp", time() - 86400), 1);
	}
}

