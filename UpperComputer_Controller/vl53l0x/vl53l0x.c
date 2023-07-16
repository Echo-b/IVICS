#include "vl53l0.h"
#include "usart.h"
#include "stm32_i2c.h"
#include "delay.h"

u8 sendbuff[6] = {0, 0, 0, 0, 0, 0};

uint16_t distancebuff[4] = {0, 0, 0, 0}; // 用来存储一次读取四个距离传感器的数据

VL53L0X_Dev_t vl53l0x_dev0;
VL53L0X_Dev_t vl53l0x_dev1;
VL53L0X_Dev_t vl53l0x_dev2;
VL53L0X_Dev_t vl53l0x_dev3;

VL53L0X_DeviceInfo_t vl53l0x_dev_info;
VL53L0X_RangingMeasurementData_t vl53l0x_data;
VL53L0X_Error vl53l0x_status;

/***************************************************************************************/

void print_pal_error(VL53L0X_Error Status)
{
    char buf[VL53L0X_MAX_STRING_LENGTH];
    VL53L0X_GetPalErrorString(Status, buf); // 得到错误码与字符串
    // printf("API Status: %i : %s\n", Status, buf);
}

void print_range_status(VL53L0X_RangingMeasurementData_t *pdata)
{
    char buf[VL53L0X_MAX_STRING_LENGTH];
    uint8_t RangeStatus;
    // New Range Status: data is valid when pdata->RangeStatus = 0
    RangeStatus = pdata->RangeStatus;

    VL53L0X_GetRangeStatusString(RangeStatus, buf);
    // printf("Range Status: %i : %s\n", RangeStatus, buf);
}

uint16_t vl53l0x_start_single_test(VL53L0X_Dev_t *pdev,
                                   VL53L0X_RangingMeasurementData_t *pdata)
{
    int i = 0, j = 0, sum = 0;
    VL53L0X_Error status = VL53L0X_ERROR_NONE;

    if (vl53l0x_status != VL53L0X_ERROR_NONE)
        return vl53l0x_status;

    status = VL53L0X_PerformSingleRangingMeasurement(pdev, pdata); // VL53L0X执行单一测量范围
    if (status != VL53L0X_ERROR_NONE)
    {
        //  printf("error:Call of VL53L0X_PerformSingleRangingMeasurement\n");
        return status;
    }

    for (i = 0; i < 5; i++)
        sum += pdata->RangeMilliMeter;
    pdata->RangeMilliMeter = sum / 5;

    return pdata->RangeMilliMeter;
}

VL53L0X_Error vl53l0x_measure_init(VL53L0X_Dev_t *pMyDevice)
{
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;

    // FixPoint1616_t LimitCheckCurrent;
    uint32_t refSpadCount;
    uint8_t isApertureSpads;
    uint8_t VhvSettings;
    uint8_t PhaseCal;
    // Device Initialization
    Status = VL53L0X_StaticInit(pMyDevice); // 基本设备初始化
    if (Status != VL53L0X_ERROR_NONE)
    { // 判断是否初始化成功 失败打印错误信息
        // printf ("Call of VL53L0X_StaticInit\n");
        print_pal_error(Status);
        return Status;
    }
    // Device Initialization
    Status = VL53L0X_PerformRefCalibration(pMyDevice, &VhvSettings, &PhaseCal); // 执行参考校准
    if (Status != VL53L0X_ERROR_NONE)
    { // 判断是否校准成功
        //  printf ("Call of VL53L0X_PerformRefCalibration\n");
        print_pal_error(Status);
        return Status;
    }

    // needed if a coverglass is used and no calibration has been performed
    Status = VL53L0X_PerformRefSpadManagement(pMyDevice, &refSpadCount, &isApertureSpads);
    if (Status != VL53L0X_ERROR_NONE)
    {
        // printf ("Call of VL53L0X_PerformRefSpadManagement\n");
        // printf ("refSpadCount = %d, isApertureSpads = %d\n", refSpadCount, isApertureSpads);
        print_pal_error(Status);
        return Status;
    }

    // no need to do this when we use VL53L0X_PerformSingleRangingMeasurement  当我们使用VL53L0X_PerformSingleRangingMeasurement不需要
    Status = VL53L0X_SetDeviceMode(pMyDevice, VL53L0X_DEVICEMODE_SINGLE_RANGING); // Setup in single ranging mode 设置单一测量模式
    if (Status != VL53L0X_ERROR_NONE)
    {
        // printf ("Call of VL53L0X_SetDeviceMode\n");
        print_pal_error(Status);
        return Status;
    }

    // Enable/Disable Sigma and Signal check
    if (Status == VL53L0X_ERROR_NONE)
    {
        Status = VL53L0X_SetLimitCheckEnable(pMyDevice, // 启用/禁用特定限制检查
                                             VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1);
    }
    if (Status == VL53L0X_ERROR_NONE)
    {
        Status = VL53L0X_SetLimitCheckEnable(pMyDevice,
                                             VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1);
    }

    if (Status == VL53L0X_ERROR_NONE)
    {
        Status = VL53L0X_SetLimitCheckValue(pMyDevice,
                                            VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE,
                                            (FixPoint1616_t)(0.25 * 65536));
    }
    if (Status == VL53L0X_ERROR_NONE)
    {
        Status = VL53L0X_SetLimitCheckValue(pMyDevice,
                                            VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE,
                                            (FixPoint1616_t)(18 * 65536));
    }
    if (Status == VL53L0X_ERROR_NONE)
    {
        Status = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(pMyDevice, // 设置微秒时间
                                                                200000);
    }
    if (Status != VL53L0X_ERROR_NONE)
    {
        // printf ("Sigma and Signal check error\n");
        print_pal_error(Status);
        return Status;
    }
    return Status; // 返回0
}

