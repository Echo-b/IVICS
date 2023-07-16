#include "vl53l0.h"
#include "usart.h"
#include "stm32_i2c.h"
#include "delay.h"

u8 sendbuff[6] = {0, 0, 0, 0, 0, 0};

uint16_t distancebuff[4] = {0, 0, 0, 0}; // �����洢һ�ζ�ȡ�ĸ����봫����������

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
    VL53L0X_GetPalErrorString(Status, buf); // �õ����������ַ���
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

    status = VL53L0X_PerformSingleRangingMeasurement(pdev, pdata); // VL53L0Xִ�е�һ������Χ
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
    Status = VL53L0X_StaticInit(pMyDevice); // �����豸��ʼ��
    if (Status != VL53L0X_ERROR_NONE)
    { // �ж��Ƿ��ʼ���ɹ� ʧ�ܴ�ӡ������Ϣ
        // printf ("Call of VL53L0X_StaticInit\n");
        print_pal_error(Status);
        return Status;
    }
    // Device Initialization
    Status = VL53L0X_PerformRefCalibration(pMyDevice, &VhvSettings, &PhaseCal); // ִ�вο�У׼
    if (Status != VL53L0X_ERROR_NONE)
    { // �ж��Ƿ�У׼�ɹ�
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

    // no need to do this when we use VL53L0X_PerformSingleRangingMeasurement  ������ʹ��VL53L0X_PerformSingleRangingMeasurement����Ҫ
    Status = VL53L0X_SetDeviceMode(pMyDevice, VL53L0X_DEVICEMODE_SINGLE_RANGING); // Setup in single ranging mode ���õ�һ����ģʽ
    if (Status != VL53L0X_ERROR_NONE)
    {
        // printf ("Call of VL53L0X_SetDeviceMode\n");
        print_pal_error(Status);
        return Status;
    }

    // Enable/Disable Sigma and Signal check
    if (Status == VL53L0X_ERROR_NONE)
    {
        Status = VL53L0X_SetLimitCheckEnable(pMyDevice, // ����/�����ض����Ƽ��
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
        Status = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(pMyDevice, // ����΢��ʱ��
                                                                200000);
    }
    if (Status != VL53L0X_ERROR_NONE)
    {
        // printf ("Sigma and Signal check error\n");
        print_pal_error(Status);
        return Status;
    }
    return Status; // ����0
}

// ����VL53L0X�豸I2C��ַ
// dev:�豸I2C�����ṹ��
// newaddr:�豸��I2C��ַ
VL53L0X_Error vl53l0x_Addr_set(VL53L0X_Dev_t *dev, uint8_t newaddr)
{
    uint16_t Id;
    uint8_t FinalAddress;
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    u8 sta = 0x00;

    FinalAddress = newaddr;

    if (FinalAddress == dev->I2cDevAddr) // ���豸I2C��ַ��ɵ�ַһ��,ֱ���˳�
        return VL53L0X_ERROR_NONE;
    // �ڽ��е�һ���Ĵ�������֮ǰ����I2C��׼ģʽ(400Khz)
    Status = VL53L0X_WrByte(dev, 0x88, 0x00);
    if (Status != VL53L0X_ERROR_NONE)
    {
        sta = 0x01; // ����I2C��׼ģʽ����
        goto set_error;
    }
    // ����ʹ��Ĭ�ϵ�0x52��ַ��ȡһ���Ĵ���
    Status = VL53L0X_RdWord(dev, VL53L0X_REG_IDENTIFICATION_MODEL_ID, &Id);
    if (Status != VL53L0X_ERROR_NONE)
    {
        sta = 0x02; // ��ȡ�Ĵ�������
        goto set_error;
    }
    if (Id == 0xEEAA)
    {
        // �����豸�µ�I2C��ַ
        Status = VL53L0X_SetDeviceAddress(dev, FinalAddress);
        if (Status != VL53L0X_ERROR_NONE)
        {
            sta = 0x03; // ����I2C��ַ����
            goto set_error;
        }
        // �޸Ĳ����ṹ���I2C��ַ
        dev->I2cDevAddr = FinalAddress;
        // ����µ�I2C��ַ��д�Ƿ�����
        Status = VL53L0X_RdWord(dev, VL53L0X_REG_IDENTIFICATION_MODEL_ID, &Id);
        if (Status != VL53L0X_ERROR_NONE)
        {
            sta = 0x04; // ��I2C��ַ��д����
            goto set_error;
        }
    }
set_error:
    if (Status != VL53L0X_ERROR_NONE)
    {
        print_pal_error(Status); // ��ӡ������Ϣ
    }
    if (sta != 0)
        // printf("sta:0x%x\r\n",sta);
        return Status;
}

// ����VL53L0��ʼ��
VL53L0X_Error vl53l0x_initX(VL53L0X_Dev_t *pMyDevice, u8 vl53l0_x_id)
{
    VL53L0X_Error Status = VL53L0X_ERROR_NONE; // ��ʼֵ��ֵΪ0

    pMyDevice->I2cDevAddr = 0x52;     // iic��ַ  0x52��Ĭ�ϵ�ַ,Ҫ��ʼ��������д0x52,���ܳ�ʼ����֮����ͨ������޸�
    pMyDevice->comms_type = 1;        // ѡ��IIC����SPI    iic=1��SPI=0
    pMyDevice->comms_speed_khz = 400; // iic����

    // ����ԭ�ӵ�VL53L0�û��ֲ���д�����ٴ�ʹ��ʱ��ַ��ָ�Ϊ0x52,����ֻ��ʹ��һ�Σ����úõ�ַ���ɣ������Ǻ���
    switch (vl53l0_x_id)
    {
    case 0:
        I2C_X0_HIGH();
        delay_ms(20);
        vl53l0x_Addr_set(pMyDevice, 0x54); // ���õ�һ��VL53L0X������I2C��ַ
        break;
    case 1:
        I2C_X1_HIGH();
        delay_ms(20);
        vl53l0x_Addr_set(pMyDevice, 0x56); // ���õ�һ��VL53L0X������I2C��ַ
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

    Status = VL53L0X_DataInit(pMyDevice); // Data initialization  //VL53L0X_DataInit��һ���豸�ĳ�ʼ������ʼ���ɹ�����0
    if (Status != VL53L0X_ERROR_NONE)
    { // �ж����״̬��Ϊ0   ��ӡ������Ϣ
        print_pal_error(Status);
        return Status; //  ���ش���ֵ ��ͨ����ֵDEBUG���Ҵ���λ��
    }

    Status = VL53L0X_GetDeviceInfo(pMyDevice, &vl53l0x_dev_info); // ��ȡ�����豸���豸��Ϣ
    if (Status != VL53L0X_ERROR_NONE)
    {
        print_pal_error(Status);
        return Status;
    }
    //    printf("VL53L0X_GetDeviceInfo:\n");
    //    printf("Device Name : %s\n", vl53l0x_dev_info.Name);     //�豸��
    //    printf("Device Type : %s\n", vl53l0x_dev_info.Type);    //��Ʒ����VL53L0X = 1, VL53L1 = 2
    //    printf("Device ID : %s\n", vl53l0x_dev_info.ProductId);   // �豸ID
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

    Status = vl53l0x_measure_init(pMyDevice); // ��������
    vl53l0x_status = Status;
    if (Status != VL53L0X_ERROR_NONE)
    { // �ж������Ϊ0��ӡ������Ϣ
        print_pal_error(Status);
        return Status;
    }
}

VL53L0X_Error vl53l0x_init(void)
{

    VL53L0X_Error Status = VL53L0X_ERROR_NONE; // ��ʼֵ��ֵΪ0

    // ��ʼ��һ���������˳��ִ�У����򲻳ɹ�
    VL53L0X_i2c_init();
    vl53l0x_initX(&vl53l0x_dev0, 0);
    vl53l0x_initX(&vl53l0x_dev1, 1);
    vl53l0x_initX(&vl53l0x_dev2, 2);
    vl53l0x_initX(&vl53l0x_dev3, 3);

    return Status; // ����0
}

void vl53l0x_getdistance()
{
    distancebuff[0] = vl53l0x_start_single_test(&vl53l0x_dev0, &vl53l0x_data);
    distancebuff[1] = vl53l0x_start_single_test(&vl53l0x_dev1, &vl53l0x_data);
    distancebuff[2] = vl53l0x_start_single_test(&vl53l0x_dev2, &vl53l0x_data);
    distancebuff[3] = vl53l0x_start_single_test(&vl53l0x_dev3, &vl53l0x_data);
}
