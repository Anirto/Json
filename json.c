#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>
#include "json.h"

typedef struct array array;
typedef struct object object;
typedef struct value value;
typedef struct keyvalue keyvalue;

struct array 
{
    value **elems;      /* 想想: 这里如果定义为'value *elems'会怎样？ */
    U32 count;          //elems中有多少个value*
};

struct keyvalue 
{
    char *key;          //键名
    value *val;         //值
};

struct object 
{
    keyvalue *kvs;      //这是一个keyvalue的数组，可以通过realloc的方式扩充的动态数组
    U32 count;          //数组kvs中有几个键值对
};

struct value 
{
    json_e type;        //JSON值的具体类型
    union {
        double num;     //数值，当type==JSON_NUM时有效
        BOOL bol;       //布尔值，当type==JSON_BOL时有效
        char *str;      //字符串值，堆中分配的一个字符串，当type==JSON_STR时有效
        array arr;      //值数组，当type==JSON_ARR时有效
        object obj;     //对象，当type==JSON_OBJ时有效
    };
};

JSON *json_new(json_e type)
{
    if (type < 0 || type > 5)
        return NULL;
    JSON *json = (JSON *)calloc(1, sizeof(JSON));
    if (!json) {
        //想想：为什么输出到stderr，不用printf输出到stdout？
        fprintf(stderr, "json_new: calloc(%lu) failed\n", sizeof(JSON));
        return NULL;
    }
    json->type = type;
    return json;
}

void json_free(JSON *json)
{
    if (json == NULL)
		return ;

	if ( json_type(json) == JSON_OBJ )
	{
		keyvalue *p;
		for (int i = 0; i < json->obj.count; i++)
		{
			p = json->obj.kvs + i;
			json_free(p->val);
			free(p->key);
		}
        free(json->obj.kvs);
	}
	else if (json_type(json) == JSON_ARR)
	{
		value **p;
		for (int i = 0; i < json->obj.count; i++)
		{
			p = json->arr.elems + i;
			json_free(*p);
		}
		free(json->arr.elems);
	}
	else if (json_type(json) == JSON_STR)
	{
		free(json->str);
	}

	free(json);

}

json_e json_type(const JSON *json)
{
    assert(json);
    return json ? json->type : JSON_NONE;
}

static void json_write(FILE *fp, const JSON *json, int rank, BOOL back)
{
    switch (json_type(json))
    {
    case JSON_OBJ:
        for (int i = 0; i < json->obj.count; i++)
        {
            if (back == TRUE)
            for (int i = 0; i < rank; i++)
                    fprintf(fp, "\t");
            back = TRUE;

            fprintf(fp, "%s: ", json->obj.kvs[i].key);

            json_e type = json_type(json->obj.kvs[i].val);
            if ( type == JSON_OBJ || type == JSON_ARR )
                fprintf(fp, "\n");
            
            json_write(fp, json->obj.kvs[i].val, rank+1, TRUE);   
        }
        break;

    case JSON_BOL:
        fprintf(fp, "\"%s\"\n", json->bol == TRUE ? "true":"false");
        break;
    
    case JSON_NUM:
        fprintf(fp, "%g\n", json->num);
        break;

    case JSON_STR:
        fprintf(fp, "\"%s\"\n", json->str);
        break;
    
    case JSON_ARR:
        for(int i = 0; i < json->arr.count; i++)
        {
            for (int i = 0; i < rank; i++)
                    fprintf(fp, "\t");
            fprintf(fp, "- ");
            json_write(fp, json->arr.elems[i], rank+1, FALSE);
        }
        break;
    
    default:
        break;
    }
}

int json_save(const JSON *json, const char *fname)
{
    FILE *fp = fopen(fname, "w");
    if (fp == NULL ) 
        return -1;
    
    json_write(fp, json, 0, TRUE);

    fclose(fp);
    return 0;
}

JSON *json_new_bool(BOOL val)
{
    //TODO:
	JSON *json = json_new(JSON_BOL);
	if (!json)
		return NULL;
	json->bol = val;
	return json;
}

JSON *json_new_str(const char *str)
{
    JSON *json;
    assert(str);
    
    json = json_new(JSON_STR);
    if (!json)
        return json;
    json->str = strdup(str);
    if (!json->str) {
        fprintf(stderr, "json_new_str: strdup(%s) failed", str);
        json_free(json);
        return NULL;
    }
    return json;
}

JSON *json_new_num(double val)
{
    JSON *json = json_new(JSON_NUM);
	if (!json)
		return NULL;
	json->num = val;
	return json;
}

double json_num(const JSON *json, double def)
{
    //想想：为什么这里不assert(json)?
    return json && json->type == JSON_NUM ? json->num : def;
}

BOOL json_bool(const JSON *json)
{
    //想想：为什么这里不assert(json)?
    return json && json->type == JSON_BOL ? json->bol : FALSE;
}

const char *json_str(const JSON *json, const char *def)
{
    //想想：为什么这里不assert(json)?
    return json && json->type == JSON_STR ? json->str : def;
}

