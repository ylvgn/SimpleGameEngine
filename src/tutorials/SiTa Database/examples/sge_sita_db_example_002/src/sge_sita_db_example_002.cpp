#include <sge_sita_db.h>

namespace sge {

struct Row {
	int			id;
	std::string name;
	int			count;
	double		weight;

	void print() {
		SGE_DUMP_VAR(id, name.c_str(), count, weight);
	}
};

class SiTaDBExample002 : public ConsoleApp {
protected:
	virtual void onRun() override {

		{ // set working dir
			auto exeFilePath = getExecutableFilename();
			String workingDir = FilePath::dirname(exeFilePath);
			workingDir.append("/../../../../../../../../examples/Test105");

			Directory::setCurrent(workingDir);
			auto curDir = Directory::getCurrent();
			SGE_LOG("current dir={}", curDir);
		}

		_conn = std::move(connectSQLite3("test_db.sqlite3"));

//		_writeData();
		_fetchData();
	}
private:

	void _writeData() {
		{
			_conn->directExec("INSERT INTO `test_table` (`name`, `count`, `weight`) VALUES ('aaa', '111', '1.11');");
		}

		{
			auto stmt = _conn->createStmt("INSERT INTO `test_table` (`name`, `count`, `weight`) VALUES ('bbb', '222', '22.2');");
			stmt->exec();
		}

		{
			auto stmt = _conn->createStmt("INSERT INTO `test_table` (`name`, `count`, `weight`) VALUES ('ccc', ?, ?);");
			stmt->exec(33, 3.23);
		}

		{
			auto stmt = _conn->createStmt("INSERT INTO `test_table` (`name`, `count`, `weight`) VALUES (?,?,?);");
			stmt->exec("ddd", 4444, 4.444);
		}
	}

	void _fetchData() {

		{
			auto stmt = _conn->createStmt("select * from test_table;");
			stmt->exec();
			int n = stmt->resultFieldCount();
			SGE_DUMP_VAR(n);
			for (int i = 0; i < n; ++i) {
				const char* name = stmt->resultFieldName(i);
				SGE_DUMP_VAR(i, name);
			}
			stmt->reset();
		}

		{
			Row row;
			auto stmt = _conn->createStmt("select id, count, weight from test_table;");
			stmt->exec();
			int n = stmt->resultFieldCount();
			while (stmt->fetch(row.id, /*row.name,*/ row.count, row.weight)) {
				row.print();

				for (int i = 0; i < n; i++) {
					if (stmt->isFieldNull(i)) {
						const char* name = stmt->resultFieldName(i);
						SGE_LOG("index {}: '{}' isNull", i, name);
					}
				}
			}
		}

		{
			Row row;
			auto stmt = _conn->createStmt("select id, name, count, weight from test_table;");
			stmt->exec();
			int n = stmt->resultFieldCount();
			while (stmt->fetch(row.id, row.name, row.count, row.weight)) {
				row.print();

				for (int i = 0; i < n; i++) {
					if (stmt->isFieldNull(i)) {
						const char* name = stmt->resultFieldName(i);
						SGE_LOG("index {}: '{}' isNull", i, name);
					}
				}
			}
		}

	}

	SPtr<Conn> _conn;
};

} // namespace sge


SGE_MAIN(sge::SiTaDBExample002)