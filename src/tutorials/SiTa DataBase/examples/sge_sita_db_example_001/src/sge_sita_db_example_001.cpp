#include <sge_sita_db.h>

namespace sge {

class SiTaDBExample001 : public ConsoleApp {
protected:
	virtual void onRun() override {

		{ // set working dir
			auto exeFilePath = getExecutableFilename();
			String workingDir = FilePath::dirname(exeFilePath);
			workingDir.append("/../../../../../../examples/Test105");

			Directory::setCurrent(workingDir);
			auto curDir = Directory::getCurrent();
			SGE_LOG("current dir={}", curDir);
		}

		_sql = std::move(connectMySQL("localhost", "test_db", "test_user", "1234"));
		_sql->directExec("INSERT INTO `test_db`.`test_table` (`name`, `count`, `weight`) VALUES ('ddd', '444', '4.44');");
	}
private:
	UPtr<Conn> _sql;
};

}

int main(int argc, const char** argv) {
	sge::SiTaDBExample001 app;
	app.run();

	return 0;
}
