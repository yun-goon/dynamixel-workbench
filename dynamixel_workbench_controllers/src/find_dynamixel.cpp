#include <rclcpp/rclcpp.hpp>
#include <dynamixel_workbench_toolbox/dynamixel_workbench.h>
#include <vector>

#define BAUDRATE_NUM 7

int main(int argc, char *argv[]) 
{
  rclcpp::init(argc, argv);
  auto node = rclcpp::Node::make_shared("dynamixel_scanner");

  std::string port_name = "/dev/ttyUSB0";

  if (argc < 2)
  {
    RCLCPP_ERROR(node->get_logger(), "Please set '-port_name' arguments for connected Dynamixels");
    return 0;
  }
  else
  {
    port_name = argv[1];
  }

  DynamixelWorkbench dxl_wb;

  const char *log;
  bool result = false;

  uint8_t scanned_id[100];
  uint8_t dxl_cnt = 0;

  uint32_t baudrate[BAUDRATE_NUM] = {9600, 57600, 115200, 1000000, 2000000, 3000000, 4000000};
  uint8_t range = 253;

  uint8_t index = 0;

  while (index < BAUDRATE_NUM)
  {
    result = dxl_wb.init(port_name.c_str(), baudrate[index], &log);
    if (result == false)
    {
      RCLCPP_WARN(node->get_logger(), "%s", log);
      RCLCPP_WARN(node->get_logger(), "Failed to init");
    }
    else
    {
      RCLCPP_INFO(node->get_logger(), "Succeed to init(%d)", baudrate[index]);
    }

    dxl_cnt = 0;
    for (uint8_t num = 0; num < 100; num++) scanned_id[num] = 0;

    RCLCPP_INFO(node->get_logger(), "Wait for scanning...");
    result = dxl_wb.scan(scanned_id, &dxl_cnt, range, &log);
    if (result == false)
    {
      RCLCPP_WARN(node->get_logger(), "%s", log);
      RCLCPP_WARN(node->get_logger(), "Failed to scan");
    }
    else
    {
      RCLCPP_INFO(node->get_logger(), "Find %d Dynamixels", dxl_cnt);

      for (int cnt = 0; cnt < dxl_cnt; cnt++)
        RCLCPP_INFO(node->get_logger(), "id : %d, model name : %s", scanned_id[cnt], dxl_wb.getModelName(scanned_id[cnt]));
    }

    index++;
  }

  rclcpp::shutdown();
  return 0;
}
