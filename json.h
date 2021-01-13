#ifndef JSON_H_
#define JSON_H_


/**
 * 枚举json类型
*/
typedef enum json_e 
{
    JSON_NONE = 0,
    JSON_BOL,           //BOOL类型
    JSON_NUM,           //数值类型
    JSON_STR,           //字符串类型
    JSON_ARR,           //数组类型
    JSON_OBJ,           //对象类型
    JSON_MAX            //错误类型
} json_e;

typedef unsigned short BOOL;
typedef unsigned int U32;
typedef struct value JSON;

#define TRUE 1
#define FALSE 0


/*!
 * @brief 新建一个JSON值
 * @param json json值
 * @return JSON* json地址 失败返回NULL
 * @details
 * 该JSON值可能含子成员，也要一起释放
 */
JSON *json_new(json_e type);


/*!
 * @brief 释放一个JSON值
 * @param json json值
 * @details
 * 该JSON值可能含子成员，也要一起释放
 */
void json_free(JSON *json);


/*!
 * @brief 获取json的类型
 * @param json json值
 * @return json_e json类型
 */
json_e json_type(const JSON *json);


/**
 * @brief 把JSON值json以YAML格式输出，保存到名字为fname的文件中
 * 
 * @param json  JSON值
 * @param fname 输出文件名
 * @return int 0表示成功，<0表示失败
 */
int json_save(const JSON *json, const char *fname);


/**
 * @brief 获取num类型的json值
 * 
 * @param json  JSON值
 * @param def 错误标识符
 * @return double JSON值，失败返回def
 */
double json_num(const JSON *json, double def);


/**
 * @brief 获取JSON_BOOL类型JSON值的布尔值
 * 
 * @param json 布尔值类型的JSON值
 * @return BOOL 如果json是合法的JSON_BOL类型，返回其数值，否则返回FALSE
 */
BOOL json_bool(const JSON *json);


/**
 * @brief 获取JSON_STR类型JSON值的字符串值
 * 
 * @param json 字符串类型的JSON值
 * @param def   类型不匹配时返回的缺省值
 * @return const char* 如果json是合法的JSON_STR类型，返回其字符串值，否则返回def
 */
const char *json_str(const JSON *json, const char *def);


/**
 * @brief 新建一个数字类型的JSON值
 * 
 * @param val 新建JSON的初值
 * @return JSON* JSON值，失败返回NULL
 */
JSON *json_new_num(double val);


/**
 * @brief 新建一个bool类型的JSON值
 * 
 * @param val 新建JSON的初值
 * @return JSON* JSON值，失败返回NULL
 */
JSON *json_new_bool(BOOL val);


/**
 * @brief 新建一个字符串类型的JSON值
 * 
 * @param val 新建JSON的初值
 * @return JSON* JSON值，失败返回NULL
 */
JSON *json_new_str(const char *str);


/**
 * @brief 从对象类型的JSON值中获取名字为key的成员(JSON值)
 * 
 * @param json 对象类型的JSON值
 * @param key  成员的键名
 * @return 找到的成员
 * @details 要求json是个对象类型
 */
const JSON *json_get_member(const JSON *json, const char *key);


/**
 * @brief 从数组类型的JSON值中获取第idx个元素(子JSON值)
 * 
 * @param json 数组类型的JSON值
 * @param idx  元素的索引值
 * @return 找到的元素(JSON值的指针)
 * @details 要求json是个数组
 */
const JSON *json_get_element(const JSON *json, U32 idx);


/**
 * @brief 往对象类型的json中增加一个键值对，键名为key，值为val
 * 
 * @param json JSON对象
 * @param key 键名，符合正则：[a-zA-Z_][a-zA-Z_-0-9]*
 * @param val 键值，必须是堆分配拥有所有权的JSON值
 * @return JSON* 成功返回val，失败返回NULL
 * @details
 *  json_add_member会转移val的所有权，所以调用json_add_member之后不用考虑释放val的问题
 *  json_add_member(json, "port", json_new_num(80));
 */
JSON *json_add_member(JSON *json, const char *key, JSON *val);

