-- phpMyAdmin SQL Dump
-- version 3.4.11.1deb1
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Mar 02, 2013 at 01:57 PM
-- Server version: 5.5.28
-- PHP Version: 5.4.4-13

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `teensypi`
--

-- --------------------------------------------------------

--
-- Table structure for table `pid`
--

CREATE TABLE IF NOT EXISTS `pid` (
  `id` int(11) NOT NULL,
  `enabled` tinyint(1) NOT NULL DEFAULT '0',
  `tempAddr` varchar(50) NOT NULL DEFAULT '0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00',
  `setpoint` double NOT NULL,
  `switchAddr` varchar(50) NOT NULL DEFAULT '0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00',
  `kp` double NOT NULL DEFAULT '0',
  `ki` double NOT NULL DEFAULT '0',
  `kd` double NOT NULL DEFAULT '0',
  `direction` int(11) NOT NULL DEFAULT '1',
  `windowSize` int(11) NOT NULL DEFAULT '5000',
  KEY `id` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `pid`
--

INSERT INTO `pid` (`id`, `enabled`, `tempAddr`, `setpoint`, `switchAddr`, `kp`, `ki`, `kd`, `direction`, `windowSize`) VALUES
(0, 0, '0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', 70, '0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', 0, 0, 0, 0, 5000),
(1, 0, '0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', 70, '0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', 0, 0, 0, 0, 5000),
(2, 0, '0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', 70, '0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', 0, 0, 0, 0, 5000),
(3, 0, '0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', 70, '0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', 0, 0, 0, 0, 5000),
(4, 0, '0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', 70, '0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', 0, 0, 0, 0, 5000),
(5, 0, '0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', 70, '0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', 0, 0, 0, 0, 5000),
(6, 0, '0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', 70, '0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', 0, 0, 0, 0, 5000),
(7, 0, '0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', 70, '0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', 0, 0, 0, 0, 5000),
(8, 0, '0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', 70, '0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', 0, 0, 0, 0, 5000),
(9, 0, '0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', 70, '0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', 0, 0, 0, 0, 5000),
(10, 0, '0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', 70, '0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', 0, 0, 0, 1, 5000),
(11, 0, '0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', 70, '0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00', 0, 0, 0, 0, 5000);

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
