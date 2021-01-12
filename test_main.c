#include "json.h"
#include "xtest.h"
#include <stdio.h>
#include <assert.h>
#include <errno.h>

TEST(json_new, exit)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json != NULL);
    free(json);
}

TEST(json_type, stat)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json != NULL);
    ASSERT_TRUE(JSON_OBJ == json_type(json));
    free(json);
}
//  测试键值对存在的情况
TEST(json_obj_get_str, exist)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json != NULL);

    ASSERT_TRUE(NULL != json_add_member(json, "ip", json_new_str("200.200.3.61")));
    const char *ip = json_obj_get_str(json, "ip", NULL);
    ASSERT_TRUE(ip != NULL);
    ASSERT_STRCASEEQ("200.200.3.61", ip);

    json_free(json);
}

//  测试键值对不存在的情况
TEST(json_obj_get_str, notexist)
{
    JSON *json = json_new(JSON_OBJ);
    ASSERT_TRUE(json != NULL);

    ASSERT_TRUE(NULL != json_add_member(json, "ip", json_new_str("200.200.3.61")));
    const char *ip = json_obj_get_str(json, "ip2", NULL);
    ASSERT_TRUE(ip == NULL);

    ip = json_obj_get_str(json, "ip3", "default");
    ASSERT_TRUE(ip != NULL);
    ASSERT_STRCASEEQ("default", ip);

    json_free(json);
}


// 测试改变num的json值
TEST(json_obj_set_num, stat)
{
	JSON *json = json_new(JSON_OBJ);
	ASSERT_TRUE(json != NULL);
	
	ASSERT_TRUE(NULL != json_add_member(json, "num", json_new_num(22)));
	ASSERT_EQ(22.0, json_obj_get_num(json, "num", -1.0));
	
	ASSERT_EQ(0, json_obj_set_num(json, "num", 23));
	
	ASSERT_EQ(23.0, json_obj_get_num(json, "num", -1.0));
	
	json_free(json);
}

//测试改变json的bool值
TEST(json_obj_set_bool, stat)
{
	JSON *json = json_new(JSON_OBJ);
	ASSERT_TRUE(json != NULL);
	
	ASSERT_TRUE(NULL != json_add_member(json, "bol", json_new_bool(TRUE)));
	ASSERT_EQ(TRUE, json_obj_get_bool(json, "bol"));
	
	ASSERT_EQ(0, json_obj_set_bool(json, "bol", FALSE));
	
	ASSERT_EQ(0, json_obj_get_bool(json, "bol"));
	
	json_free(json);
}

//测试json字符串
TEST(json_obj_set_str, stat)
{
	JSON *json = json_new(JSON_OBJ);
	ASSERT_TRUE(json != NULL);
	
	ASSERT_TRUE(NULL != json_add_member(json, "str", json_new_str("hello word")));
	ASSERT_STREQ("hello word", json_obj_get_str(json, "str", "error"));
	ASSERT_EQ(0, json_obj_set_str(json, "str", "xuchong is nb"));
	ASSERT_STREQ("xuchong is nb", json_obj_get_str(json, "str", "error"));
	
	json_free(json);	
}

//数组set bool
TEST(json_arr_set_bool, stat)
{
    JSON *json = json_new(JSON_ARR);
	ASSERT_TRUE(json != NULL);

    ASSERT_EQ(0, json_arr_add_bool(json, TRUE));
    ASSERT_EQ(TRUE, json_arr_get_bool(json, 0));
    free(json);
}

TEST(json_arr_set_num, stat)
{
    JSON *json = json_new(JSON_ARR);
	ASSERT_TRUE(json != NULL);

    ASSERT_EQ(0, json_arr_add_num(json, 1.1));
    ASSERT_EQ(1.1 , json_arr_get_num(json, 0, -1));
    free(json);
}

TEST(json_arr_set_str, stat)
{
    JSON *json = json_new(JSON_ARR);
	ASSERT_TRUE(json != NULL);

    ASSERT_EQ(0, json_arr_add_str(json, "hello world"));
    ASSERT_STREQ("hello world", json_arr_get_str(json, 0, "error"));
    free(json);
}


//测试json数组
TEST(json_arr, stat)
{
	JSON *json = json_new(JSON_OBJ);
	ASSERT_TRUE(json != NULL);
	
	ASSERT_TRUE(NULL != json_add_member(json, "arr", json_new(JSON_ARR)));
	
	ASSERT_EQ(0, json_arr_add_num((JSON*)json_get_member(json, "arr"), 123.0));
	ASSERT_EQ(0, json_arr_add_bool((JSON*)json_get_member(json, "arr"), TRUE));
	ASSERT_EQ(0, json_arr_add_str((JSON*)json_get_member(json, "arr"), "xuchongnb"));
	ASSERT_TRUE(NULL != json_add_element((JSON*)json_get_member(json, "arr"), json_new_str("hello")));
	
	const JSON *cjson = json_get_member(json, "arr");
	ASSERT_EQ(123.0, json_num(json_get_element(cjson, 0), -1.0));
	ASSERT_EQ(123.0, json_arr_get_num(cjson, 0, -1.0));
	ASSERT_EQ(TRUE, json_arr_get_bool(cjson, 1));
	ASSERT_STREQ("xuchongnb", json_arr_get_str(cjson, 2, "error"));
	ASSERT_STREQ("hello",json_arr_get_str(cjson, 3, "error"));
	ASSERT_EQ(4, json_arr_count(cjson));
	
	json_free(json);
}


typedef struct buf_t {
    char *str;
    unsigned int size;
} buf_t;

int read_file(buf_t *buf, const char *fname)
{
    FILE *fp;
    long len;
    long realsize;

    assert(buf);
    assert(fname);
    assert(fname[0]);

    fp = fopen(fname, "rb");
    if (!fp) {
        fprintf(stderr, "open file [%s] failed\n", fname);
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    if (len <= 0) {
        fclose(fp);
        fprintf(stderr, "ftell failed, errno: %d\n", errno);
        return -1;
    }
    fseek(fp, 0, SEEK_SET);
    buf->str = (char *)malloc(len + 1);
    if (!buf->str) {
        fclose(fp);
        buf->size = 0;
        fprintf(stderr, "malloc(%ld) failed\n", len + 1);
        return -1;
    }
    buf->size = len + 1;
    realsize = fread(buf->str, 1, len, fp);
    fclose(fp);

    buf->str[realsize] = '\0';
    return 0;
}

TEST(json_save, str)
{
    JSON *json;
    buf_t result;
    const char *expect = "\"hello world\"\n";

    json = json_new_str("hello world");
    EXPECT_EQ(0, json_save(json, "test.yml"));
    EXPECT_EQ(0, read_file(&result, "test.yml"));

    ASSERT_TRUE(strcmp(result.str, expect) == 0);
    free(result.str);
    json_free(json);
}



TEST(json_save, obj)
{
    JSON *json;
    buf_t result;
    const char *expect = "key: \"hello\"\nname: \"world\"\n";

    json = json_new(JSON_OBJ);
    json_add_member(json, "key", json_new_str("hello"));
    json_add_member(json, "name", json_new_str("world"));

    EXPECT_EQ(0, json_save(json, "test-obj.yml"));
    EXPECT_EQ(0, read_file(&result, "test-obj.yml"));

    ASSERT_TRUE(strcmp(result.str, expect) == 0);
    free(result.str);
    json_free(json);
}



int main(int argc, char **argv)
{
	return xtest_start_test(argc, argv);
}

