#ifndef __mb_db_sou__
#define __mb_db_sou__

//#include "../mb_db/mb_db_cmn.h"


// -----------------------------------------
//��������� ������������� ����������
// �������� = a* adc_val + b , ��� adc_val - �������� � �������� ���
typedef  struct
{
	float a;	//
	float b;    //
} TCalCoeffs;

// -----------------------------------------
// ************* ��������� ������������� �������� ��� ***********************

typedef  struct
{
	TCalCoeffs CurrentIn;		//������������ ���������� ���1
	TCalCoeffs VoltageIn;   //������������ ���������� ���2
	TCalCoeffs CurrentOut;		//������������ ���������� ������� ��������
	TCalCoeffs VoltageOut;   //������������ ���������� ������� ��������
	unsigned short _crc; //����������� �����
	unsigned short reseve_calib; //Even for 32 bits
} TCALIB_ADC;

#define SIZE_CALIB sizeof(TCALIB_ADC)

//--------- Statistics
typedef struct
{
  unsigned long tx_cnt;       // ���������� ���������� �������
  unsigned long rx_cnt;       // ���������� �������� �������
  unsigned long request_cnt;  // ���������� ��������, ���������� �������
  unsigned long comm_err_cnt; // ���������� ���������������� ������, ��������� ��� ������ (������, �������� � �������� � ��������� - ������ ������������� �����, ������ ����������� ����� � �.�)
  unsigned long reply_cnt;    // ���������� �������, ������������ �������
} TStatisticCPU;
//---------------------------------------------------------------------------





//�����-��������� �� ���-----------------------------------------------------------------
typedef  struct tagTSOU_STATE
{
	unsigned short reserved_0: 1; //������ (�������� ����� ���, 0 - ������, 1 - ������)
	unsigned short reserved_1: 1; //������ ( ������ �� ���, 0 - �������� �������, 1 - ���)
	unsigned short reserved_2: 1; //������ (����� ���, 0 - �����, 1 - ����� ���)
	unsigned short link_GPS: 1; //����� � GPS-������� (0 - ���� �����, 1 - ��� �����)
	unsigned short link_satellite: 1; //����� GPS-������ �� ��������� (0 - ���� �����, 1 - ��� �����)
	unsigned short flash_error: 1; //���� ������ ���� (0 - ���, 1 -����)
	unsigned short ADC1_value_error: 1; //������ ������� ��������1 (0 - ���, 1 -����)
	unsigned short ADC2_value_error: 1; //������ ������� ��������2 (0 - ���, 1 -����)
	unsigned short satellite_time: 1; //���� �� ���� ��� �������� ����� � GPS �� �������� (1 - ����, 0 - �� ����)
	unsigned short signal_GPS: 1; //������� ������� �� GPS-������ (0 - ���, 1 - e���)
	unsigned short signal_PPS: 1; //������� ������� PPS �� GPS-������ (0 - ���, 1 - e���)
	unsigned short unsync_PPS: 1; //������� ���������������� � PPS �������� GPS-������ (0 - ���, 1 - e���)
	unsigned short eeprom_err: 1; //������ ������������� �������� �� ���� (0 - ���, 1 - ����)
	unsigned short reserved_: 1; //������
	unsigned short program_state: 1; //1 - ���������� ��� ������� ��������� � DSP-���������
	unsigned short file_system_state: 1; //1 - �������� ������� ������� ����������������
}TSOU_STATE;


//��� �������� ���-----------------------------------------------------------------
typedef  struct TFirmware_Type
{
	unsigned short flash_mode: 1; //����� ������ � ���� (0 - ������ ������ �� ����, 1 - ������������ ������ � ����)
	unsigned short reserved: 14; //1 - �������� ������� ������� ����������������
} TFirmware_Type;


//��������� ��� �������� ������ ������ �� ����
typedef struct
{
	unsigned short busy;
	unsigned short other;
}TFLASH_ERROR;

#pragma pack(2)
typedef  struct
{
	unsigned short Win_Size; //������ ���� ��� ����������� �������� ��������� ��������
	unsigned short small_Win_Size; //������ ������ ����, �� �������� ��������� ���������
	float Threshold_Wave; //����� �������� ��������� �������� ��� ����������� ������, ���/�
	float Threshold_Front; //����� ��������� ��������� �������� ��� ������ �����, ���/c^2
	unsigned long Max_Front_Distence; //����������� ��������� ���������� ����� �������� �����, ��
} TALGORITHMIC_PARAMS;
#pragma pack()


