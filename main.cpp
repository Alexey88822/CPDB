#include <iostream>
#include <windows.h>
#include <odbcinst.h>
#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>
#include <string>
#include <sstream>
#include <vector>
using namespace std;
// SELECT sysbook.id, type_table.type, sysbook.location, sysbook.ipaddress, sysbook.macaddress, services_table.service from sysbook, type_table, services_table, hardware_services_table WHERE sysbook.type_id=type_table.id AND hardware_services_table.id_service=services_table.id AND sysbook.id=hardware_services_table.id_hardware;
// CREATE TABLE sysbook(id SERIAL PRIMARY KEY, type_id integer references type_table(id), location text, ipaddress text, macaddress text);
// CREATE TABLE type_table(id SERIAL PRIMARY KEY, type text);
// CREATE TABLE services_table(id SERIAL PRIMARY KEY, service text);
// CREATE TABLE hardware_services_table(id SERIAL PRIMARY KEY, id_service integer references services_table(id), id_hardware integer references sysbook(id) ON DELETE CASCADE);
#define SQL_HANDLE_ENV 1
#define SQL_HANDLE_DBC 2
#define SQL_HANDLE_STMT 3
#define SQL_HANDLE_DESC 4
SQLHENV outputEnv;
SQLHDBC hdbcOutput;
SQLHSTMT varSelect;
SQLHSTMT varInsert;
SQLHSTMT varDelete;
SQLHSTMT varUpdate;
SQLHSTMT varCreate;
SQLCHAR selecttxt[] = "SELECT sysbook.id, type_table.type, sysbook.location, sysbook.ipaddress, sysbook.macaddress, services_table.service from sysbook, type_table, services_table, hardware_services_table WHERE sysbook.type_id=type_table.id AND hardware_services_table.id_service=services_table.id AND sysbook.id=hardware_services_table.id_hardware;";
SQLCHAR deletetxt[] = "DELETE FROM sysbook WHERE id=?;";
SQLSCHAR sqf2[50], sqf3[50], sqf4[50], sqf5[50], sqf6[70];
SQLLEN sqf1,sbf1,sbf2,sbf3,sbf4,sbf5,sbf6;

void checkDescriptor(SQLHENV env, SQLHDBC hdbc, SQLHSTMT varDescriptor) {
    SQLCHAR sqlstate[6],Msg[SQL_MAX_MESSAGE_LENGTH];
    SQLINTEGER    NativeError;
    SQLSMALLINT   MsgLen;
    SQLRETURN rc2 = SQLError(env, hdbc, varDescriptor, sqlstate, &NativeError, Msg, sizeof(Msg), &MsgLen);
    std::cout << Msg << std::endl;
}

void checkAnswer(int code) {
    if (code == 0) {
        std::cout << "#SQL_SUCCESS" << std::endl;
    }
    if (code == -2) {
        std::cout << "#SQL_INVALID_HANDLE" << std::endl;
    }
    if (code == -1) {
        std::cout << "#SQL_ERROR" << std::endl;
    }
    if (code == 1) {
        std::cout << "#SQL_SUCCESS_WITH_INFO" << std::endl;
    }
}

void checkTables() { // Проверка существования таблиц
    SQLRETURN code;
    code = SQLAllocHandle(SQL_HANDLE_STMT, hdbcOutput, &varCreate);
    checkAnswer(code);
    code = SQLExecDirect(varCreate, (SQLCHAR*)"CREATE TABLE sysbook(id SERIAL PRIMARY KEY, type_id integer references type_table(id), location text, ipaddress text, macaddress text);", SQL_NTS);
    if (code==0) {
        std::cout << "Table sysbook was create" << endl;
    }
    else {
        checkDescriptor(outputEnv, hdbcOutput, varCreate);
    }
    code = SQLExecDirect(varCreate, (SQLCHAR*)"CREATE TABLE type_table(id SERIAL PRIMARY KEY, type text);", SQL_NTS);
    if (code==0) {
        std::cout << "Table type_table was create" << endl;
    }
    else {
        checkDescriptor(outputEnv, hdbcOutput, varCreate);
    }
    code = SQLExecDirect(varCreate, (SQLCHAR*)"CREATE TABLE services_table(id SERIAL PRIMARY KEY, service text);", SQL_NTS);
    if (code==0) {
        std::cout << "Table services_table was create" << endl;
    }
    else {
        checkDescriptor(outputEnv, hdbcOutput, varCreate);
    }
    code = SQLExecDirect(varCreate, (SQLCHAR*)"CREATE TABLE hardware_services_table(id SERIAL PRIMARY KEY, id_service integer references services_table(id), id_hardware integer references sysbook(id) ON DELETE CASCADE);", SQL_NTS);
    if (code==0) {
        std::cout << "Table hardware_services_table was create" << endl;
    }
    else {
        checkDescriptor(outputEnv, hdbcOutput, varCreate);
    }
}