// 配置VL53L0X设备I2C地址
// dev:设备I2C参数结构体
// newaddr:设备新I2C地址
VL53L0X_Error vl53l0x_Addr_set(VL53L0X_Dev_t *dev, uint8_t newaddr)
{
    uint16_t Id;
    uint8_t FinalAddress;
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    u8 sta = 0x00;

    FinalAddress = newaddr;

    if (FinalAddress == dev->I2cDevAddr) // 新设备I2C地址与旧地址一致,直接退出
        return VL53L0X_ERROR_NONE;
    // 在进行第一个寄存器访问之前设置I2C标准模式(400Khz)
    Status = VL53L0X_WrByte(dev, 0x88, 0x00);
    if (Status != VL53L0X_ERROR_NONE)
    {
        sta = 0x01; // 设置I2C标准模式出错
        goto set_error;
    }
    // 尝试使用默认的0x52地址读取一个寄存器
    Status = VL53L0X_RdWord(dev, VL53L0X_REG_IDENTIFICATION_MODEL_ID, &Id);
    if (Status != VL53L0X_ERROR_NONE)
    {
        sta = 0x02; // 读取寄存器出错
        goto set_error;
    }
    if (Id == 0xEEAA)
    {
        // 设置设备新的I2C地址
        Status = VL53L0X_SetDeviceAddress(dev, FinalAddress);
        if (Status != VL53L0X_ERROR_NONE)
        {
            sta = 0x03; // 设置I2C地址出错
            goto set_error;
        }
        // 修改参数结构体的I2C地址
        dev->I2cDevAddr = FinalAddress;
        // 检查新的I2C地址读写是否正常
        Status = VL53L0X_RdWord(dev, VL53L0X_REG_IDENTIFICATION_MODEL_ID, &Id);
        if (Status != VL53L0X_ERROR_NONE)
        {
            sta = 0x04; // 新I2C地址读写出错
            goto set_error;
        }
    }
set_error:
    if (Status != VL53L0X_ERROR_NONE)
    {
        print_pal_error(Status); // 打印错误信息
    }
    if (sta != 0)
        // printf("sta:0x%x\r\n",sta);
        return Status;
}

