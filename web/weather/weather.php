<?php

require_once "wind.php";
require_once "rain.php";
require_once "temp.php";
require_once "press.php";
require_once "humidity.php";

class Weather {
	public static function generate() {
		$data = array(
			'temp' => Temp::getCurrent(),
			'temp_tendency' => Temp::getTendency(),
			'dew_point' => Temp::getDewPoint(),
			'temp_max' => Temp::max24h(),
			'temp_min' => Temp::min24h(),
			'temp_avg' => Temp::avg24h(),
			'wind_speed' => Wind::getSpeed(),
			'wind_dir' => Wind::getDirection(),
			'wind_gusts' => Wind::getGusts(),
			'rain' => Rain::getIntensity(),
			'rain_hour' => Rain::lastHour(),
			'rain_midnight' => Rain::sinceMidnight(),
			'rain_month' => Rain::thisMonth(),
			'rain_year' => Rain::thisYear(),
			'rain_date' => Rain::getLatestDate(),
			'pres' => Press::getCurrent(),
			'pres_tendency' => Press::getTendency(),
			'humidity' => Humidity::getCurrent(),
			'humidity_tendency' => Humidity::getTendency());

		return $data;
	}

	public static function getWeek()
	{
		return Db::getAllSince(time()-604800, 1);
	}

	public static function getMonth()
	{
		return Db::getAllSince(time()-2678400, 4);

	}

	public static function getYear()
	{
		return Db::getAllSince(time()-31556926, 72);

	}

	public static function records() {

	}

}
