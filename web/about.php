<?php
/**
 * Weather station UI
 *
 * @copyright 2016 Jakub Kaderka
 * @license GNU General Public License, version 2; see LICENSE
 */
defined("IN_APP") or die("Unauthorized access");

if (is_file("about/".Lang::getLang()).".phtml")
	require "about/".Lang::getLang().".phtml";
else
	require "view/about.phtml";
