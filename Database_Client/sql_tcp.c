#include <stdio.h>
#include <sqlite3.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include "sql_tcp.h"

int rc = SQLITE_ERROR;
sqlite3 *db = NULL;

struct Data ClientData = {  
    .id = 0,  
    .dept = "0", // 编译器通常会自动添加 '\0'  
    .time = "0" // 同上  
};  

void CreatBase()
{
//1.打开数据库
    rc = sqlite3_open("database.db", &db);
    if(rc == SQLITE_ERROR)
    {
        sqlite3_log(sqlite3_errcode(db), "open failed!");
    }
}

void CreatTable()
{
//2.创建数据表
    rc = sqlite3_exec(db, "CREATE TABLE datamemory(id INT, dept VARCHAR(20)\
    , time VARCHAR(20))", NULL, NULL, NULL);
    if(rc == SQLITE_OK)
    {
        printf("创建datamemory成功~\n");
    }
    else
    {
        printf("创建失败~：%s\n", sqlite3_errmsg(db));
    }
}

const char* Data_toSql(struct Data* data)
{
    static char buf[BUFSIZ] = {0};
    sqlite3_snprintf(BUFSIZ, buf, "INSERT INTO datamemory(id, dept, time)\
    VALUES(%d,'%s','%s')",data->id,data->dept,data->time);
    return buf;
}

void InsertData(struct Data* data)
{
//3让用户输入
    rc = sqlite3_exec(db, Data_toSql(data), NULL, NULL, NULL);
    if(rc != SQLITE_OK)
    {
        printf("输入失败：%s\n", sqlite3_errmsg(db));
    }
}

void get_time(char *buf)
{
    struct tm *tm_ptr;
    time_t the_time;
    (void)time(&the_time);
    tm_ptr = gmtime(&the_time);
    sprintf(buf, "%02d-%02d-%02d %02d:%02d:%02d",tm_ptr->tm_year+1900, tm_ptr->tm_mon+1, tm_ptr->tm_mday,(tm_ptr->tm_hour)+8, tm_ptr->tm_min, tm_ptr->tm_sec);
}

int printTable(void* time,int column,char** values,char** fileds)
{
    printf("query time: %s\n", (char *)time);

    for(size_t i=0; i<column; i++)
        printf("%s  ",fileds[i]);

    printf("\n");
    for(size_t i=0; i<column; i++)
        printf("%s  ",values[i]);
    printf("\n");
    return SQLITE_OK;

}

void FindData()
{
    //4.查询数据表
    char buf[512];
    memset(buf, 0, sizeof(buf));
    get_time(buf);

    rc = sqlite3_exec(db, "SELECT * FROM datamemory", printTable, buf, NULL);
    if(rc == SQLITE_OK)
    {
        printf("查询成功\n");
    }
    else
    {
        printf("查询失败~：%s\n", sqlite3_errmsg(db));
    }
}

void CloseTable()
{
    //关闭数据库
    sqlite3_close(db);
}
