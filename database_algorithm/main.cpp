#include <stdlib.h>
#include <iostream>
#include <fstream>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
using namespace std;

int main() {
	sql::Driver* driver;
	sql::Connection* con;
	sql::Statement* stmt;
	sql::PreparedStatement* pstmt;

	try {
		driver = get_driver_instance();
		con = driver->connect("tcp://localhost:3306", "root", "1357");
	}
	catch (sql::SQLException e) {
		cout << "Could not connect to server. Error message: " << e.what() << endl;
		system("pause");
		exit(1);
	}

	con->setSchema("sequencing");

	stmt = con->createStatement();
	stmt->execute("drop table if exists Pattern");
	stmt->execute("create table Pattern (pattern varchar(99));");
	delete stmt;

	ifstream file("sequence.txt");
	string seq;
	char b;
	while (file >> b)
		seq.push_back(b);
	file.close();
	
	pstmt = con->prepareStatement("insert into Pattern(pattern) VALUES(?)");

	int k {3};
	for (int i {0}; i < k; ++i) {
		int m {i};
		while (m + k <= seq.size()) {
			pstmt->setString(1, seq.substr(m, k));
			pstmt->execute();
			m += k;
		}
	}
	cout << "All patterns inserted." << endl;

	stmt = con->createStatement();
	stmt->execute("drop table if exists PatternCnt");
	stmt->execute("create table PatternCnt as select pattern, count(pattern) as repeats from Pattern group by pattern having count(*) > 1 order by repeats desc;"); 
	delete stmt;

	cout << "Repeats generated." << endl;

	delete pstmt;
	delete con;
	system("pause");
	return 0;
}