int initConnection(char* UserName, SQLCHAR Password[]) { // Установка соединения с базой данных
    setlocale(LC_ALL, "Russian");
    std::cout << "Connecting..." << std::endl;
    printf("ODBC Version: %04X,%04X\n",ODBCVER,0x0300);
    SQLSMALLINT code;
    std::cout << "Creating environment descriptor..." << std::endl;
    code = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &outputEnv);
    checkAnswer(code);
    code = SQLSetEnvAttr(outputEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    std::cout << "Creating connection descriptor..." << std::endl;
    code = SQLAllocHandle(SQL_HANDLE_DBC, outputEnv, &hdbcOutput);
    checkAnswer(code);
    std::cout << "Connecting to DB..." << std::endl;
    code = SQLConnect(hdbcOutput, (SQLTCHAR*)"PostgreSQL35W", SQL_NTS, (SQLCHAR*)UserName, SQL_NTS, (SQLCHAR*)Password, SQL_NTS);
    std::cout << "Check connection: " << code << std::endl;
    if (code == 0) {
        std::cout << "CONNECTION SUCCESSFULL!" << std::endl;
        return 0;
    }
    else {
        std::cout << "FAILED TO CONNECT!" << std::endl;
        return -1;
    }
}

int Update() { // Обновление данных в таблице
    SQLRETURN retcode;
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbcOutput, &varUpdate);
    checkAnswer(retcode);
    SQLCHAR first[50];
    SQLCHAR second[50];
    cout << "IP Address of equipment: " << endl;
    cin >> second;
    cout << "Type of equipment(1 - PC, 2 - Laptop, 3 - Phone, 4 - Server): " << endl;
    cin >> first;
    SQLPrepare(varUpdate, (SQLCHAR*)"UPDATE sysbook SET type_id=? WHERE ipaddress=?", SQL_NTS);
    SQLBindParameter(varUpdate, 1, SQL_PARAM_INPUT,
                     SQL_C_CHAR, SQL_CHAR,
                     0, 0,
                     first, 0, NULL);
    SQLBindParameter(varUpdate, 2, SQL_PARAM_INPUT,
                     SQL_C_CHAR, SQL_CHAR,
                     0, 0,
                     second, 0, NULL);
    retcode = SQLExecute(varUpdate);
    if (retcode == 0) {
        std::cout << "{UPDATE} - Successfull update!" << std::endl;
    }
    else {
        std::cout << "Error in Execute {FUNCTION:UPDATE}! " << "Retcode of Error: " << retcode << std::endl;
        checkAnswer(retcode);
        checkDescriptor(outputEnv, hdbcOutput, varUpdate);
    }
}

enum {
    OPERATION_SELECT = 1,
};