const JSON *json_get_member(const JSON *json, const char *key)
{
    U32 i;
    assert(json);
    assert(json->type == JSON_OBJ);
    assert(!(json->obj.count > 0 && json->obj.kvs == NULL));
    assert(key);
    assert(key[0]);

    for (i = 0; i < json->obj.count; ++i) {
        if (strcmp(json->obj.kvs[i].key, key) == 0)
            return json->obj.kvs[i].val;
    }
    return NULL;
}

const JSON *json_get_element(const JSON *json, U32 idx)
{
    assert(json);
    assert(json->type == JSON_ARR);
    assert(!(json->arr.count > 0 && json->arr.elems == NULL));

    if (json->type != JSON_ARR)
        return NULL;
    if (idx >= json->arr.count)
        return NULL;
    return json->arr.elems[idx];
}

JSON *json_add_member(JSON *json, const char *key, JSON *val)
{
    assert(json->type == JSON_OBJ);
    assert(!(json->obj.count > 0 && json->obj.kvs == NULL));
    assert(key);
    assert(key[0]);
    //想想: 为啥不用assert检查val？
    //想想：如果json中已经存在名字为key的成员，怎么办？
    //TODO:
	keyvalue * p = (keyvalue *)realloc(json->obj.kvs, (json->obj.count + 1) * sizeof(keyvalue));
	if (p == NULL)
	{
		json_free(val);
		return NULL;
	}
	json->obj.kvs = p;

	p[json->obj.count].key = strdup(key);
	p[json->obj.count].val = val;
	json->obj.count++;

    return val;
}

JSON *json_add_element(JSON *json, JSON *val)
{
    assert(json);
    assert(json->type == JSON_ARR);
    assert(!(json->arr.count > 0 && json->arr.elems == NULL));

    //想想：为啥不用assert检查val？
    value ** p = (value **)realloc(json->arr.elems, (json->arr.count + 1) * sizeof(value));
	if (p == NULL)
		return NULL;

	json->arr.elems = p;
	p = p + json->arr.count;
	*p = val;
	json->arr.count++;
    return json;
}

static const JSON *get_child(const JSON *json, const char *key, json_e expect_type)
{
    const JSON *child;

    child = json_get_member(json, key);
    if (!child)
        return NULL;
    if (child->type != expect_type)
        return NULL;
    return child;
}

double json_obj_get_num(const JSON *json, const char *key, double def)
{
    const JSON *child = get_child(json, key, JSON_NUM);
    if (!child)
        return def;
    return child->num;
}

BOOL json_obj_get_bool(const JSON *json, const char *key)
{
    const JSON *child = get_child(json, key, JSON_BOL);
    if (!child)
        return FALSE;
    return child->bol;
}

const char *json_obj_get_str(const JSON *json, const char *key, const char *def)
{
    const JSON *child = get_child(json, key, JSON_STR);
    if (!child)
        return def;
    return child->str;
}

int json_obj_set_num(JSON *json, const char *key, double val)
{
    JSON *p = (JSON*)get_child(json, key, JSON_NUM);
	if (p == NULL)
		return -1;
	p->num = val;
	return 0;
}

int json_obj_set_bool(JSON *json, const char *key, BOOL val)
{
	JSON *p = (JSON*)get_child(json, key, JSON_BOL);
	if (p == NULL)
		return -1;
	p->bol = val;
	return 0;
}

int json_obj_set_str(JSON *json, const char *key, const char *val)
{
	assert(json);
    value *p = (value*)get_child(json, key, JSON_STR);
	if(p == NULL)
		return -1;

	free(p->str);
	p->str=strdup(val);
	return 0;
}

int json_arr_count(const JSON *json)
{
	assert(json);
    if (!json || json->type != JSON_ARR)
        return -1;
    return json->arr.count;
}

double json_arr_get_num(const JSON *json, int idx, double def)
{
    if(json == NULL || idx >= json->arr.count)
		return def;
	return json_num(json->arr.elems[idx], -1);
    //return def;
}

BOOL json_arr_get_bool(const JSON *json, int idx)
{
	assert(json);
	return json_bool(json->arr.elems[idx]);
}

const char *json_arr_get_str(const JSON *json, int idx, const char *def)
{
    //TODO:
	if(json == NULL || idx >= json->arr.count)
		return def;
	return json_str(json->arr.elems[idx], "error");
}

int json_arr_add_num(JSON *json, double val)
{
    if (json == NULL || json->type != JSON_ARR)
		return -1;

	JSON *p = json_new_num(val);
	if(p == NULL)
		return -1;

	if(NULL == json_add_element(json, p))
		return -1;
	return 0;
}

int json_arr_add_bool(JSON *json, BOOL val)
{
    if(json == NULL)
		return -1;

	JSON *p = json_new_bool(val);
	if (p == NULL)
		return -1;

	if(json_add_element(json, p) == NULL)
		return -1;
	return 0;
}

int json_arr_add_str(JSON *json, const char *val)
{
	if(json == NULL || val == NULL)
		return -1;

	JSON *p = json_new_str(val);
	if (p == NULL)
		return -1;

	if(json_add_element(json, p) == NULL)
		return -1;

	return 0;
}

