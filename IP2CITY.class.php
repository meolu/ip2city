<?php
/**
 * ip2city
 * author:wushuiyong@huamanshu.com
 * date 2014-5-22
 */

class IP2CITY {
    private $db; // ip 数据库文件

    private $firstIndex;

    private $lastIndex;

    private $count;

    private $curIpStart;

    private $curIpEnd;

    private $curOffset;

    function __construct($db) {
        $this->db = fopen($db, "rb");
        $buf = fread($this->db, 8);
        $list = unpack("I2", $buf);
        sort($list);
        list($this->firstIndex, $this->lastIndex) = $list;
        $this->count = ($this->lastIndex - $this->firstIndex) / 7 + 1;
    }

    function ip2city($ip) {
        if (!is_numeric($ip)) $ip = ip2long($ip);
        $ip = pack("N", intval($ip));
        $this->searchIpRange($ip);
        if ($this->curIpStart <= $ip && $ip <= $this->curIpEnd) {
            return $this->searchAddr();
        } else {
            return "not found";
        }
    }

    function searchAddr() {
        fseek($this->db, $this->curOffset + 4);
        $mode = fread($this->db, 1);
        switch (ord($mode)) {
            case "1":
                $offset = $this->get3byte();
                fseek($this->db, $offset);
                $m = fread($this->db, 1);
                if (2 == ord($m)) {
                    $cOffset = $this->get3byte();
                    $cAddr = $this->getString($cOffset);
                    fseek($this->db, $offset + 4);
                } else {
                    $cAddr = $this->getString($offset);
                }
                $aAddr = $this->searchArea();
                break;

            case "2":
                $offset = $this->get3byte();
                $cAddr  = $this->getString($offset);
                $aAddr  = $this->searchArea($this->curOffset + 8);// 看图片有解释，为什么是偏移8
                break;

            default :
                $cAddr = $this->getString($this->curOffset + 4);
                $aAddr = $this->getString();
                break;
        }
        return $cAddr . " " . $aAddr;
    }

    function searchArea($offset = '') {
        if ($offset) fseek($this->db, $offset);
        $mode = fread($this->db, 1);
        if (in_array(ord($mode), array(1, 2))) {
            $offset = $this->get3byte();
            $address = $this->getString($offset);
        } else {
            fseek($tihs->db, -1, 1);
            $address = $this->getString();
        }
        return $address;
    }

    function searchIpRange($ip) {
        $L = 0;
        $R = $this->count - 1;
        $M = floor(($L + $R) / 2);
        while ($L < $R - 1) {
            $this->setOffset($M);
            if ($ip < $this->curIpStart) {
                $R = $M;
            } elseif ($this->curIpEnd < $ip) {
                $L = $M;
            } else {
                $L = $M;
                break;
            }
            $M = floor(($L + $R) / 2);
        }
    }

    function setOffset($index) {
        $this->curOffset = ftell($this->db);
        fseek($this->db, $this->firstIndex + $index * 7);
        $this->curIpStart = strrev(fread($this->db, 4));
        $this->curOffset = $this->get3byte();
        fseek($this->db, $this->curOffset);
        $this->curIpEnd = strrev(fread($this->db, 4));
   }

    private function get3byte($offset = '') {
        if ($offset) fseek($this->db, $offset);

        $result = unpack('Vlong', fread($this->db, 3).chr(0));
        return $result['long'];
    }

    private function getString($offset = '') {
        if ($offset) fseek($this->db, $offset);
        $str = '';
        $byte = fread($this->db, 1);
        while (ord($byte) > 0) {
            $str .= $byte;
            $byte = fread($this->db, 1);
        }
        return $str;
    }

}

