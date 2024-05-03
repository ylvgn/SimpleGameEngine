#include <sge_sita_db.h>

namespace sge {

class SiTaDBExample001 : public ConsoleApp {
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

		_sql = std::move(connectMySQL("localhost", "test_db", "test_user", "1234"));

		{
			_sql->directExec("INSERT INTO `test_db`.`test_table` (`name`, `count`, `weight`) VALUES ('aa', '111', '1.11');");
		}

		{
			auto stmt = _sql->createStmt("INSERT INTO `test_table` (`name`, `count`, `weight`) VALUES ('bbb', '222', '22.2');");
			stmt->exec();
		}

		{
			auto stmt = _sql->createStmt("INSERT INTO `test_table` (`name`, `count`, `weight`) VALUES ('ccc', ?, ?);");
			stmt->exec(33, 3.23);
		}

		{
			auto stmt = _sql->createStmt("INSERT INTO `test_table` (`name`, `count`, `weight`) VALUES (?,?,?);");
			stmt->exec("ddd", 4444, 4.444);
		}
	}
private:
	SPtr<Conn> _sql;
};

}

int main(int argc, const char** argv) {
	sge::SiTaDBExample001 app;
	app.run();

	return 0;
}
