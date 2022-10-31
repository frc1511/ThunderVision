#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>
#include <unistd.h>

int main(int argc, char** argv) {
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
