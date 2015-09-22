## ip2city查询(php扩展)

```php
// 只有一个函数ip_city()；只支持一个参数，ip字符串；只返回字符串，以空格分割地区 网格
echo ip_city("122.226.100.69");
// 浙江省金华市 电信
```


## 扩展快速安装
```shell
git clone git@github.com:meolu/ip2city.git
cd ip2city
/php/path/bin/phpize
./configure --with-php-config=/php/path/bin/php-config
make && make install
```

## php.ini配置
- php.ini中添加`extension=ip2city.so`
- php.ini中添加数据文件：`ip2city.data=/path/of/ip2city.data`

