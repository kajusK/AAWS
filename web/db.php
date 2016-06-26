<?php
/**
 * Weather station UI
 *
 * @copyright 2016 Jakub Kaderka
 * @license GNU General Public License, version 2; see LICENSE
 */
defined("IN_APP") or die("Unauthorized access");

Class Db
{
	public static function latest($field, $table="weather") {
		return 10;
	}

	//difference between current value and value an hour ago
	public static function tendency($field) {
		return 20;
	}

	public static function getSum($field, $since, $table="weather") {
		return 30;
	}

	public static function getMax($field, $since, $table="weather") {
		return 60;
	}

	public static function getMin($field, $since, $table="weather") {
		return -10;
	}

	public static function getAvg($field, $since, $table="weather") {
		return 11.2;
	}

	public static function getLatestDate() {
		return "1.1.2000";
	}
}
