#include "json.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static JSON *create_json(void)
{
#define RET_CHECK(fun) if(fun != 0) goto _failed
#define PTR_CHECK(ptr) if(ptr == NULL) goto _failed

    JSON *json = json_new(JSON_OBJ);
    PTR_CHECK( json );

    JSON *basic = json_new(JSON_OBJ);
    PTR_CHECK( basic );

    PTR_CHECK( json_add_member(json, "basic", basic) );
    PTR_CHECK( json_add_member(basic, "enable", json_new_bool(TRUE)) );
    PTR_CHECK( json_add_member(basic, "ip", json_new_str("200.200.3.61")) );
	PTR_CHECK( json_add_member(basic, "port", json_new_num(389)) );
    PTR_CHECK( json_add_member(basic, "timeout", json_new_num(10)) );
    PTR_CHECK( json_add_member(basic, "basedn", json_new_str("aa")) );
    PTR_CHECK( json_add_member(basic, "maxcnt", json_new_num(133333333333)) );
    JSON *dns = json_new(JSON_ARR);
    PTR_CHECK( dns );
    RET_CHECK( json_arr_add_str(dns, "200.200.0.1") );
    RET_CHECK( json_arr_add_str(dns, "200.0.0.254") ) ;
    PTR_CHECK( json_add_member(basic, "dns", dns) );

    JSON *advance = json_new(JSON_OBJ);
    PTR_CHECK( advance );
    PTR_CHECK( json_add_member(json, "advance", advance) );

    JSON *obj1 = json_new(JSON_OBJ) ;
    PTR_CHECK( obj1 );
    PTR_CHECK( json_add_member(obj1, "name", json_new_str("hunan")) );
    PTR_CHECK( json_add_member(obj1, "ip", json_new_str("200.200.0.1")) );

    JSON *obj2 = json_new(JSON_OBJ);
    PTR_CHECK( obj2 );
    PTR_CHECK( json_add_member(obj2, "name", json_new_str("huabei")) );
    PTR_CHECK( json_add_member(obj2, "ip", json_new_str("200.0.0.254")) );
    
    JSON *adv_dns = json_new(JSON_ARR);
    PTR_CHECK( adv_dns );
    PTR_CHECK( json_add_element(adv_dns, obj1) );
    PTR_CHECK( json_add_element(adv_dns, obj2) );
    PTR_CHECK( json_add_member(advance, "dns", adv_dns) );
    

    JSON *arr = json_new(JSON_ARR);
    PTR_CHECK( arr );
    RET_CHECK( json_arr_add_num(arr, 130) );
    RET_CHECK( json_arr_add_num(arr, 131) );
    RET_CHECK( json_arr_add_num(arr, 132) );
    PTR_CHECK( json_add_member(advance, "portpool", arr) );

    PTR_CHECK( json_add_member(advance, "url", json_new_str("http://200.200.0.4/main")) );
    PTR_CHECK( json_add_member(advance, "path", json_new_str("/etc/sinfors")) );
    PTR_CHECK( json_add_member(advance, "value", json_new_num(3.14)) );

    return json;

#undef RET_CHECK
#undef PTR_CHECK

_failed:
    json_free(json);
    return NULL;
    
}

#if 0
/**
 * @brief 后台服务程序使用的配置项
 */
typedef struct config_st {
    char *ip;
    int port;
    BOOL enable;
    char *basedn;
    //TODO: ...
} config_st;

/**
 * @brief 初始化为默认配置
 */
static void config_init(config_st *cfg)
{
    memset(cfg, 0, sizeof(*cfg));
}
/**
 * @brief 释放加载的配置数据
 */
static void config_clear(config_st *cfg)
{
    free(cfg->ip);
    free(cfg->basedn);
    memset(cfg, 0, sizeof(*cfg));
}
/**
 * @brief 从JSON数据中加载配置
 */
static int config_load(config_st *cfg, const JSON *json)
{
    const JSON *basic = json_get_member(json, "basic");
    if (!basic)
        return -1;
    const char *ip = json_obj_get_str(basic, "ip", NULL);
    if (!ip)
        return -1;
    cfg->ip = strdup(ip);
    cfg->port = json_num(json_get_member(basic, "port"), 80);
    // TODO: ...
    return 0;
}
#endif


int main(int argc, char *argv[])
{
	JSON* json = create_json();
    json_save(json, "./demo.yml");
	json_free(json);
	return 0;
}
