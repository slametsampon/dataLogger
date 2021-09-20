## Users Management
#### User data:
typedef struct userData
#### { 
1. String username;
2. String password;//123456
3. String email;//exampla@example.com
4. int level;
####
} userData;
### class AccesUser
#### public - functions:
1. AccesUser(String);
2. String getJson();
3. userData getUser();
4. String getUser(int);
5. void setUserJson(JsonObject);
6. void setUser(userData);
7. void setUser(int, String);
8. String getId();
9. void info();

#### private - variables:
1. String    _id;
2. userData _userData;
### Class ini bagian dari : [dataLogger](https://github.com/slametsampon/dataLogger/blob/main/config/dataLogger.md)