int getData(int Operation, SQLSCHAR Parametr[]) { // Operation: 1 - SELECT; 2 - SearchType, 3 - SearchIP, 4 - SearchMAC
    SQLRETURN code;
    code = SQLAllocHandle(SQL_HANDLE_STMT, hdbcOutput, &varSelect);
    checkAnswer(code);
    code = SQLExecDirect(varSelect, selecttxt, SQL_NTS);
    if (code == 0) {
        while (TRUE) {
            SQLRETURN retcode = SQLFetch(varSelect);
            if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO) {
                std::cout << "Error!" << std::endl;
            }
            if (retcode == 100) {
                std::cout << "No data available." << std::endl;
            }
            if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {/* Извлечение данных результирующего набора */
                SQLGetData(varSelect, 1, SQL_C_ULONG, &sqf1, 0, &sbf1);
                SQLGetData(varSelect, 2, SQL_C_CHAR, sqf2, 50, &sbf2);
                SQLGetData(varSelect, 3, SQL_C_CHAR, sqf3, 50, &sbf3);
                SQLGetData(varSelect, 4, SQL_C_CHAR, sqf4, 50, &sbf4);
                SQLGetData(varSelect, 5, SQL_C_CHAR, sqf5, 50, &sbf5);
                SQLGetData(varSelect, 6, SQL_C_CHAR, sqf6, 70, &sbf6);
                if (Operation == 1) { // SELECT * FROM
                    std::cout << "ID: " << sqf1 << "   TYPE: " << sqf2 << "  LOCATION: " << sqf3 << "   IPADDRESS: "
                              << sqf4 << "   MACADDRESS: " << sqf5 << "   SUPPORTEDSERVICES: " << sqf6 << std::endl;
                }
                if (Operation == 2) { // Searching by Type
                    string type = (string)((char*)sqf2);
                    string parametr = (string)(char*)Parametr;
                    if (type.find(parametr)!=-1) {
                        std::cout << "ID: " << sqf1 << "   TYPE: " << sqf2 << "  LOCATION: " << sqf3 << "   IPADDRESS: "
                                  << sqf4 << "   MACADDRESS: " << sqf5 << "   SUPPORTEDSERVICES: " << sqf6 << std::endl;
                    }
                }
                if (Operation == 3) { // Searching IP-Address
                    string ipaddress = (string)((char*)sqf4);
                    string parametr = (string)(char*)Parametr;
                    if (ipaddress.find(parametr)!=-1) {
                        std::cout << "ID: " << sqf1 << "   TYPE: " << sqf2 << "  LOCATION: " << sqf3 << "   IPADDRESS: "
                                  << sqf4 << "   MACADDRESS: " << sqf5 << "   SUPPORTEDSERVICES: " << sqf6 << std::endl;
                    }
                }
                if (Operation == 4) { // Searching MAC-Address
                    string macaddress = (string)((char*)sqf5);
                    string parametr = (string)(char*)Parametr;
                    if (macaddress.find(parametr)!=-1) {
                        std::cout << "ID: " << sqf1 << "   TYPE: " << sqf2 << "  LOCATION: " << sqf3 << "   IPADDRESS: "
                                  << sqf4 << "   MACADDRESS: " << sqf5 << "   SUPPORTEDSERVICES: " << sqf6 << std::endl;
                    }
                }
            } else {
                break;
            }
        }
        if (Operation == 1) std::cout << "{FUNCTION:GETDATA} - Successfull Select!" << std::endl;
        if (Operation == 2) std::cout << "{FUNCTION:GETDATA} - Successfull SearchType!" << std::endl;
        if (Operation == 3) std::cout << "{FUNCTION:GETDATA} - Successfull SearchIP!" << std::endl;
        if (Operation == 4) std::cout << "{FUNCTION:GETDATA} - Successfull SearchMAC!" << std::endl;
    }
    else {
        std::cout << "Error in ExecDirect! {FUNCTION:GETDATA} " << "Retcode of Error: " << code << std::endl;
        checkAnswer(code);
        checkDescriptor(outputEnv, hdbcOutput, varSelect);
    }
}

