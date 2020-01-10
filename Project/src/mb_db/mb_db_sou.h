#ifndef __mb_db_sou__
#define __mb_db_sou__

//#include "../mb_db/mb_db_cmn.h"


// -----------------------------------------
//Структура коэффициентов калибровки
// значение = a* adc_val + b , где adc_val - значение в единицах ацп
typedef  struct
{
	float a;	//
	float b;    //
} TCalCoeffs;

// -----------------------------------------
// ************* структура калибровочных значений АЦП ***********************

typedef  struct
{
	TCalCoeffs CurrentIn;		//Коэффициенты калибровки АЦП1
	TCalCoeffs VoltageIn;   //Коэффициенты калибровки АЦП2
	TCalCoeffs CurrentOut;		//Коэффициенты калибровки датчика дваления
	TCalCoeffs VoltageOut;   //Коэффициенты калибровки датчика дваления
	unsigned short _crc; //контрольная сумма
	unsigned short reseve_calib; //Even for 32 bits
} TCALIB_ADC;

#define SIZE_CALIB sizeof(TCALIB_ADC)

//--------- Statistics
typedef struct
{
  unsigned long tx_cnt;       // Количество переданных пакетов
  unsigned long rx_cnt;       // Количество принятых пакетов
  unsigned long request_cnt;  // Количество запросов, полученных модулем
  unsigned long comm_err_cnt; // Количество коммуникационных ошибок, возникших при приеме (пакеты, принятые с ошибками в протоколе - ошибки синхронизации битов, ошибки контрольной суммы и т.д)
  unsigned long reply_cnt;    // Количество ответов, отправленных модулем
} TStatisticCPU;
//---------------------------------------------------------------------------





//Слово-состояние КП СОУ-----------------------------------------------------------------
typedef  struct tagTSOU_STATE
{
	unsigned short reserved_0: 1; //Резерв (Открытие шкафа СОУ, 0 - закрыт, 1 - открыт)
	unsigned short reserved_1: 1; //Резерв ( Работа от ИГП, 0 - основное питание, 1 - ИГП)
	unsigned short reserved_2: 1; //Резерв (Отказ ИГП, 0 - норма, 1 - отказ ИГП)
	unsigned short link_GPS: 1; //Связь с GPS-модулем (0 - есть связь, 1 - нет связи)
	unsigned short link_satellite: 1; //Связь GPS-модуля со спутником (0 - есть связь, 1 - нет связи)
	unsigned short flash_error: 1; //Флаг ошибки флеш (0 - нет, 1 -есть)
	unsigned short ADC1_value_error: 1; //Ошибка датчика давления1 (0 - нет, 1 -есть)
	unsigned short ADC2_value_error: 1; //Ошибка датчика давления2 (0 - нет, 1 -есть)
	unsigned short satellite_time: 1; //было ли хоть раз получено время с GPS по спутнику (1 - было, 0 - не было)
	unsigned short signal_GPS: 1; //наличие сигнала от GPS-модуля (0 - нет, 1 - eсть)
	unsigned short signal_PPS: 1; //наличие сигнала PPS от GPS-модуля (0 - нет, 1 - eсть)
	unsigned short unsync_PPS: 1; //наличие рассинхронизации с PPS сигналом GPS-модуле (0 - нет, 1 - eсть)
	unsigned short eeprom_err: 1; //ошибка востонавления настроек из флеш (0 - нет, 1 - есть)
	unsigned short reserved_: 1; //Резерв
	unsigned short program_state: 1; //1 - Приложение СОУ успешно загружено в DSP-процессор
	unsigned short file_system_state: 1; //1 - Файловая система успешно инициализирована
}TSOU_STATE;


//Тип прошивки СОУ-----------------------------------------------------------------
typedef  struct TFirmware_Type
{
	unsigned short flash_mode: 1; //Режим работы с флеш (0 - запись данных на флеш, 1 - проигрывание данных с флеш)
	unsigned short reserved: 14; //1 - Файловая система успешно инициализирована
} TFirmware_Type;


//структура для подсчёта ошибок записи во флеш
typedef struct
{
	unsigned short busy;
	unsigned short other;
}TFLASH_ERROR;

