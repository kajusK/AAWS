<?php
/**
 * Weather station UI
 *
 * @copyright 2016 Jakub Kaderka
 * @license GNU General Public License, version 2; see LICENSE
 */
defined("IN_APP") or die("Unauthorized access");

//TODO: rewrite to generate correct times, synodic period is not constant!
//current dates might be a day off
class Moon
{
	private static $period = 2551443; //average synodic period of Moon
	private static $since_new = false;

	/* Get seconds since last new moon */
	private static function getSinceNew() {
		if (self::$since_new)
			return self::$since_new;

		//new moon date in UTC - e.g. 10.1.2016 1:30
		$new_date = gmmktime(1,30,0,1,10,2016);
		return self::$since_new;
	}

	/* Get moon age in days */
	public static function getAge() {
		return round(self::getSinceNew()/86400);
	}

	/* Illumination in % */
	public static function getIllumination() {
		$age = self::getSinceNew();
		return round($age/self::$period*100);
	}

	public static function getNextFull() {
		$age = self::getSinceNew();
		$half = self::$period/2;

		if ($age > $half)
			$next = time() + (self::$period - $age) + $half;
		else
			$next = time() + ($half - $age);

		return date("j.n.Y", $next);
	}

	public static function getNextNew() {
		$age = self::getSinceNew();
		$next = time() + (self::$period - $age);
		return date("j.n.Y", $next);
	}
}
