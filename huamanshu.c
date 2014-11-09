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
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<inttypes.h>
#include "php.h"
#include "php_ini.h"
#include "ext/standard/basic_functions.h"
#include "ext/standard/info.h"
#include "ext/standard/crc32.h"
#include "php_huamanshu.h"
#include "log.h"
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
typedef unsigned int uint;

/* If you declare any globals in php_huamanshu.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(huamanshu)
*/

/* True global resources - no need for thread safety here */
static int le_huamanshu;
static uint   CRC32[256];
static char   init = 0;

#ifdef HAVE_INET_NTOP
ZEND_BEGIN_ARG_INFO(arginfo_inet_ntop, 0)
    ZEND_ARG_INFO(0, in_addr)
ZEND_END_ARG_INFO()
#endif

PHP_METHOD (IPer, __construct) {
}

PHP_METHOD (IPer, __destruct) {
}

PHP_METHOD (IPer, getLocat) {
    char *addr;
    size_t addr_len;
    unsigned int iplong;
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
    //sprintf(&iplong, "%u", ntohl(ip.s_addr));
    //RETURN_LONG(ntohl(ip.s_addr));
#else
    if (addr_len ==0 || (ip = inet_addr(addr)) == INADDR_NONE) {
        RETURN_FLASE;
    }
    iplong = ntohl(ip);
    //sprintf(&iplong, "%u", ntohl(ip));
    //RETURN_LONG(ntohl(ip));
#endif
    FILE * in;
    // php_printf("ip2long d: %d\n", iplong);
    // php_printf("ip2long u: %u\n", iplong);
    in = fopen("/home/paopao/Data/soft/php-5.5.1/ext/huamanshu/UTFWry.dat", "rb");

    unsigned int indexHead = 0;
    unsigned int indexTail = 0;
    fread(&indexHead, sizeof(indexHead), 1, in);
    fread(&indexTail, sizeof(indexTail), 1, in);

    IpStruct tmp;
    char local[255] = {0};

    php_printf("ip2long: %d\n", iplong);
    int pos = searchIndex(iplong, in, indexHead, indexTail);
    fseek(in, indexHead+pos*ipsize, SEEK_SET);
    fread(&tmp, ipsize, 1, in);
    searchLocal(tmp, iplong, in, local);
    RETURN_STRING(local, 1);

}

int searchIndex(unsigned int iplong, FILE * db, int startPos, int mount) {
    int i = 0, j = mount;
    int m, c;
    IpStruct tmp;

    while (i < j - 1) {
        m = (int) (i + j) / 2;
        fseek(db, startPos+m*ipsize, SEEK_SET);
        fread(&tmp, ipsize, 1, db);
        c = compare(tmp.ip, iplong);
        //printf("compare out: %d %d\n", c, m);
        if (c < 0) {
            i = m;
        } else if (c > 0) {
            j = m;
        } else {
            i = j = m;
        }
    }
    return i;
    //int pos = BYTE3INT(xip.local);
    //printf("pos: %d %d\n", startPos+i*ipsize, pos);

    return 0;
}

int compare(unsigned char a[4], unsigned int tb) {
    unsigned int ta = BYTE4INT(a);
    //printf("%d %d\n", ta, tb);
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
        //byte[0] = fgetc(db);
        i++;
    }
    str[i] = 0;
}