int insertToTable() { // Добавление данных в таблицу
    SQLSCHAR idEquipment[50];
    SQLRETURN retcode;
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbcOutput, &varInsert);
    checkAnswer(retcode);
    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        SQLCHAR sF1_ID[50], sF2_ID[50], sF3_ID[50], sF4_ID[50], sF5_ID[50];
        SQLPrepare(varInsert, (SQLCHAR*)"INSERT INTO sysbook(type_id, location, ipaddress, macaddress) VALUES(?, ?, ?, ?);", SQL_NTS);
        cout << "Type(1-PC, 2-Laptop, 3-Phone, 4-Server): ";
        cin >> sF1_ID;
        cout << "Location: ";
        cin >> sF2_ID;
        cout << "IP-Address: ";
        cin >> sF3_ID;
        cout << "MAC-Address: ";
        cin >> sF4_ID;
        cout << "Service(1-ftp, 2-telnet, 3-ssh, 4-snmp, 5-cifs): ";
        cin >> sF5_ID;
        const char delim = ',';
        string supportedservices = (string)((char*)sF5_ID);
        std::vector<std::string> elems;
        std::stringstream ss(supportedservices);
        std::string item;
        while(std::getline(ss, item, delim))
        {
            elems.push_back(item);
        }
        SQLBindParameter(varInsert, 1, SQL_PARAM_INPUT,
                                   SQL_C_CHAR, SQL_CHAR,
                                   0, 0,
                                   sF1_ID, 0, NULL);
        SQLBindParameter(varInsert, 2, SQL_PARAM_INPUT,
                                   SQL_C_CHAR, SQL_CHAR,
                                   0, 0,
                                   sF2_ID, 0, NULL);
        SQLBindParameter(varInsert, 3, SQL_PARAM_INPUT,
                                   SQL_C_CHAR, SQL_CHAR,
                                   0, 0,
                                   sF3_ID, 0, NULL);
        SQLBindParameter(varInsert, 4, SQL_PARAM_INPUT,
                                   SQL_C_CHAR, SQL_CHAR,
                                   0, 0,
                                   sF4_ID, 0, NULL);
        retcode = SQLExecute(varInsert);
        if (retcode == 0) {
            SQLRETURN code;
            code = SQLAllocHandle(SQL_HANDLE_STMT, hdbcOutput, &varSelect);
            checkAnswer(code);
            SQLPrepare(varSelect, (SQLCHAR*)"SELECT id FROM sysbook WHERE type_id=? AND location=? AND ipaddress=? AND macaddress=?;", SQL_NTS);
            SQLBindParameter(varSelect, 1, SQL_PARAM_INPUT,
                             SQL_C_CHAR, SQL_CHAR,
                             0, 0,
                             &sF1_ID, 0, NULL);
            SQLBindParameter(varSelect, 2, SQL_PARAM_INPUT,
                             SQL_C_CHAR, SQL_CHAR,
                             0, 0,
                             &sF2_ID, 0, NULL);
            SQLBindParameter(varSelect, 3, SQL_PARAM_INPUT,
                             SQL_C_CHAR, SQL_CHAR,
                             0, 0,
                             &sF3_ID, 0, NULL);
            SQLBindParameter(varSelect, 4, SQL_PARAM_INPUT,
                             SQL_C_CHAR, SQL_CHAR,
                             0, 0,
                             &sF4_ID, 0, NULL);
            code = SQLExecute(varSelect);
            if (code == 0) {
                    SQLRETURN retcode = SQLFetch(varSelect);
                    if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO) {
                        std::cout << "Error!" << std::endl;
                    }
                    if (retcode == 100) {
                        std::cout << "No data available." << std::endl;
                    }
                    if (retcode == SQL_SUCCESS ||
                        retcode == SQL_SUCCESS_WITH_INFO) {
                            SQLGetData(varSelect, 1, SQL_C_CHAR, idEquipment, 50, &sbf1);
                            std::cout << "ID of equipment: " << idEquipment << endl;
                            int countOfElements = elems.size();
                            for(int i = 0; i<countOfElements; i++) {
                                std::string elementVector = elems.back();
                                elems.pop_back();
                                SQLCHAR* idService = (SQLCHAR *)elementVector.c_str();
                                SQLPrepare(varInsert,
                                           (SQLCHAR *) "INSERT INTO hardware_services_table(id_service, id_hardware) VALUES(?, ?);",
                                           SQL_NTS);
                                SQLBindParameter(varInsert, 1, SQL_PARAM_INPUT,
                                                 SQL_C_CHAR, SQL_CHAR,
                                                 0, 0,
                                                 idService, 0, NULL);
                                SQLBindParameter(varInsert, 2, SQL_PARAM_INPUT,
                                                 SQL_C_CHAR, SQL_CHAR,
                                                 0, 0,
                                                 idEquipment, 0, NULL);
                                retcode = SQLExecute(varInsert);
                                if (retcode == 0) {
                                    std::cout << "{INSERTTOTABLE} - Successfull insert to hardware_services_table!"
                                              << std::endl;
                                } else {
                                    checkAnswer(retcode);
                                    checkDescriptor(outputEnv, hdbcOutput, varInsert);
                                }
                            }
                    }
            }
            else {
                checkAnswer(retcode);
                checkDescriptor(outputEnv, hdbcOutput, varSelect);
            }
            std::cout << "{INSERTTOTABLE} - Successfull insert!" << std::endl;
        }
        else {
            std::cout << "Error in Execute {FUNCTION:INSERTTOTABLE}! " << "Retcode of Error: " << retcode << std::endl;
            checkAnswer(retcode);
            checkDescriptor(outputEnv, hdbcOutput, varInsert);
        }
    }
    return 0;
}

