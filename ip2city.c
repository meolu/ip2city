/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_ip2city.h"

#include "ext/standard/php_standard.h"


#define BYTE3INT(X)  (    ( X[0] & 0x000000FF ) \
                      | ( ( X[1] & 0x000000FF ) <<  8 ) \
                      | ( ( X[2] & 0x000000FF ) << 16 )  )

#define BYTE4INT(X)  (    ( X[0] & 0x000000FF ) \
                      | ( ( X[1] & 0x000000FF ) <<  8 ) \
                      | ( ( X[2] & 0x000000FF ) << 16 ) \
                      | ( ( X[3] & 0x000000FF ) << 24 )  )
#define BYTE1INT(X)  (    ( X[0] & 0x000000FF ) )

typedef struct IndexIp {
    unsigned char ip[4];
    unsigned char local[3];
} IpStruct;


#define ipsize  (int)sizeof(IpStruct)

#ifdef HAVE_INET_NTOP
ZEND_BEGIN_ARG_INFO(arginfo_inet_ntop, 0)
    ZEND_ARG_INFO(0, in_addr)
ZEND_END_ARG_INFO()
#endif

/* If you declare any globals in php_ip2city.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(ip2city)
*/

/* True global resources - no need for thread safety here */
static int le_ip2city;
FILE * in;
/* {{{ ip2city_functions[]
 *
 * Every user visible function must have an entry in ip2city_functions[].
 */
const zend_function_entry ip2city_functions[] = {
    PHP_FE(ip_city,    NULL)        /* For testing, remove later. */
    PHP_FE_END    /* Must be the last line in ip2city_functions[] */
};
/* }}} */

/* {{{ ip2city_module_entry
 */
zend_module_entry ip2city_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    "ip2city",
    ip2city_functions,
    PHP_MINIT(ip2city),
    PHP_MSHUTDOWN(ip2city),
    PHP_RINIT(ip2city),        /* Replace with NULL if there's nothing to do at request start */
    PHP_RSHUTDOWN(ip2city),    /* Replace with NULL if there's nothing to do at request end */
    PHP_MINFO(ip2city),
#if ZEND_MODULE_API_NO >= 20010901
    "0.1", /* Replace with version number for your extension */
#endif
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_IP2CITY
ZEND_GET_MODULE(ip2city)
#endif

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
    PHP_INI_ENTRY("ip2city.data", "foobar", PHP_INI_ALL, NULL)
PHP_INI_END()
/* }}} */

/* {{{ php_ip2city_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_ip2city_init_globals(zend_ip2city_globals *ip2city_globals)
{
    ip2city_globals->global_value = 0;
    ip2city_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(ip2city)
{
    REGISTER_INI_ENTRIES();
    in = fopen(INI_STR("ip2city.data"), "rb");
    // php_printf("php_minit_function\n");

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(ip2city)
{
    UNREGISTER_INI_ENTRIES();
    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(ip2city)
{

    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(ip2city)
{
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(ip2city)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "ip2city support", "enabled");
    php_info_print_table_end();

    DISPLAY_INI_ENTRIES();
}
/* }}} */


/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string ip_city(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(ip_city)
{
    char *addr;
    size_t addr_len;
    unsigned int iplong = 0;
#ifdef HAVE_INET_PTON
    struct in_addr ip;
#else
    zend_ulong ip;
#endif

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &addr, &addr_len) == FAILURE) {
        return;
    }

#ifdef HAVE_INET_PTON
    if (addr_len == 0 || inet_pton(AF_INET, addr, &ip) != 1) {
        RETURN_FALSE;
    }
    iplong = ntohl(ip.s_addr);
#else
    if (addr_len ==0 || (ip = inet_addr(addr)) == INADDR_NONE) {
        RETURN_FLASE;
    }
    iplong = ntohl(ip);
#endif
    unsigned int indexHead = 0;
    unsigned int indexTail = 0;
    fseek(in, 0, SEEK_SET);
    fread(&indexHead, sizeof(indexHead), 1, in);
    fread(&indexTail, sizeof(indexTail), 1, in);

    IpStruct tmp;
    char local[255] = {0};

    int pos = searchIndex(iplong, in, indexHead, indexTail);
    fseek(in, indexHead+pos*ipsize, SEEK_SET);
    fread(&tmp, ipsize, 1, in);
    searchLocal(tmp, iplong, in, local);
    RETURN_STRING(local, 1);

}