int searchAreaAddr(FILE * db, char * local) {
    int pos = 0;
    char buf[80] = {0};
    fread(buf, 1, 1, db);
    if (1|| buf[0] == 0x02 ) {
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
    // php_printf("local pos: %d\n", pos);
    fseek(db, pos, SEEK_SET);
    fread(buf, 4, 1, db);
    int c = compare(buf, iplong);
    //printf("is compare: %d\n", c);

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
    //printf("%s\t", buf);

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
    if ( buf[0] )
        GetData(buf+1, db, 40);
    strcat(local, (char*)buf);
    return;
    ////////////////////////////////////
    // fread(buf, 1, 1, db);
    // if (buf[0] == 0x01) {
    //     fread(buf, 3, 1, db);
    //     countPos = BYTE3INT(buf);
    //     fseek(db, countPos, SEEK_SET);
    //     fread(buf, 1, 1, db);
    //     if (buf[0] == 0x02) {
    //         fread(buf, 3, 1, db);
    //         GetData(buf+1, db, 50);
    //         strcat(local, (char*)buf);
    //         strcat(local, " ");
    //         printf("[01==02]local: %s\n", local);
    //     } else {
    //         fseek(db, countPos, SEEK_SET);
    //         GetData(buf+1, db, 50);
    //         strcat(local, (char*)buf);
    //         strcat(local, " ");
    //     }
    //         printf("[country]local: %s\n", local);
    //     searchAreaAddr(db, local);
    //         printf("[country+area]local: %s\n", local);
    //     php_printf("\n0x01\n");
    // } else if (buf[0] == 0x02) {
    //     fread(buf, 3, 1, db);
    //     GetData(buf+1, db, 50);
    //     strcat(local, (char*)buf);
    //     printf("[0x02]local: %s\n", local);
    // }
    // return 0;

    // if (buf[0] == 0x02) {
    //     fread(buf, 3, 1, db);
    //     tmpCount = ftell(db);
    //     pos = BYTE3INT(buf);
    //     fseek(db, pos, SEEK_SET);
    //     fread(buf, 1, 1, db);
    //     php_printf(" =========== > %s\n", buf);
    //     if (first == 1) {
    //        // fseek(db, offset, SEEK_SET);
    //     }
    //     php_printf(" =========== > %s\n", buf);
    //     php_printf("\n0x02\n");
    // }

    // if ( buf[0] ) {
    //     GetData(buf+1, db, 50);
    // }
    // // printf("local: %s\n", local);
    // strcat(local, (char*)buf);
    // strcat(local, " ");
    // //printf("local: %s\n", local);

    // // 获取地区
    // if ( tmpCount && first ==0) {
    //     php_printf("\ntmpCount\n");
    //     fseek(db, tmpCount, SEEK_SET);
    //     //fseek(db, offset, SEEK_SET);
    // }
    // fread(buf, 1, 1, db);
    // while ( buf[0] == 0x02 ) {
    //     // 获取地区偏移
    //     fread(buf, 3, 1, db);
    //     pos = BYTE3INT(buf);
    //     fseek(db, pos, SEEK_SET);
    //     fread(buf, 1, 1, db);
    // }
    // if ( buf[0] == 0x01 || buf[0] == 0x02 ) {
    //     strcat(local, "未知");
    //     return 0;
    // }
    // if ( buf[0] ) {
    //     GetData(buf+1, db, 40);
    // }
    // strcat(local, (char*)buf);
    // php_printf("local:%s  \n", local);
    // //printf("local: %s\n", local);

    return 0;
}

/* {{{ huamanshu_functions[]
 *
 * Every user visible function must have an entry in huamanshu_functions[].
 */
const zend_function_entry huamanshu_functions[] = {
    PHP_FE(confirm_huamanshu_compiled,    NULL)        /* For testing, remove later. */
    PHP_FE(vivian, NULL)
    PHP_FE(htable, NULL)
    PHP_FE(syn_log, NULL)
    PHP_FE(has, NULL)
    PHP_FE(hammin, NULL)
    PHP_FE_END    /* Must be the last line in huamanshu_functions[] */
};
const zend_function_entry ip_functions[] = {
    PHP_ME(IPer, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(IPer, __destruct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_DTOR)
    PHP_ME(IPer, getLocat, arg_ip_info, ZEND_ACC_PUBLIC)
    PHP_FE_END    /* Must be the last line in huamanshu_functions[] */
};


/* }}} */

/* {{{ huamanshu_module_entry
 */
zend_module_entry huamanshu_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    "huamanshu",
    huamanshu_functions,
    PHP_MINIT(huamanshu),
    PHP_MSHUTDOWN(huamanshu),
    PHP_RINIT(huamanshu),        /* Replace with NULL if there's nothing to do at request start */
    PHP_RSHUTDOWN(huamanshu),    /* Replace with NULL if there's nothing to do at request end */
    PHP_MINFO(huamanshu),
#if ZEND_MODULE_API_NO >= 20010901
    "0.1", /* Replace with version number for your extension */
#endif
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_HUAMANSHU
ZEND_GET_MODULE(huamanshu)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("huamanshu.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_huamanshu_globals, huamanshu_globals)
    STD_PHP_INI_ENTRY("huamanshu.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_huamanshu_globals, huamanshu_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_huamanshu_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_huamanshu_init_globals(zend_huamanshu_globals *huamanshu_globals)
{
    huamanshu_globals->global_value = 0;
    huamanshu_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
zend_class_entry * IPer_ce;
int time_of_minit;
PHP_MINIT_FUNCTION(huamanshu)
{
    /*
    REGISTER_INI_ENTRIES();
    */
    zend_class_entry IPer;
    INIT_CLASS_ENTRY(IPer, "IPer", ip_functions);
    IPer_ce=zend_register_internal_class_ex(&IPer, NULL, NULL TSRMLS_CC);

    time_of_minit = time(NULL);
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(huamanshu)
{
    /* uncomment this line if you have INI entries
    UNREGISTER_INI_ENTRIES();
    */
    /*
    FILE *fp = fopen("/home/paopao/Data/soft/php-5.5.1/ext/huamanshu/tests/time.txt", "a+");
    fprintf(fp, "php_Mshutdown_function:%d\n", time(NULL));
    fclose(fp);
    */
    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
int time_of_rinit;
PHP_RINIT_FUNCTION(huamanshu)
{
    time_of_rinit = time(NULL);
    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(huamanshu)
{
    /*
    FILE *fp = fopen("/home/paopao/Data/soft/php-5.5.1/ext/huamanshu/tests/time.txt", "a+");
    fprintf(fp, "php_Rshutdown_function:%d\n", time(NULL));
    fclose(fp);
    */
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(huamanshu)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "huamanshu support", "enabled");
    php_info_print_table_end();

    /* Remove comments if you have entries in php.ini
    DISPLAY_INI_ENTRIES();
    */
}
/* }}} */


/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_huamanshu_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_huamanshu_compiled)
{
    char *arg = NULL;
    int arg_len, len;
    char *strg;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
        return;
    }

    len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "huamanshu", arg);
    RETURN_STRINGL(strg, len, 0);
}

PHP_FUNCTION(syn_log)
{
    char *arg = NULL;
    int arg_len, len;
    char *strg;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
        return;
    }

    h_log(arg);

}

int h_log(char *str) {
//    php_var_dump("writing log...");
    snprintf(str, 0, "writing log..");
    return 1;
}

PHP_FUNCTION(has)
{
    char *arg = NULL;
    int arg_len, len;
    char *strg;
    int i;

    int argc = ZEND_NUM_ARGS();

    zval ***args;
    args = (zval ***)safe_emalloc(argc, sizeof(zval **), 0);
    zend_get_parameters_array_ex(argc, args);

    for (i=0; i < argc; i++) {
        php_var_dump(args[i], 1 TSRMLS_CC);
    }


    /*
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",  &arg, &arg_len) == FAILURE) {
        return;
    }
    */

    //PHPWRITE(arg, arg_len);

}

ZEND_FUNCTION(vivian) {
    zval *arg;
    /*
    if (return_value_used) {
        RETURN_STRING("this is vivian method", 0);
    } else {
        php_error_docref(NULL TSRMLS_CC, E_NOTICE, "猫了个咪的，我就知道你没用我的劳动成果！");
        RETURN_NULL();
    }
    php_printf("time_of_minit:%d  \n", time_of_minit);
    php_printf("time_of_rinit:%d  \n", time_of_rinit);
    */
    /*
    //  detect type
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &arg) == FAILURE) {
        return;
    }

    switch(Z_TYPE_PP(arg)) {
        case IS_NULL:
            RETURN_STRING("null", 1);
            break;
        case IS_BOOL:
            RETURN_STRING("bool", 1);
            break;
        case IS_ARRAY:
            RETURN_STRING("array", 1);
            break;
        case IS_STRING:
            RETURN_STRING("string", 1);
            break;
        default:
            RETURN_STRING("sha wanyi", 1);
    }
    */
    /*
    // 创建变量
    //
    zval *fooval;
    MAKE_STD_ZVAL(fooval);
    ZVAL_STRING(fooval, "bar", 1);
    ZEND_SET_SYMBOL(EG(active_symbol_table), "foo", fooval);
    */
    /*
    zval ** fooval;
    if (zend_hash_find(
            EG(active_symbol_table),
            "foo",
            sizeof("foo"),
            (void **)&fooval
        ) == SUCCESS
    ) {
        php_printf("find var $foo");
    } else {
        php_printf("can't find ");
    }
    */

    //FILE *fp = fopen("UTFWry.dat");
    char o[10];
    //fread(o, 4, 2, fp);
    return;
}

ZEND_FUNCTION(hammin) {
    char *r1 = NULL, *r2 = NULL;
    int n = 0, m = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &r1, &n, &r2, &m) == FAILURE) {
        return;
    }
    while(n && m && *r1 == *r2) {
        r1++;
        r2++;
        n--;
        m--;
    }
    if(n == 0) RETURN_LONG(m);
    if(m == 0) RETURN_LONG(n);
    int d[n+1][m+1];
    int cost;
    int i,j;
    for(i = 0; i <= n; i++) d[i][0] = i;
    for(j = 0; j <= m; j++) d[0][j] = j;
    for(i = 1; i <= n; i++) {
        for(j = 1; j <= m; j++) {
            if(r1[i-1] == r2[j-1]) cost = 0;
            else cost = 1;
            int a = MIN(d[i-1][j]+1,d[i][j-1]+1);
            a = MIN(a, d[i-1][j-1]+cost);
            d[i][j] = a;
        }
    }
    RETURN_LONG(d[n][m]);
}
ZEND_FUNCTION(htable) {
    char *p;
    int len, nr;
    php_uint32 crcinit = 0;
    register php_uint32 crc;
    double crc_d;
    php_uint32 per = 10;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &p, &nr) == FAILURE) {
        return;
    }
    crc = crcinit^0xFFFFFFFF;

    for (len =+nr; nr--; ++p) {
        crc = ((crc >> 8) & 0x00FFFFFF) ^ crc32tab[(crc ^ (*p)) & 0xFF ];
    }
    crc = crc^0xFFFFFFFF;
    crc_d = (double) crc;

    RETURN_LONG((php_uint32)crc_d % per + 1);
}