#pragma pack(2)
typedef  struct
{
	unsigned short Win_Size; //Размер окна для определения скорости изменения давления
	unsigned short small_Win_Size; //Размер малого окна, по которому считается ускарение
	float Threshold_Wave; //порог скорости изменения давления для обнаружения утечки, атм/с
	float Threshold_Front; //порог ускорения изменения давления для фронта волны, атм/c^2
	unsigned long Max_Front_Distence; //Максимально возможное расстояние между фронтами волны, мс
} TALGORITHMIC_PARAMS;
#pragma pack()


typedef  struct
{
  unsigned short  slave[6];
} SLAVES;
#pragma pack()



#pragma pack(2)
//Опиcание базы MODBUS-----------------------------------------------------------------------
typedef  struct tagMODBUS_IR_SOU
{

	signed long value_ADC_1; //данные АЦП1
	float   Convert_ADC; //ADC converted with calib coefficients
	signed long BoardStatus; //данные АЦП2
	unsigned long RESTART_CNT;
        SLAVES slave2;
	float Temperature_CTRL; //Температура платы СОУ
        TCALIB_ADC   CalibDacAdc;

//	TSOU_STATE SOU_State;  //слово-состояние КП СОУ

	//Метка времени от GPS--------------------
		//GPS должен быть сконфигурирован либо на передачу телеграммы GPRMC (дата и время)
	unsigned short Year; //год
	unsigned short Month; //месяц
	unsigned short Day; //день месяца
	unsigned short Hours; //часы
	unsigned short Minutes; //минуты
	unsigned short Seconds; //секунды
	//-----------------------------------------

	//давление-----------------------------------
	unsigned short Pressure_ARM_1; //Давление, канал 1 переведённое для АРМ
	unsigned short Pressure_ARM_2; //резерв: Давление, канал 2  переведённое для АРМ
	float Pressure_1; //Давление канала 1
	float Pressure_2; //Давление канала 2
	//------------------------------------------------------------

	//Данные АЦП-----------------------------------------------------
//	unsigned long value_ADC_1; //данные АЦП1
//	unsigned long value_ADC_2; //данные АЦП2
	float I_ADC_1; //ток АЦП1
	float I_ADC_2; //Moving Average ADC_1
	unsigned short reserved_Channel_ADC_8; //Резерв (для каналов АЦП)
	unsigned short reserved_Channel_ADC_9; //Резерв (для каналов АЦП)
	//---------------------------------------------------------------

	//------------------------------------------------
	float speed_1; //скорость изм. давления
	float speed_2; //скорость изм. давления

	TFLASH_ERROR Flash_Error_Count; //кол-во и типы ошибок записи во флеш
	//------------------------------------------------------------------


	//Ниже идут поля, которые используются в СОУ и ее терминальной программе, но не используются в вышестоящих контроллерах (выше поля, унаследованные от АИСПа Энергоавтоматики
	TStatisticCPU      StatisticCPU;    // Статистика по контроллеру

	unsigned short version; // версия ПО

	unsigned short gps_count_error; //кол-во обрывов связи сигналов GPS
	unsigned short gps_link_error_count; //кол-во ошибок по связи с GPS
	unsigned short gps_sync_error_count; //кол-во ошибок по синхронизации с GPS
	unsigned short pps_count_error; //кол-во обрывов связи сигналов PPS
	unsigned short pps_sync_error_count; //кол-во ошибок по синхронизации с PPS

	//Метка времени GPS для данных--------------------
	unsigned short data_Year; //0 или год
	unsigned short data_Month_Week; //неделя или месяц
	unsigned short data_Day; //день недели или месяца
	unsigned short data_Hours; //часы
	unsigned short data_Minutes; //минуты
	unsigned short data_Seconds; //секунды
	//-----------------------------------------

	TFirmware_Type type_firmware; //тип прошивки контроллера

	unsigned short Frequency_ADC; //отображение частоты АЦП
	unsigned long Error_Buf_ADC; //Кол-во ошибок переполнения буфера АЦП
	unsigned long Error_Time_ADC; //Кол-во ошибок частоты измерений АЦП
	unsigned long Error_ADC_Sync; //кол-во ошибок по очерёдности срабатывания АЦП

	TALGORITHMIC_PARAMS AParams; //параметры алгоритма обнаружения утечек
} CMODBUS_IR_SOU;
#pragma pack()


