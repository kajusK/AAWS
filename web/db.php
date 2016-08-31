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
	private static $pdo = false;
	private static $latest = false;

	public static function init($type) {
		if (self::$pdo)
			return;

		switch ($type) {
		case "sqlite":
			$file = Config::get("db", "filename");
			$dsn = is_file($file) ? "sqlite:".$file : false;
			$user = null;
			$pass = null;
			$opts = false;
			break;
		case "mysql":
			$dsn = "mysql:host=".Config::get("db", "host").";";
			$dsn .= "dbname=".Config::get("db", "name").";";
			$dsn .= "port=".Config::get("db", "port").";";
			$dsn .= "charset=utf-8";
			$user = Config::get("db", "user");
			$pass = Config::get("db", "pass");
			$opts = false;
			break;
		default:
			Err:fatal("Incorrect database selected", 500);
		}

		$opt = array(
			     PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION,
			     PDO::ATTR_DEFAULT_FETCH_MODE => PDO::FETCH_ASSOC);

		try {
			self::$pdo = new PDO($dsn, $user, $pass, $opt);
		} catch (PDOException $e) {
			error_log("Unable to connect to database".$e->getMessage());
			Err::fatal("Unable to connect to database", 500);
		}
	}


	public static function latest($field, $table="weather") {
		if (!isset(self::$latest[$table])) {
			$q = "SELECT * FROM '$table' ORDER BY timestamp DESC LIMIT 1";
			self::$latest[$table] = self::_query($q, array());
		}

		if (!isset(self::$latest[$table][$field]))
			return 'NaN';
		return self::$latest[$table][$field];
	}

	//difference between current value and value an hour ago
	public static function tendency($field, $table="weather") {
		$q = "SELECT $field FROM '$table' WHERE timestamp >= ?";
		$q .= " ORDER BY timestamp LIMIT 1";

		$hour_ago = self::_query($q, array(time()-3600));
		if (!isset($hour_ago['temp']))
			return 0;

		return self::latest($field, $table) - $hour_ago[$field];
	}

	public static function getSum($field, $since, $table="weather") {
		$q = "SELECT sum($field) FROM '$table' WHERE timestamp >= ?";
		$res = self::_query($q, array($since));
		return reset($res);
	}

	public static function getMax($field, $since, $table="weather") {
		$q = "SELECT max($field) as max,timestamp FROM '$table' ";
		$q .= "WHERE timestamp >= ?";
		$res = self::_query($q, array($since));
		$date = date("j.n.y h:i", $res['timestamp']);
		return array($res['max'], $date);
	}

	public static function getMin($field, $since, $table="weather") {
		$q = "SELECT min($field) as min, timestamp FROM '$table' ";
		$q .= "WHERE timestamp >= ?";
		$res = self::_query($q, array($since));
		$date = date("j.n.y h:i", $res['timestamp']);
		return array($res['min'], $date);
	}

	public static function getAvg($field, $since, $table="weather") {
		$q = "SELECT avg($field) FROM '$table' WHERE timestamp >= ?";
		$res = self::_query($q, array($since));
		return reset($res);
	}

	public static function getLatestDate() {
		$q = "SELECT timestamp FROM 'weather' WHERE rain != 0 ";
		$q .= "ORDER BY timestamp DESC LIMIT 1";
		$time = self::_query($q, array())['timestamp'];

		return date("j.n.Y H:i", $time);
	}

	public static function getAllSince($since, $step) {
		$q = "SELECT * FROM 'weather' WHERE timestamp >= ? ";
		$q .= "ORDER BY timestamp";
		//TODO: get only every step item

		$res = self::_query($q, array($since), false);
		return $res;
	}

	private static function _query($statement, $params, $single=true) {
		$stmt = self::$pdo->prepare($statement);
		$stmt->execute($params);

		if ($single)
			return $stmt->fetch();
		return $stmt->fetchAll();
	}
}