// 单个VL53L0初始化
VL53L0X_Error vl53l0x_initX(VL53L0X_Dev_t *pMyDevice, u8 vl53l0_x_id)
{
    VL53L0X_Error Status = VL53L0X_ERROR_NONE; // 初始值赋值为0

    pMyDevice->I2cDevAddr = 0x52;     // iic地址  0x52是默认地址,要初始化必须先写0x52,才能初始化，之后再通过软件修改
    pMyDevice->comms_type = 1;        // 选择IIC还是SPI    iic=1；SPI=0
    pMyDevice->comms_speed_khz = 400; // iic速率

    // 正点原子的VL53L0用户手册上写明了再次使能时地址会恢复为0x52,所以只能使能一次，设置好地址即可，这里是核心
    switch (vl53l0_x_id)
    {
    case 0:
        I2C_X0_HIGH();
        delay_ms(20);
        vl53l0x_Addr_set(pMyDevice, 0x54); // 设置第一个VL53L0X传感器I2C地址
        break;
    case 1:
        I2C_X1_HIGH();
        delay_ms(20);
        vl53l0x_Addr_set(pMyDevice, 0x56); // 设置第一个VL53L0X传感器I2C地址
        break;
    case 2:
        I2C_X2_HIGH();
        delay_ms(20);
        vl53l0x_Addr_set(pMyDevice, 0x58);
        break;
    case 3:
        I2C_X3_HIGH();
        delay_ms(20);
        vl53l0x_Addr_set(pMyDevice, 0x5a);
        break;
    }

    Status = VL53L0X_DataInit(pMyDevice); // Data initialization  //VL53L0X_DataInit：一次设备的初始化，初始化成功返回0
    if (Status != VL53L0X_ERROR_NONE)
    { // 判断如果状态不为0   打印错误信息
        print_pal_error(Status);
        return Status; //  返回错误值 可通过此值DEBUG查找错误位置
    }

    Status = VL53L0X_GetDeviceInfo(pMyDevice, &vl53l0x_dev_info); // 读取给定设备的设备信息
    if (Status != VL53L0X_ERROR_NONE)
    {
        print_pal_error(Status);
        return Status;
    }
    //    printf("VL53L0X_GetDeviceInfo:\n");
    //    printf("Device Name : %s\n", vl53l0x_dev_info.Name);     //设备名
    //    printf("Device Type : %s\n", vl53l0x_dev_info.Type);    //产品类型VL53L0X = 1, VL53L1 = 2
    //    printf("Device ID : %s\n", vl53l0x_dev_info.ProductId);   // 设备ID
    //    printf("ProductRevisionMajor : %d\n", vl53l0x_dev_info.ProductRevisionMajor);
    //    printf("ProductRevisionMinor : %d\n", vl53l0x_dev_info.ProductRevisionMinor);

    if ((vl53l0x_dev_info.ProductRevisionMajor != 1) && (vl53l0x_dev_info.ProductRevisionMinor != 1))
    {
        //  printf("Error expected cut 1.1 but found cut %d.%d\n",
        //  vl53l0x_dev_info.ProductRevisionMajor, vl53l0x_dev_info.ProductRevisionMinor);
        Status = VL53L0X_ERROR_NOT_SUPPORTED;
        print_pal_error(Status);
        return Status;
    }

    Status = vl53l0x_measure_init(pMyDevice); // 测量配置
    vl53l0x_status = Status;
    if (Status != VL53L0X_ERROR_NONE)
    { // 判断如果不为0打印错误信息
        print_pal_error(Status);
        return Status;
    }
}

VL53L0X_Error vl53l0x_init(void)
{

    VL53L0X_Error Status = VL53L0X_ERROR_NONE; // 初始值赋值为0

    // 初始化一定按照这个顺序执行，否则不成功
    VL53L0X_i2c_init();
    vl53l0x_initX(&vl53l0x_dev0, 0);
    vl53l0x_initX(&vl53l0x_dev1, 1);
    vl53l0x_initX(&vl53l0x_dev2, 2);
    vl53l0x_initX(&vl53l0x_dev3, 3);

    return Status; // 返回0
}

void vl53l0x_getdistance()
{
    distancebuff[0] = vl53l0x_start_single_test(&vl53l0x_dev0, &vl53l0x_data);
    distancebuff[1] = vl53l0x_start_single_test(&vl53l0x_dev1, &vl53l0x_data);
    distancebuff[2] = vl53l0x_start_single_test(&vl53l0x_dev2, &vl53l0x_data);
    distancebuff[3] = vl53l0x_start_single_test(&vl53l0x_dev3, &vl53l0x_data);
}
