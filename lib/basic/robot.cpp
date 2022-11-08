#include <RollingRaspberry/basic/robot.h>

#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>
#include <unistd.h>

void Robot::auto_init() {

}

void Robot::auto_process() {

}

void Robot::teleop_init() {

}

void Robot::teleop_process() {

}

void Robot::disabled_init() {

}

void Robot::disabled_process() {

}

void Robot::test_init() {

}

void Robot::test_process() {

}


int main(int argc, char** argv) {
  /* Vision vision; */

  nt::NetworkTableInstance nt_inst = nt::NetworkTableInstance::GetDefault();

  std::shared_ptr<nt::NetworkTable> sd_table = nt_inst.GetTable("SmartDashboard"),
                                    fms_table = nt_inst.GetTable("FMSInfo");

  nt_inst.StartClientTeam(1511);

  while (true) {
    sd_table->PutNumber("Raspberry Pi is working :D", 1234.5);
    sleep(1);
  }

  return 0;
}
