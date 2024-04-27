#include <sge_sita_db.h>

namespace sge {

class SiTaDBExample001App : public ConsoleApp {
protected:
	virtual void onRun() override {
		_sql.reset(connectMySQL("localhost", "test_db", "test_user", "1234"));

		_sql->directExec("INSERT INTO `test_db`.`test_table` (`name`, `count`, `weight`) VALUES ('ddd', '444', '4.44');");
	}
private:
	UPtr<Conn> _sql;
};

}

int main(int argc, const char** argv) {
	sge::SiTaDBExample001App app;
	app.run();

	return 0;
}
