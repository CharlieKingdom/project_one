struct Data
{
    int id;
    char dept[20];
    char time[20];
};

void CreatBase();
void CreatTable();
void InsertData(struct Data* data);
void FindData();
void CloseTable();
void get_time(char *buf);