/**
 * @brief 往数组类型的json中追加一个元素
 * 
 * @param json JSON数组
 * @param val 加入到数组的元素，必须是堆分配拥有所有权的JSON值
 * @details
 *  json_add_element会转移val的所有权，所以调用json_add_element之后不用考虑释放val的问题
 */
JSON *json_add_element(JSON *json, JSON *val);


/**
 * @brief 获取JSON对象中键名为key的数值，如果获取不到，或者类型不对，返回def
 * 
 * @param json json对象
 * @param key  成员键名
 * @param def  取不到结果时返回的默认值
 * @return double 获取到的数值
 */
double json_obj_get_num(const JSON *json, const char *key, double def);

/**
 * @brief 获取JSON对象中键名为key的BOOL值
 * 
 * @param json json对象
 * @param key  成员键名
 * @return BOOL 获取到的键值，如果获取不到，或者类型不对，返回false
 */
BOOL json_obj_get_bool(const JSON *json, const char *key);

/**
 * @brief 获取JSON对象中键名为key的str值
 * 
 * @param json json对象
 * @param key  成员键名
 * @param def  错误标识符
 * @return char* 获取到的值，如果获取不到，或者类型不对，返回def
 */
const char *json_obj_get_str(const JSON *json, const char *key, const char *def);


/**
 * @brief 往json对象中增加一个num的键值
 * 
 * @param json json对象
 * @param key  成员键名
 * @param val  需增加的num值
 * @return int 0成功，-1失败
 * @details 如果已存在名为key的键，则返回-1
 */
int json_obj_set_num(JSON *json, const char *key, double val);


/**
 * @brief 往json对象中增加一个bool类型的键值
 * 
 * @param json json对象
 * @param key  成员键名
 * @param val  需增加的bool值
 * @return int 0成功，-1失败
 * @details 如果已存在名为key的键，则返回-1
 */
int json_obj_set_bool(JSON *json, const char *key, BOOL val);


/**
 * @brief 往json对象中增加一个str类型的键值
 * 
 * @param json json对象
 * @param key  成员键名
 * @param val  需增加的字符串值
 * @return int 0成功，-1失败
 * @details 如果已存在名为key的键，则返回-1
 */
int json_obj_set_str(JSON *json, const char *key, const char *val);


/**
 * @brief 往json数组中增加一个double类型的键值
 * 
 * @param json json数组
 * @param val  需增加的num值
 * @return int 0成功，-1失败
 * @details 如果已存在名为key的键，则返回-1
 */
int json_arr_add_num(JSON *json, double val);

/**
 * @brief 往json数组中增加一个bool类型的键值
 * 
 * @param json json数组
 * @param val  需增加的bool值
 * @return int 0成功，-1失败
 * @details 如果已存在名为key的键，则返回-1
 */
int json_arr_add_bool(JSON *json, BOOL val);


/**
 * @brief 往json数组中增加一个str类型的键值
 * 
 * @param json json数组
 * @param val  需增加的str值
 * @return int 0成功，-1失败
 * @details 如果已存在名为key的键，则返回-1
 */
int json_arr_add_str(JSON *json, const char *val);


/**
 * @brief 统计json数组元素个数
 * 
 * @param json json数组
 * @return int 0成功，-1失败
 */
int json_arr_count(const JSON *json); 


/**
 * @brief 获取json数组中第idx的num值
 * 
 * @param json json数组
 * @param idx 数组下标
 * @param def 错误标识符
 * @return double 0成功，-1失败
 * @details 需确保第idx个json值必须是num类型
 */
double json_arr_get_num(const JSON *json, int idx, double def);


/**
 * @brief 获取json数组中第idx的bool值
 * 
 * @param json json数组
 * @param idx 数组下标
 * @return BOOL 第idx的BOOL值
 * @details 需确保第idx个json值必须是BOOL类型
 */
BOOL  json_arr_get_bool(const JSON *json, int idx);

/**
 * @brief 获取json数组中第idx的字符串值
 * 
 * @param json json数组
 * @param idx 数组下标
 * @param def 错误标识符
 * @return BOOL 第idx的字符串值，失败返回def
 * @details 需确保第idx个json值必须是BOOL类型
 */
const char *json_arr_get_str(const JSON *json, int idx, const char *def); 

#endif
