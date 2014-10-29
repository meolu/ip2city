<?php
/* *************************************************************************
 * File Name: example/test-IP2CITY.class.php
 * Author: wushuiyong
 * mail: wushuiyong@huamanshu.com
 * Created Time: Tue 21 Oct 2014 03:51:00 PM
 * ************************************************************************/

include dirname(__FILE__) . '/../IP2CITY.class.php';
$ip2city = new IP2CITY("UTFWry.dat");
$ip = $argv[1] ? $argv[1] : "121.199.24.143";
$address = $ip2city->getAddr($ip);
echo $address . PHP_EOL;
