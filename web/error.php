<?php
/**
 * Weather station UI
 *
 * @copyright 2016 Jakub Kaderka
 * @license GNU General Public License, version 2; see LICENSE
 */
defined("IN_APP") or die("Unauthorized access");

class Err
{
	private static $logged = array();

	public static function statusCode($code)
	{
		switch ($code){
		case 301:
			$string = "301 Moved Permanently";
			break;
		case 401:
			$string = "401 Unauthorized";
			break;
		case 404:
			$string = "404 Not Found";
			break;
		case 500:
			$string = "500 Internal Server Error";
			break;
		case 503:
			$string = "503 Service Unavaliable";
			break;
		}

		if (!isset($string))
			return false;

		header($_SERVER["SERVER_PROTOCOL"]." ".$string);
		return true;
	}

	// return status code $code and die with $message shown to user
	public static function fatal($message, $code)
	{
		self::statusCode($code);
		die("<H1>$message<H1>");
	}

	//log error to array for get method
	public static function minor($message, $code=false) {
		self::$logged[] = $message;
		if ($code)
			self::statusCode($code);
	}

	//return array of minor errors
	public static function get() {
		return self::$logged;
	}
}
