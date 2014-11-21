<?php
/* *************************************************************************
 * File Name: test-ip2city.php
 * Author: wushuiyong
 * mail: wushuiyong@huamanshu.com
 * Created Time: Sun 19 Oct 2014 07:51:07 PM
 * ************************************************************************/

$ip = $argv[1] ? $argv[1] : "122.226.100.69";
echo ip_city($ip), PHP_EOL;
// 浙江省金华市 电信
