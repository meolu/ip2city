## ip2city查询(php扩展)

```php
// 只有一个函数ip_city()；只支持一个参数，ip字符串；只返回字符串，以空格分割地区 网格
echo ip_city("122.226.100.69");
// 浙江省金华市 电信
```

## 简单安装
- 直接拷贝ip2city.so到PHP扩展目录
- php.ini中起用`extension=ip2city.so`

## 编译安装
- 下载PHP源码
    
        cd ext
        ./ext_skel --extname=ip2city
        /php/path/php/bin/phpize
    
- 启用

        vi config.m4 #把16行和18行前面的dnl去掉
        16 PHP_ARG_ENABLE(ip2city, whether to enable ip2city support,
        17 dnl Make sure that the comment is aligned:
        18 [  --enable-ip2city           Enable ip2city support])
    
- 编译

        ./configure --with-php-config=/php/path/php/bin/php-config
        make && make install
    
- 配置

    php.ini中起用`extension=ip2city.so`