int deleteFromTable() { // Удаление данных из таблицы
    SQLRETURN retcode;
    SQLCHAR first[50];
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbcOutput, &varDelete);
    checkAnswer(retcode);
    cout << "Input ID of Equipment for delete: ";
    cin >> first;
    SQLPrepare(varDelete, deletetxt, SQL_NTS);
    SQLBindParameter(varDelete, 1, SQL_PARAM_INPUT,
                               SQL_C_CHAR, SQL_CHAR,
                               0, 0,
                               first, 0, NULL);
    retcode = SQLExecute(varDelete);
    if (retcode == 0) {
        std::cout << "{DELETEFROMTABLE} - Successfull deleted!" << std::endl;
    }
    else {
        std::cout << "Error in ExecDirect {FUNCTION:DELETEFROMTABLE}! " << "Retcode of Error: " << retcode << std::endl;
        checkAnswer(retcode);
        checkDescriptor(outputEnv, hdbcOutput, varDelete);
    }
    return 0;
}

int Disconnect() { // Отсоединение
    int retcode;
    std::cout << "Disconnecting..." << std::endl;
    std::cout << "Delete descriptors..." << std::endl;
    retcode = SQLFreeHandle(SQL_HANDLE_STMT, varSelect);
    retcode = SQLDisconnect(hdbcOutput);
    retcode = SQLFreeHandle(SQL_HANDLE_DBC, hdbcOutput);
    retcode = SQLFreeHandle(SQL_HANDLE_ENV, outputEnv);
    checkAnswer(retcode);
    return 0;
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");
    char* userName;
    SQLCHAR password[] = "";
    std::cout << "Connect to database 'lab1sysadm': " << std::endl;
    std::cout << "Username: ";
    cin >> userName;
    std::cout << "Password: ";
    cin >> password;
    int i;
    int retcode = initConnection(userName, password);
    if (retcode == 0) {
        checkTables();
        std::cout << "Operations:" << std::endl;
        std::cout << "1. Update " << std::endl;
        std::cout << "2. Insert into table " << std::endl;
        std::cout << "3. Delete from table " << std::endl;
        std::cout << "4. Check table" << std::endl;
        std::cout << "5. Search by type" << std::endl;
        std::cout << "6. Search by IP-Address" << std::endl;
        std::cout << "7. Search by MAC-address" << std::endl;
        std::cout << "100. Exit" << std::endl;
        bool flag = true;
        while (flag) {
            cin >> i;
            switch (i) {
                case 1: {
                    Update();
                    break;
                }
                case 2: {
                    insertToTable();
                    break;
                }
                case 3: {
                    deleteFromTable();
                    break;
                }
                case 4: {
                    SQLSCHAR nul[]="";
                    getData(1, nul);
                    break;
                }
                case 5: {
                    SQLSCHAR type[]="";
                    cout << "Enter Type of Equipment: ";
                    cin >> type;
                    getData(2, type);
                    break;
                }
                case 6: {
                    SQLSCHAR ipaddress[]="";
                    cout << "Enter IP-Address: ";
                    cin >> ipaddress;
                    getData(3, ipaddress);
                    break;
                }
                case 7: {
                    SQLSCHAR macaddress[]="";
                    cout << "Enter MAC-Address: ";
                    cin >> macaddress;
                    getData(4, macaddress);
                    break;
                }
                case 100: {
                    flag = false;
                    Disconnect();
                    break;
                }
            }
        }
    }
    else {
        std::cout << "Failed to connect to DB!" << std::endl;
    }
    return 0;
}