typedef  struct
{
  unsigned short  slave[6];
} SLAVES;
#pragma pack()



#pragma pack(2)
//���c���� ���� MODBUS-----------------------------------------------------------------------
typedef  struct tagMODBUS_IR_SOU
{

	signed long value_ADC_1; //������ ���1
	float   Convert_ADC; //ADC converted with calib coefficients
	signed long BoardStatus; //������ ���2
	unsigned long RESTART_CNT;
        SLAVES slave2;
	float Temperature_CTRL; //����������� ����� ���
        TCALIB_ADC   CalibDacAdc;

//	TSOU_STATE SOU_State;  //�����-��������� �� ���

	//����� ������� �� GPS--------------------
		//GPS ������ ���� ��������������� ���� �� �������� ���������� GPRMC (���� � �����)
	unsigned short Year; //���
	unsigned short Month; //�����
	unsigned short Day; //���� ������
	unsigned short Hours; //����
	unsigned short Minutes; //������
	unsigned short Seconds; //�������
	//-----------------------------------------

	//��������-----------------------------------
	unsigned short Pressure_ARM_1; //��������, ����� 1 ����������� ��� ���
	unsigned short Pressure_ARM_2; //������: ��������, ����� 2  ����������� ��� ���
	float Pressure_1; //�������� ������ 1
	float Pressure_2; //�������� ������ 2
	//------------------------------------------------------------

	//������ ���-----------------------------------------------------
//	unsigned long value_ADC_1; //������ ���1
//	unsigned long value_ADC_2; //������ ���2
	float I_ADC_1; //��� ���1
	float I_ADC_2; //Moving Average ADC_1
	unsigned short reserved_Channel_ADC_8; //������ (��� ������� ���)
	unsigned short reserved_Channel_ADC_9; //������ (��� ������� ���)
	//---------------------------------------------------------------

	//------------------------------------------------
	float speed_1; //�������� ���. ��������
	float speed_2; //�������� ���. ��������

	TFLASH_ERROR Flash_Error_Count; //���-�� � ���� ������ ������ �� ����
	//------------------------------------------------------------------


	//���� ���� ����, ������� ������������ � ��� � �� ������������ ���������, �� �� ������������ � ����������� ������������ (���� ����, �������������� �� ����� ����������������
	TStatisticCPU      StatisticCPU;    // ���������� �� �����������

	unsigned short version; // ������ ��

	unsigned short gps_count_error; //���-�� ������� ����� �������� GPS
	unsigned short gps_link_error_count; //���-�� ������ �� ����� � GPS
	unsigned short gps_sync_error_count; //���-�� ������ �� ������������� � GPS
	unsigned short pps_count_error; //���-�� ������� ����� �������� PPS
	unsigned short pps_sync_error_count; //���-�� ������ �� ������������� � PPS

	//����� ������� GPS ��� ������--------------------
	unsigned short data_Year; //0 ��� ���
	unsigned short data_Month_Week; //������ ��� �����
	unsigned short data_Day; //���� ������ ��� ������
	unsigned short data_Hours; //����
	unsigned short data_Minutes; //������
	unsigned short data_Seconds; //�������
	//-----------------------------------------

	TFirmware_Type type_firmware; //��� �������� �����������

	unsigned short Frequency_ADC; //����������� ������� ���
	unsigned long Error_Buf_ADC; //���-�� ������ ������������ ������ ���
	unsigned long Error_Time_ADC; //���-�� ������ ������� ��������� ���
	unsigned long Error_ADC_Sync; //���-�� ������ �� ���������� ������������ ���

	TALGORITHMIC_PARAMS AParams; //��������� ��������� ����������� ������
} CMODBUS_IR_SOU;
#pragma pack()