// -----------------------------------------
#pragma pack(2)
typedef  struct tagMODBUS_HR_SOU
{
  	unsigned short mode_DAC; //Резерв (данные СОУ) (среднее время начала волны (время срабатывания по порогу скорости))
  	unsigned short value_DAC; //Резерв (данные СОУ) (среднее время начала волны (время срабатывания по порогу скорости))
  	unsigned short channel_ADC; //Резерв (данные СОУ) (среднее время начала волны (время срабатывания по порогу скорости))
  	unsigned short command1; //Резерв (данные СОУ) (среднее время начала волны (время срабатывания по порогу скорости))
  	unsigned short etalon_value; //Резерв (данные СОУ) (среднее время начала волны (время срабатывания по порогу скорости))
  	unsigned short cmnd_calib; //Резерв (данные СОУ) (среднее время начала волны (время срабатывания по порогу скорости))

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

  	unsigned short rec_0; //Нулевой байт
  	unsigned short rec_1; //Нулевой байт
  	unsigned short rec_2; //Нулевой байт
  	unsigned short rec_3; //Нулевой байт
  	unsigned short rec_4; //Нулевой байт
  	unsigned short rec_5; //Нулевой байт
  	unsigned short rec_6; //Нулевой байт
  	unsigned short rec_7; //Нулевой байт


	unsigned short Front_Wave_Time; //Резерв (данные СОУ) (среднее время начала волны (время срабатывания по порогу скорости))
	unsigned short Min_Acceleration_Time; //Резерв (данные СОУ) (среднее время минимума 2-ой произоводной)
	unsigned short Year_Day_Reg_Wave; //год - день обнаружения утечки (младшее слово день, старшее год)

	unsigned short leak_cnt; //Кол-во обнаруженных утечек
	unsigned long Time_Reg_Wave; //Время фиксации волны (данные СОУ) (если в GPS используется телеграмма GPRMC, то время считается от начала суток, иначе - от начала недели)
	unsigned short Month_Week_Reg_Wave; //неделя/месяц (данные СОУ) (если в GPS используется телеграмма GPRMC, то в этом регистре будет месяц, иначе неделя)
	unsigned short Direction_Wave; // направление волны (данные СОУ)

	float Amplitude_Wave; //Резерв (амплитуда (данные СОУ))
	unsigned short type_frame; //Тип фрейма (0 - рабочий фрейм, 1 - эмуляционный фрейм)

	unsigned short Service_Flags; //Флаги алгоритма (данные СОУ)

	float min_KF_1_2; //отношение интегральной КФ расчитанной до минимума скорости
	float KF_1_2; //отношение интегральной КФ расчитанной до привышения порогоа скорости

	unsigned short Frame_Count; //Счетчик кадров в цикле 0..65535, кол-во сформированных кадров
	unsigned short Frame_Act; //0 - разрешено обновление информации, 1 - запрещено обновление информации

	unsigned short reserved_FILTERS; //Резерв

	unsigned short AISP_FLAGS; //Флаги AISP-модуля
								// Бит "1" - игнорировать GPS (без корректного времени от GPS данные АЦП не транслируются в DSP, это можно обойти, становив этот флаг)
								// Флаг пердназначен для отладки системы, при отсутствии GPS или видимости спутников
	unsigned short RESET; //Сброс контрллера AISP, если значение > 0
	unsigned short EMULEAK; //Имитация утечек. Для имитации утечек по каждому объекту вышестоящее устройство должно выставить соответствующие биты этого региста в 1.
							//В случае успешной обработки этоого регистра приложением СОУ, этот регистр сбрасывается в 0.

	unsigned long Wave_Time; //время начала волны (базовове время привязки волны), мс с начала суток
#ifdef BUILD_SOU
	// Ниже идут служебные поля, которые используются в СОУ и ее терминальной программе, но не используются в вышестоящих контроллерах
	unsigned short MB_adr;     // modbus-адрес устройства
#endif //BUILD_SOU
} CMODBUS_HR_SOU;
#pragma pack()


//Регистры DI СОУ
typedef  struct tagTDI_SOU
{
	unsigned short NotUsed:16;
} TDI_SOU;

//Регистры CO СОУ
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