int searchIndex(unsigned int iplong, FILE * db, int startPos, int mount) {
    int i = 0;
    int j = mount;
    int m = 0;
    int c = 0;
    IpStruct tmpA;
    while (i < j - 1) {
        m = (int) (i + j) / 2;
        fseek(db, startPos+m*ipsize, SEEK_SET);
        fread(&tmpA, ipsize, 1, db);
        c = compare(tmpA.ip, iplong);
        // printf("compare out: %d m= %d %s\n", c, m, c < 0 ? "c < 0" : "c not < 0");
        if (c < 0) {
            i = m;
        } else if (c > 0) {
            j = m;
        } else {
            i = j = m;
        }
    }
    return i;

}

int compare(unsigned char a[4], unsigned int tb) {
    unsigned int ta = BYTE4INT(a);
    if (ta > tb) {
        return 1;
    } else if (ta < tb) {
        return -1;
    } else {
        return 0;
    }
}

void GetData(unsigned char* str, FILE* db, int max) {
    int i = 0;
    unsigned char byte[1] = {0};;
    int c = 0;
    while ( (c = fgetc(db)) != '\0') {
        *(str+i)=c;
        i++;
    }
    str[i] = 0;
}

int searchAreaAddr(FILE * db, char * local) {
    int pos = 0;
    char buf[80] = {0};
    fread(buf, 1, 1, db);
    if (buf[0] == 0x02 ) {
        // 获取地区偏移
        fread(buf, 3, 1, db);
        pos = BYTE3INT(buf);
        fseek(db, pos, SEEK_SET);
        fread(buf, 1, 1, db);
    }
    if ( buf[0] == 0x01 || buf[0] == 0x02 ) {
        strcat(local, "未知");
        return 0;
    }
    if ( buf[0] ) {
        GetData(buf+1, db, 40);
    }
    strcat(local, (char*)buf);
}
int searchLocal(IpStruct tmp, unsigned int iplong, FILE * db, char * local) {
    char buf[80] = {0};
    int first = 0;
    int offset = 0;
    int countPos= 0;
    int tmpCount = 0;
    int pos = BYTE3INT(tmp.local);
    fseek(db, pos, SEEK_SET);
    fread(buf, 4, 1, db);
    int c = compare(buf, iplong);

    // 获取资料
    fread(buf, 1, 1, db);
    if ( buf[0] == 0x01 ) { // 国家地区均重复, 跳转至新地址
        fread(buf, 3, 1, db);
        pos = BYTE3INT(buf);
        fseek(db, pos, SEEK_SET);
        fread(buf, 1, 1, db);
    }

    // 获取国家
    if ( buf[0] == 0x02 ) {
        // 获取国家偏移
        fread(buf, 3, 1, db);
        // 保存地区信息
        tmpCount = ftell(db);
        pos = BYTE3INT(buf);
        fseek(db, pos, SEEK_SET);
        fread(buf, 1, 1, db);
    }
    if ( buf[0] == 0x01 || buf[0] == 0x02 ) {
        strcat(local, "未知");
        return;
    }

    if ( buf[0] ) {
        GetData(buf+1, db, 40);
    }
    strcat(local, (char*)buf);
    strcat(local, " ");

    // 获取地区
    if ( tmpCount ) {
        fseek(db, tmpCount, SEEK_SET);
    }
    fread(buf, 1, 1, db);
    while ( buf[0] == 0x02 ) {
        // 获取地区偏移
        fread(buf, 3, 1, db);
        pos = BYTE3INT(buf);
        fseek(db, pos, SEEK_SET);
        fread(buf, 1, 1, db);
    }
    if ( buf[0] == 0x01 || buf[0] == 0x02 ) {
        strcat(local, "未知");
        return;
    }
    if ( buf[0] ) {
        GetData(buf+1, db, 40);
    }
    strcat(local, (char*)buf);
    return;
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and
   unfold functions in source code. See the corresponding marks just before
   function definition, where the functions purpose is also documented. Please
   follow this convention for the convenience of others editing your code.
*/


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