// -----------------------------------------
#pragma pack(2)
typedef  struct tagMODBUS_HR_SOU
{
  	unsigned short mode_DAC; //������ (������ ���) (������� ����� ������ ����� (����� ������������ �� ������ ��������))
  	unsigned short value_DAC; //������ (������ ���) (������� ����� ������ ����� (����� ������������ �� ������ ��������))
  	unsigned short channel_ADC; //������ (������ ���) (������� ����� ������ ����� (����� ������������ �� ������ ��������))
  	unsigned short command1; //������ (������ ���) (������� ����� ������ ����� (����� ������������ �� ������ ��������))
  	unsigned short etalon_value; //������ (������ ���) (������� ����� ������ ����� (����� ������������ �� ������ ��������))
  	unsigned short cmnd_calib; //������ (������ ���) (������� ����� ������ ����� (����� ������������ �� ������ ��������))

unsigned short TEST_OUT_IKZ_H;
unsigned short TEST_IN_U_H;
unsigned short TEST_IN_UXX_H;
unsigned short CTRL_SEN_H;
unsigned short TEST_OUT_H;
unsigned short TEST_OUT_I_H;
unsigned short TEST_IN_I_H;
unsigned short CLEAR_ISO_H;
unsigned short CLEAR_SEL_H;
unsigned short CURR_IN_MODE_H;
unsigned short DUPLEX;
unsigned short RTS_ISO_1;

  	unsigned short rec_0; //������� ����
  	unsigned short rec_1; //������� ����
  	unsigned short rec_2; //������� ����
  	unsigned short rec_3; //������� ����
  	unsigned short rec_4; //������� ����
  	unsigned short rec_5; //������� ����
  	unsigned short rec_6; //������� ����
  	unsigned short rec_7; //������� ����


	unsigned short Front_Wave_Time; //������ (������ ���) (������� ����� ������ ����� (����� ������������ �� ������ ��������))
	unsigned short Min_Acceleration_Time; //������ (������ ���) (������� ����� �������� 2-�� ������������)
	unsigned short Year_Day_Reg_Wave; //��� - ���� ����������� ������ (������� ����� ����, ������� ���)

	unsigned short leak_cnt; //���-�� ������������ ������
	unsigned long Time_Reg_Wave; //����� �������� ����� (������ ���) (���� � GPS ������������ ���������� GPRMC, �� ����� ��������� �� ������ �����, ����� - �� ������ ������)
	unsigned short Month_Week_Reg_Wave; //������/����� (������ ���) (���� � GPS ������������ ���������� GPRMC, �� � ���� �������� ����� �����, ����� ������)
	unsigned short Direction_Wave; // ����������� ����� (������ ���)

	float Amplitude_Wave; //������ (��������� (������ ���))
	unsigned short type_frame; //��� ������ (0 - ������� �����, 1 - ������������ �����)

	unsigned short Service_Flags; //����� ��������� (������ ���)

	float min_KF_1_2; //��������� ������������ �� ����������� �� �������� ��������
	float KF_1_2; //��������� ������������ �� ����������� �� ���������� ������� ��������

	unsigned short Frame_Count; //������� ������ � ����� 0..65535, ���-�� �������������� ������
	unsigned short Frame_Act; //0 - ��������� ���������� ����������, 1 - ��������� ���������� ����������

	unsigned short reserved_FILTERS; //������

	unsigned short AISP_FLAGS; //����� AISP-������
								// ��� "1" - ������������ GPS (��� ����������� ������� �� GPS ������ ��� �� ������������� � DSP, ��� ����� ������, �������� ���� ����)
								// ���� ������������ ��� ������� �������, ��� ���������� GPS ��� ��������� ���������
	unsigned short RESET; //����� ���������� AISP, ���� �������� > 0
	unsigned short EMULEAK; //�������� ������. ��� �������� ������ �� ������� ������� ����������� ���������� ������ ��������� ��������������� ���� ����� ������� � 1.
							//� ������ �������� ��������� ������ �������� ����������� ���, ���� ������� ������������ � 0.

	unsigned long Wave_Time; //����� ������ ����� (�������� ����� �������� �����), �� � ������ �����
#ifdef BUILD_SOU
	// ���� ���� ��������� ����, ������� ������������ � ��� � �� ������������ ���������, �� �� ������������ � ����������� ������������
	unsigned short MB_adr;     // modbus-����� ����������
#endif //BUILD_SOU
} CMODBUS_HR_SOU;
#pragma pack()


//�������� DI ���
typedef  struct tagTDI_SOU
{
	unsigned short NotUsed:16;
} TDI_SOU;

//�������� CO ���
typedef  struct tagTCO_SOU
{
	unsigned short NotUsed:16;
} TCO_SOU;



//#ifdef BUILD_SOU

extern CMODBUS_IR_SOU *p_IR;
extern CMODBUS_HR_SOU *p_HR;

extern CMODBUS_IR_SOU g_IR;
extern CMODBUS_HR_SOU g_HR;

//#endif //SOU

#endif

