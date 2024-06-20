#include<stdio.h>
#include<sqlite3.h>

typedef struct Student
{
    int id;
    char name[20];
    char dept[20];
    double chinese;
    double math;
    double english;
}Student;

const char* student_toSql(Student* stu)
{
    static char buf[BUFSIZ] = {0};
    sqlite3_snprintf(BUFSIZ, buf, "INSERT INTO students(id, name, dept, chinese, math, english)\
    VALUES(%d,'%s','%s',%lf,%lf,%lf)",stu->id,stu->name,stu->dept,stu->chinese,stu->math,stu->english);
    return buf;
}

const char* student_header()
{
    return "学号、姓名、专业、语文成绩、数学成绩、英语成绩";
}

int printTable(void* data,int column,char** values,char** fileds)
{
    printf("data: %d\n", *(int*)data);

    for(size_t i=0; i<column; i++)
        printf("%s  ",fileds[i]);

    printf("\n");
    for(size_t i=0; i<column; i++)
        printf("%s  ",values[i]);
    printf("\n");
    return SQLITE_OK;

}

int main()
{
    int rc = SQLITE_ERROR;

    //1.打开数据库
    sqlite3 *db = NULL;
    rc = sqlite3_open("student.db", &db);
    if(rc == SQLITE_ERROR)
    {
        sqlite3_log(sqlite3_errcode(db), "open failed!");
        return -1;
    }

    //2.创建数据表
    rc = sqlite3_exec(db, "CREATE TABLE students(id INT, name VARCHAR(20), dept VARCHAR(30), chinese REAL,\
                        math REAL, english REAL)", NULL, NULL, NULL);
    if(rc == SQLITE_OK)
    {
        printf("创建students表成功~\n");
    }
    else
    {
        printf("创建失败~：%s\n", sqlite3_errmsg(db));
    }

    /*
    //3.插入数据表
    rc = sqlite3_exec(db, "INSERT INTO students(id, name, dept, chinese, math, english) VALUES(100,'maye','软件工程',100,100,100)", NULL, NULL, NULL);
    if(rc != SQLITE_OK)
    {
        printf("insert into failed：%s\n", sqlite3_errmsg(db));
    }
    */

    //3.1让用户输入
    printf("请输入学生信息(%s)：\n", student_header());
    Student stu;
    scanf("%d %s %s %lf %lf %lf",&stu.id,stu.name,stu.dept,&stu.chinese,&stu.math,&stu.english);
    rc = sqlite3_exec(db, student_toSql(&stu), NULL, NULL, NULL);
    if(rc != SQLITE_OK)
    {
        printf("输入失败：%s\n", sqlite3_errmsg(db));
    }
    //4.查询数据表
    int number = 999;
    rc = sqlite3_exec(db, "SELECT * FROM students", printTable, &number, NULL);
    if(rc == SQLITE_OK)
    {
        printf("查询成功\n");
    }
    else
    {
        printf("查询失败~：%s\n", sqlite3_errmsg(db));
    }
    //5.修改数据表
    rc = sqlite3_exec(db, "UPDATE students SET name='JEFF' WHERE id=100", NULL, NULL, NULL);
    if(rc != SQLITE_OK)
    {
        printf("修改失败~：%s\n", sqlite3_errmsg(db));
    }
    /*
    //6.删除数据表
    rc = sqlite3_exec(db, "DELETE FROM students WHERE id=100", NULL, NULL, NULL);
    if(rc != SQLITE_OK)
    {
        printf("删除失败~：%s\n", sqlite3_errmsg(db));
    }
    */

    //再次输出表
    char **result = NULL;
    int rowCount, columnCount;
    char *errMsg;
    rc = sqlite3_get_table(db, 
            "SELECT * FROM students",
            &result,
            &rowCount,
            &columnCount,
            &errMsg);
    if(rc != SQLITE_OK)
    {
        printf("显示失败~：%s\n", sqlite3_errmsg(db));
    }
    else
    {
        for(size_t j=0; j<= rowCount; j++)
        {
            for(size_t k=0; k<columnCount; k++)
                printf("%s  ",result[j*columnCount + k]);
            printf("\n");
        }
    }
    //关闭数据库
    sqlite3_close(db);

    return 0;
}


