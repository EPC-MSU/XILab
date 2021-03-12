
#ifndef INC_URMC_H

#define INC_URMC_H

/** @file urmc.h
	* \~english
	* @brief Header file for urmc library
	* \~russian
	* @brief Заголовочный файл для библиотеки urmc
    *
    * Библиотека имеет два варианта компиляции - локальный и сетевой.
    *
    * Для компиляции сетевого варианта необходима библиотека bindy. Она может быть взята с GitHub:
    *       git clone --recursive https://github.com/EPC-MSU/Bindy.git -b sqlite
    * (Создаваемый GitHub zip-файл не содержит всего необходимого софта!).
    * Сетевой вариант компилируется только C++ компилятором (g++ 4.8, MS Visual Studio 2012, ...).
    * При этом будет получен код, который линкуется с программами написанными на C по правилам C.
    * При компиляции сетевого варианта под Linux необходимо подключить библиотеку dl.
    *
    * Локальный вариант может быть скомпилирован под Linux с использованием компилятора gcc 4.8 или 4.9 с включённой опцией "C-99",
    * либо компилятором g++ 4.8 или 4.9,  либо под Windows c использованием MS Visual Studio 2012.
    *
    * Сборка под Windows осуществляется для Win32.
    *
    * @include cmake.md
    *
    * Использование библиотеки:
    * - все необходимые пользователю объявления находятся в заголовочном файле urmc.h
    *
    * - под Windows при сборке библиотеки в папке проекта образуется папка Debug или Release, в которой будут файлы urmc.lib,
    * который необходимо поместить в папку дополнительных библиотек для линковки проекта использующего данную библиотеку
    * и urmc.dll, который необходимо помещать вместе с полученным .exe файлом проекта или регистрировать в системе Windows.
    *
    * - под Linux все необходимые для компиляции данные находятся в файле liburmc.so .
    * При компиляции его необходимо скопировать в директорию с исходными файлами.
    * Этот же файл используется и для запуска откомпилированных бинарников.
    * Для этого он должен быть помещён в /usr/local/lib или любое другое место, прописанное как расположение разделяемых библиотек,
    * либо путь к нему должен быть указан при вызове программы следующим образом:
    * LD_PRELOAD="[Путь к файлу liburmc.so]" [Вызов программы]
    *
    * Общие принципы работы URPC
    *
    * Все коммуникации между ПК и микроконтроллером осуществляются по принципу клиент-сервер. При этом микроконтроллер является сервером, а ПК – клиентом. Это значит, что все коммуникации инициируются компьютером.
    * Микроконтроллер не может отправить какие-либо данные на ПК самостоятельно, для этого ему требуется соответствующий запрос клиента (компьютера).
    *
    * URPC позволяет осуществлять коммуникации двух типов:
    *
    *    - Коммуникации посредством команд
    *    - Коммуникации посредством аксессоров
    *
    * Коммуникация посредством команд осуществляется следующим образом:
    *
    *    Клиент (ПК) готовит данные для отправки на сервер (данный этап не является обязательным, поскольку допускаются команды, не передающие никаких данных на сервер).
    *    Клиент (ПК) отправляет запрос на выполнение команды клиенту (микроконтроллеру). В запросе также передаются входные данные
    *    После получения запроса на сервере (микроконтроллере) запускается обработчик, в котором производится обработка входных данных, собственно выполнение команды, подготовка выходных данных. По умолчанию URPC генерирует пустой обработчик, код обработчика пишется пользователем. Сразу после завершения обработчика сервер отправляет ответ клиенту вместе с набором выходных данных.
    *    Получив ответ сервера, клиент (ПК) передаёт управление пользовательской программе, которая может приступить к обработке выходных данных, полученных с сервера (допускаются команды без выходных данных).
    *
    * Входные и выходные никак не зависят друг от друга и могут иметь формат. Единственное ограничение: суммарный объём всех входных данных, также как и суммарный объём всех выходных данных не должен превышать 250 байт.
    * Команды могут иметь входные и выходные аргументы, только входные аргументы, только выходные аргументы, а могут вообще не иметь аргументов.
    *
    * Общение посредством аксессоров.
    *
    * Аксессор – это специальная команда (а точнее set/get пара команд), предназначенная для работы с данными на сервере.
    * Отправка данных с клиента на сервер осуществляется посредством set-функции аксессора, получение данных – посредством вызова get-функции аксессора. Обе функции могут быть вызваны только клиентом (ПК).
    * Set-функция имеет только входные аргументы, get-функция – только выходные. Формат входных и выходных данных set и get функций одного аксессора должен быть одинаковым.
    * Также как и в случае команд после получения set и get запросов аксессора сервер (микроконтроллер) запускает соответствующие обработчики, содержание которых определяется пользователем.
    *
    *
   */


/**
	* \def URPC_VIS
    * \~english
	* \brief Library import macro
 *
	* Macros allows to automatically import function from shared library.
	* On Windows it automatically expands to dllimport/dllexport depending on usage scenario.
	* On Linux GCC it expands to visibility attribute.
    * \~russian
    * \brief Макрос импорта функций
    * 
    * Макрос предназначен для автоматического импорта функций из разделяемой библиотеки.
    * Под Windows он автоматически развёртывается в dllimport/dllexport в используемом сценарии.
    * Под Linux он развёртывается в атрибут visibility.
	*/
#undef URPC_VIS
#if defined(WIN32) || defined(WIN64) || defined(LABVIEW64_IMPORT) || defined(LABVIEW32_IMPORT) || defined(MATLAB_IMPORT)
	#if URPC_EXPORTS
        #define URPC_VIS __declspec(dllexport)
    #else
        #define URPC_VIS __declspec(dllimport)
    #endif
#else
	#define URPC_VIS __attribute__((visibility("default")))
#endif

#undef URPC_CALLCONV
/**
	* \def URPC_CALLCONV
    * \~english
	* \brief Library calling convention macros.
    * \~russian
    * \brief Макрос конвенций библиотечных вызовов.
	*/
#if defined(WIN32) || defined(WIN64) || defined(LABVIEW64_IMPORT) || defined(LABVIEW32_IMPORT) || defined(MATLAB_IMPORT)
	#define URPC_CALLCONV __cdecl
#else
	#define URPC_CALLCONV
#endif


#if !defined(URPC_NO_STDINT)

#if ( (defined(_MSC_VER) && (_MSC_VER < 1600)) || defined(LABVIEW64_IMPORT) || defined(LABVIEW32_IMPORT)) && !defined(MATLAB_IMPORT)
// msvc types burden
typedef __int8 int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef __int64 int64_t;
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif

/* labview doesn't speak C99 */
#if defined(LABVIEW64_IMPORT) || defined(LABVIEW32_IMPORT)
typedef unsigned __int64 ulong_t;
typedef __int64 long_t;
#else
typedef unsigned long long ulong_t;
typedef long long long_t;
#endif

#endif

#include <time.h>

#ifdef __cplusplus
extern "C"
{
#endif
/**
	* \~english
	* Type describes device identifier
	* \~russian
	* Тип идентификатора устройства
	*/
typedef int device_t;

/**
	* \~english
	* Type specifies result of any operation
	* \~russian
	* Тип, определяющий результат выполнения команды.
	*/
typedef int result_t;

/**
	* \~english
	* Handle specified undefined device
	* \~russian
	* Макрос, означающий неопределенное устройство
	*/
#define device_undefined -1

/**
	* \~english
	* @name Result statuses
	* \~russian
	* @name Результаты выполнения команд
	*/
//@{

/**
	* \~english
	* success
	* \~russian
	* выполнено успешно
	*/
#define result_ok 0

/**
	* \~english
	* generic error
	* \~russian
	* общая ошибка
	*/
#define result_error -1

/**
	* \~english
	* function is not implemented
	* \~russian
	* функция не определена
	*/
#define result_not_implemented -2

/**
	* \~english
	* value error
	* \~russian
	* ошибка записи значения
	*/
#define result_value_error -3

/**
	* \~english
	* device is lost
	* \~russian
	* устройство не подключено
	*/
#define result_nodevice -4

//@}

/**
	* \~english
	* @name Logging level
	* \~russian
	* @name Уровень логирования
	*/
//@{

/**
	* \~english
	* Logging level - error
	* \~russian
	* Уровень логирования - ошибка
	*/
#define LOGLEVEL_ERROR 		0x01
/**
	* \~english
	* Logging level - warning
	* \~russian
	* Уровень логирования - предупреждение
	*/
#define LOGLEVEL_WARNING 	0x02
/**
	* \~english
	* Logging level - info
	* \~russian
	* Уровень логирования - информация
	*/
#define LOGLEVEL_INFO		0x03
/**
	* \~english
	* Logging level - debug
	* \~russian
	* Уровень логирования - отладка
	*/
#define LOGLEVEL_DEBUG		0x04
//@}

/*
* Builder version
*/
#define BUILDER_VERSION 0.5.0-Sarov

/*
 * Protocol version
 */
#define PROTOCOL_VERSION 18.3.0
#define PROTOCOL_VERSION_Q "18.3.0"

/*
 * Command codes.
 */
#define URMC_GFBS_CMD  0x73626667
#define URMC_SFBS_CMD  0x73626673
#define URMC_GHOM_CMD  0x6D6F6867
#define URMC_SHOM_CMD  0x6D6F6873
#define URMC_GMOV_CMD  0x766F6D67
#define URMC_SMOV_CMD  0x766F6D73
#define URMC_GENG_CMD  0x676E6567
#define URMC_SENG_CMD  0x676E6573
#define URMC_GENT_CMD  0x746E6567
#define URMC_SENT_CMD  0x746E6573
#define URMC_GPWR_CMD  0x72777067
#define URMC_SPWR_CMD  0x72777073
#define URMC_GSEC_CMD  0x63657367
#define URMC_SSEC_CMD  0x63657373
#define URMC_GEDS_CMD  0x73646567
#define URMC_SEDS_CMD  0x73646573
#define URMC_GPID_CMD  0x64697067
#define URMC_SPID_CMD  0x64697073
#define URMC_GSNI_CMD  0x696E7367
#define URMC_SSNI_CMD  0x696E7373
#define URMC_GSNO_CMD  0x6F6E7367
#define URMC_SSNO_CMD  0x6F6E7373
#define URMC_GEIO_CMD  0x6F696567
#define URMC_SEIO_CMD  0x6F696573
#define URMC_GBRK_CMD  0x6B726267
#define URMC_SBRK_CMD  0x6B726273
#define URMC_GCTL_CMD  0x6C746367
#define URMC_SCTL_CMD  0x6C746373
#define URMC_GJOY_CMD  0x796F6A67
#define URMC_SJOY_CMD  0x796F6A73
#define URMC_GCTP_CMD  0x70746367
#define URMC_SCTP_CMD  0x70746373
#define URMC_GURT_CMD  0x74727567
#define URMC_SURT_CMD  0x74727573
#define URMC_GNMF_CMD  0x666D6E67
#define URMC_SNMF_CMD  0x666D6E73
#define URMC_GNVM_CMD  0x6D766E67
#define URMC_SNVM_CMD  0x6D766E73
#define URMC_STOP_CMD  0x706F7473
#define URMC_PWOF_CMD  0x666F7770
#define URMC_MOVE_CMD  0x65766F6D
#define URMC_MOVR_CMD  0x72766F6D
#define URMC_HOME_CMD  0x656D6F68
#define URMC_LEFT_CMD  0x7466656C
#define URMC_RIGT_CMD  0x74676972
#define URMC_LOFT_CMD  0x74666F6C
#define URMC_SSTP_CMD  0x70747373
#define URMC_GPOS_CMD  0x736F7067
#define URMC_SPOS_CMD  0x736F7073
#define URMC_ZERO_CMD  0x6F72657A
#define URMC_SAVE_CMD  0x65766173
#define URMC_READ_CMD  0x64616572
#define URMC_GETS_CMD  0x73746567
#define URMC_STMS_CMD  0x736D7473
#define URMC_GETM_CMD  0x6D746567
#define URMC_GETC_CMD  0x63746567
#define URMC_GETI_CMD  0x69746567
#define URMC_GSER_CMD  0x72657367
#define URMC_GFWV_CMD  0x76776667
#define URMC_RAFL_CMD  0x6C666172
#define URMC_RBLD_CMD  0x646C6272
#define URMC_UPDF_CMD  0x66647075
#define URMC_RDAN_CMD  0x6E616472
#define URMC_DBGR_CMD  0x72676264
#define URMC_DBGW_CMD  0x77676264
#define URMC_CLFR_CMD  0x72666C63
#define URMC_GNME_CMD  0x656D6E67
#define URMC_SNME_CMD  0x656D6E73
#define URMC_GSTI_CMD  0x69747367
#define URMC_SSTI_CMD  0x69747373
#define URMC_GSTS_CMD  0x73747367
#define URMC_SSTS_CMD  0x73747373
#define URMC_GMTI_CMD  0x69746D67
#define URMC_SMTI_CMD  0x69746D73
#define URMC_GMTS_CMD  0x73746D67
#define URMC_SMTS_CMD  0x73746D73
#define URMC_GENI_CMD  0x696E6567
#define URMC_SENI_CMD  0x696E6573
#define URMC_GENS_CMD  0x736E6567
#define URMC_SENS_CMD  0x736E6573
#define URMC_GHSI_CMD  0x69736867
#define URMC_SHSI_CMD  0x69736873
#define URMC_GHSS_CMD  0x73736867
#define URMC_SHSS_CMD  0x73736873
#define URMC_GGRI_CMD  0x69726767
#define URMC_SGRI_CMD  0x69726773
#define URMC_GGRS_CMD  0x73726767
#define URMC_SGRS_CMD  0x73726773
#define URMC_GACC_CMD  0x63636167
#define URMC_SACC_CMD  0x63636173
#define URMC_GBLV_CMD  0x766C6267
#define URMC_REST_CMD  0x74736572
#define URMC_CHMT_CMD  0x746D6863

/*
 * ERRC, ERRD, ERRV codes must be always
 */
#define ERRC_CMD  0x63727265
#define ERRD_CMD  0x64727265
#define ERRV_CMD  0x76727265

/*
 * All command flags below.
 */

/**
* @anchor PosFlags_flagset
*/
//@{
    #define URMC_SETPOS_IGNORE_POSITION    0x1 /**< \~english Will not reload position in steps/microsteps if this flag is set. \~russian Если установлен, то позиция в шагах и микрошагах не обновляется. */
    #define URMC_SETPOS_IGNORE_ENCODER    0x2 /**< \~english Will not reload encoder state if this flag is set. \~russian Если установлен, то счётчик энкодера не обновляется. */
//@}
/**
* @anchor MoveSts_flagset
*/
//@{
    #define URMC_MOVE_STATE_MOVING    0x1 /**< \~english This flag indicates that controller is trying to move the motor. Don't use this flag for waiting of completion of the movement command. Use MVCMD_RUNNING flag from the MvCmdSts field instead. \~russian Если флаг установлен, то контроллер пытается вращать двигателем. Не используйте этот флаг для ожидания завершения команды движения. Вместо него используйте MVCMD_RUNNING из поля MvCmdSts. */
    #define URMC_MOVE_STATE_TARGET_SPEED    0x2 /**< \~english Target speed is reached, if flag set. \~russian Флаг устанавливается при достижении заданной скорости. */
    #define URMC_MOVE_STATE_ANTIPLAY    0x4 /**< \~english Motor is playing compensation, if flag set. \~russian Выполняется компенсация люфта, если флаг установлен. */
//@}
/**
* @anchor MvCmdSts_flagset
*/
//@{
    #define URMC_MVCMD_NAME_BITS    0x3f /**< \~english Move command bit mask. \~russian Битовая маска активной команды. */
    #define URMC_MVCMD_UKNWN    0x0 /**< \~english Unknown command. \~russian Неизвестная команда. */
    #define URMC_MVCMD_MOVE    0x1 /**< \~english Command move. \~russian Команда move. */
    #define URMC_MVCMD_MOVR    0x2 /**< \~english Command movr. \~russian Команда movr. */
    #define URMC_MVCMD_LEFT    0x3 /**< \~english Command left. \~russian Команда left. */
    #define URMC_MVCMD_RIGHT    0x4 /**< \~english Command rigt. \~russian Команда rigt. */
    #define URMC_MVCMD_STOP    0x5 /**< \~english Command stop. \~russian Команда stop. */
    #define URMC_MVCMD_HOME    0x6 /**< \~english Command home. \~russian Команда home. */
    #define URMC_MVCMD_LOFT    0x7 /**< \~english Command loft. \~russian Команда loft. */
    #define URMC_MVCMD_SSTP    0x8 /**< \~english Command soft stop. \~russian Команда плавной остановки(SSTP). */
    #define URMC_MVCMD_ERROR    0x40 /**< \~english Finish state (1 - move command have finished with an error, 0 - move command have finished correctly). This flags is actual when MVCMD_RUNNING signals movement finish. \~russian Состояние завершения движения (1 - команда движения выполнена с ошибкой, 0 - команда движения выполнена корректно). Имеет смысл если MVCMD_RUNNING указывает на завершение движения. */
    #define URMC_MVCMD_RUNNING    0x80 /**< \~english Move command state (0 - move command have finished, 1 - move command is being executed). \~russian Состояние команды движения (0 - команда движения выполнена, 1 - команда движения сейчас выполняется). */
//@}
/**
* @anchor PWRSts_flagset
*/
//@{
    #define URMC_PWR_STATE_UNKNOWN    0x0 /**< \~english Unknown state, should never happen. \~russian Неизвестное состояние, которое не должно никогда реализовываться. */
    #define URMC_PWR_STATE_OFF    0x1 /**< \~english Motor windings are disconnected from the driver. \~russian Обмотки мотора разомкнуты и не управляются драйвером. */
    #define URMC_PWR_STATE_NORM    0x3 /**< \~english Motor windings are powered by nominal current. \~russian Обмотки запитаны номинальным током. */
    #define URMC_PWR_STATE_REDUCT    0x4 /**< \~english Motor windings are powered by reduced current to lower power consumption. \~russian Обмотки намеренно запитаны уменьшенным током от рабочего для снижения потребляемой мощности. */
    #define URMC_PWR_STATE_MAX    0x5 /**< \~english Motor windings are powered by maximum current driver can provide at this voltage. \~russian Обмотки запитаны максимально доступным током, который может выдать схема при данном напряжении питания. */
//@}
/**
* @anchor EncSts_flagset
*/
//@{
    #define URMC_ENC_STATE_ABSENT    0x0 /**< \~english Encoder is absent. \~russian Энкодер не подключен. */
    #define URMC_ENC_STATE_UNKNOWN    0x1 /**< \~english Encoder state is unknown. \~russian Cостояние энкодера неизвестно. */
    #define URMC_ENC_STATE_MALFUNC    0x2 /**< \~english Encoder is connected and malfunctioning. \~russian Энкодер подключен и неисправен. */
    #define URMC_ENC_STATE_REVERS    0x3 /**< \~english Encoder is connected and operational but counts in other direction. \~russian Энкодер подключен и исправен, но считает в другую сторону. */
    #define URMC_ENC_STATE_OK    0x4 /**< \~english Encoder is connected and working properly. \~russian Энкодер подключен и работает адекватно. */
//@}
/**
* @anchor Flags_flagset
*/
//@{
    #define URMC_STATE_CONTR    0x3f /**< \~english Flags of controller states. \~russian Флаги состояния контроллера. */
    #define URMC_STATE_IS_HOMED    0x20 /**< \~english Calibration performed \~russian Калибровка выполнена */
    #define URMC_STATE_SECUR    0x3ffc0 /**< \~english Flags of security. \~russian Флаги опасности. */
    #define URMC_STATE_ALARM    0x40 /**< \~english Controller is in alarm state indicating that something dangerous had happened. Most commands are ignored in this state. To reset the flag a STOP command must be issued. \~russian Контроллер находится в состоянии ALARM, показывая, что случилась какая-то опасная ситуация. В состоянии ALARM все команды игнорируются пока не будет послана команда STOP и состояние ALARM деактивируется. */
    #define URMC_STATE_CTP_ERROR    0x80 /**< \~english Control position error(is only used with stepper motor). \~russian Контроль позиции нарушен(используется только с шаговым двигателем). */
    #define URMC_STATE_POWER_OVERHEAT    0x100 /**< \~english Power driver overheat. \~russian Перегрелась силовая часть платы. */
    #define URMC_STATE_OVERLOAD_POWER_VOLTAGE    0x400 /**< \~english Power voltage exceeds safe limit. \~russian Превышено напряжение на силовой части. */
    #define URMC_STATE_WINDINGS_MALFUNC    0x4000 /**< \~english The windings are malfunc. Detection occurs by comparing current integrals over several periods. If the difference is greater than the threshold, then the decision is made to enter the Alarm state. \~russian Обмотки повреждены. Детектирование происходит путём сравнения интегралов токов за несколько периодов. Если разница больше пороговой, то принимается решение войти в состояние Alarm. */
    #define URMC_STATE_BORDERS_SWAP_MISSET    0x8000 /**< \~english Engine stuck at the wrong edge. \~russian Достижение неверной границы. */
    #define URMC_STATE_LOW_POWER_VOLTAGE    0x10000 /**< \~english Power voltage is lower than Low Voltage Protection limit \~russian Напряжение на силовой части ниже чем напряжение Low Voltage Protection */
    #define URMC_STATE_H_BRIDGE_FAULT    0x20000 /**< \~english Signal from the driver that fault happened \~russian Получен сигнал от драйвера о неисправности */
    #define URMC_STATE_CURRENT_MOTOR_BITS    0xc0000 /**< \~english Bits indicating the current operating motor on boards with multiple outputs for engine mounting. \~russian Биты, показывающие текущий рабочий мотор на платах с несколькими выходами для двигателей. */
    #define URMC_STATE_CURRENT_MOTOR0    0x0 /**< \~english Motor 0. \~russian Мотор 0. */
    #define URMC_STATE_CURRENT_MOTOR1    0x40000 /**< \~english Motor 1. \~russian Мотор 1. */
    #define URMC_STATE_CURRENT_MOTOR2    0x80000 /**< \~english Motor 2. \~russian Мотор 2. */
    #define URMC_STATE_CURRENT_MOTOR3    0xc0000 /**< \~english Motor 3. \~russian Мотор 3. */
//@}
/**
* @anchor GPIOFlags_flagset
*/
//@{
    #define URMC_STATE_DIG_SIGNAL    0xffff /**< \~english Flags of digital signals. \~russian Флаги цифровых сигналов. */
    #define URMC_STATE_RIGHT_EDGE    0x1 /**< \~english Engine stuck at the right edge. \~russian Достижение правой границы. */
    #define URMC_STATE_LEFT_EDGE    0x2 /**< \~english Engine stuck at the left edge. \~russian Достижение левой границы. */
    #define URMC_STATE_REV_SENSOR    0x400 /**< \~english State of Revolution sensor pin. \~russian Состояние вывода датчика оборотов(флаг "1", если датчик активен). */
//@}
/**
* @anchor FeedbackType_flagset
*/
//@{
    #define URMC_FEEDBACK_ENCODER    0x1 /**< \~english Feedback by encoder. \~russian Обратная связь с помощью энкодера. */
    #define URMC_FEEDBACK_NONE    0x5 /**< \~english Feedback is absent. \~russian Обратная связь отсутствует. */
//@}
/**
* @anchor FeedbackFlags_flagset
*/
//@{
    #define URMC_FEEDBACK_ENC_REVERSE    0x1 /**< \~english Reverse count of encoder. \~russian Обратный счет у энкодера. */
    #define URMC_FEEDBACK_ENC_TYPE_BITS    0xc0 /**< \~english Bits of the encoder type. \~russian Биты, отвечающие за тип энкодера. */
    #define URMC_FEEDBACK_ENC_TYPE_AUTO    0x0 /**< \~english Auto detect encoder type. \~russian Определять тип энкодера автоматически. */
    #define URMC_FEEDBACK_ENC_TYPE_SINGLE_ENDED    0x40 /**< \~english Single ended encoder. \~russian Недифференциальный энкодер. */
    #define URMC_FEEDBACK_ENC_TYPE_DIFFERENTIAL    0x80 /**< \~english Differential encoder. \~russian Дифференциальный энкодер. */
//@}
/**
* @anchor HomeFlags_flagset
*/
//@{
    #define URMC_HOME_DIR_FIRST    0x1 /**< \~english Flag defines direction of 1st motion after execution of home command. Direction is right, if set; otherwise left. \~russian Определяет направление первоначального движения мотора после поступления команды HOME. Если флаг установлен - вправо; иначе - влево. */
    #define URMC_HOME_DIR_SECOND    0x2 /**< \~english Flag defines direction of 2nd motion. Direction is right, if set; otherwise left. \~russian Определяет направление второго движения мотора. Если флаг установлен - вправо; иначе - влево. */
    #define URMC_HOME_MV_SEC_EN    0x4 /**< \~english Use the second phase of calibration to the home position, if set; otherwise the second phase is skipped. \~russian Если флаг установлен, реализуется второй этап доводки в домашнюю позицию; иначе - этап пропускается. */
    #define URMC_HOME_HALF_MV    0x8 /**< \~english If the flag is set, the stop signals are ignored in start of second movement the first half-turn. \~russian Если флаг установлен, в начале второго движения первые пол оборота сигналы завершения движения игнорируются. */
    #define URMC_HOME_STOP_FIRST_BITS    0x30 /**< \~english Bits of the first stop selector. \~russian Биты, отвечающие за выбор сигнала завершения первого движения. */
    #define URMC_HOME_STOP_FIRST_REV    0x10 /**< \~english First motion stops by  revolution sensor. \~russian Первое движение завершается по сигналу с Revolution sensor. */
    #define URMC_HOME_STOP_FIRST_LIM    0x30 /**< \~english First motion stops by limit switch. \~russian Первое движение завершается по сигналу с концевика. */
    #define URMC_HOME_STOP_SECOND_BITS    0xc0 /**< \~english Bits of the second stop selector. \~russian Биты, отвечающие за выбор сигнала завершения второго движения. */
    #define URMC_HOME_STOP_SECOND_REV    0x40 /**< \~english Second motion stops by  revolution sensor. \~russian Второе движение завершается по сигналу с Revolution sensor. */
    #define URMC_HOME_STOP_SECOND_LIM    0xc0 /**< \~english Second motion stops by limit switch. \~russian Второе движение завершается по сигналу с концевика. */
    #define URMC_HOME_USE_FAST    0x100 /**< \~english Use the fast algorithm of calibration to the home position, if set; otherwise the traditional algorithm. \~russian Если флаг установлен, используется быстрый поиск домашней позиции; иначе - традиционный. */
//@}
/**
* @anchor EngineFlags_flagset
*/
//@{
    #define URMC_ENGINE_REVERSE    0x1 /**< \~english Reverse flag. It determines motor shaft rotation direction that corresponds to feedback counts increasing. If not set (default), motor shaft rotation direction under positive voltage corresponds to feedback counts increasing and vice versa. Change it if you see that positive directions on motor and feedback are opposite. \~russian Флаг реверса. Связывает направление вращения мотора с направлением счета текущей позиции. При сброшенном флаге (по умолчанию) прикладываемое к мотору положительное напряжение увеличивает счетчик позиции. И наоборот, при установленном флаге счетчик позиции увеличивается, когда к мотору приложено отрицательное напряжение. Измените состояние флага, если положительное вращение мотора уменьшает счетчик позиции. */
    #define URMC_ENGINE_CURRENT_AS_RMS    0x2 /**< \~english Engine current meaning flag. If the flag is set, then engine current value is interpreted as root mean square current value. If the flag is unset, then engine current value is interpreted as maximum amplitude value. \~russian Флаг интерпретации значения тока. Если флаг установлен, то задаваемое значение тока интерпретируется как среднеквадратичное значение тока, если флаг снят, то задаваемое значение тока интерпретируется как максимальная амплитуда тока. */
    #define URMC_ENGINE_MAX_SPEED    0x4 /**< \~english Max speed flag. If it is set, engine uses maximum speed achievable with the present engine settings as nominal speed. \~russian Флаг максимальной скорости. Если флаг установлен, движение происходит на максимальной скорости. */
    #define URMC_ENGINE_ANTIPLAY    0x8 /**< \~english Play compensation flag. If it set, engine makes backlash (play) compensation procedure and reach the predetermined position accurately on low speed. \~russian Компенсация люфта. Если флаг установлен, позиционер будет подходить к заданной точке всегда с одной стороны. Например, при подходе слева никаких дополнительных действий не совершается, а при подходе справа позиционер проходит целевую позицию на заданное расстояния и возвращается к ней опять же справа. */
    #define URMC_ENGINE_ACCEL_ON    0x10 /**< \~english Acceleration enable flag. If it set, motion begins with acceleration and ends with deceleration. \~russian Ускорение. Если флаг установлен, движение происходит с ускорением. */
    #define URMC_ENGINE_LIMIT_VOLT    0x20 /**< \~english Maximum motor voltage limit enable flag(is only used with DC motor). \~russian Номинальное напряжение мотора. Если флаг установлен, напряжение на моторе ограничивается заданным номинальным значением(используется только с DC двигателем). */
    #define URMC_ENGINE_LIMIT_CURR    0x40 /**< \~english Maximum motor current limit enable flag(is only used with DC motor). \~russian Номинальный ток мотора. Если флаг установлен, ток через мотор ограничивается заданным номинальным значением(используется только с DC двигателем). */
    #define URMC_ENGINE_LIMIT_RPM    0x80 /**< \~english Maximum motor speed limit enable flag. \~russian Номинальная частота вращения мотора. Если флаг установлен, частота вращения ограничивается заданным номинальным значением. */
//@}
/**
* @anchor MicrostepMode_flagset
*/
//@{
    #define URMC_MICROSTEP_MODE_FULL    0x1 /**< \~english Full step mode. \~russian Полношаговый режим. */
    #define URMC_MICROSTEP_MODE_FRAC_2    0x2 /**< \~english 1/2 step mode. \~russian Деление шага 1/2. */
    #define URMC_MICROSTEP_MODE_FRAC_4    0x3 /**< \~english 1/4 step mode. \~russian Деление шага 1/4. */
    #define URMC_MICROSTEP_MODE_FRAC_8    0x4 /**< \~english 1/8 step mode. \~russian Деление шага 1/8. */
    #define URMC_MICROSTEP_MODE_FRAC_16    0x5 /**< \~english 1/16 step mode. \~russian Деление шага 1/16. */
    #define URMC_MICROSTEP_MODE_FRAC_32    0x6 /**< \~english 1/32 step mode. \~russian Деление шага 1/32. */
    #define URMC_MICROSTEP_MODE_FRAC_64    0x7 /**< \~english 1/64 step mode. \~russian Деление шага 1/64. */
    #define URMC_MICROSTEP_MODE_FRAC_128    0x8 /**< \~english 1/128 step mode. \~russian Деление шага 1/128. */
    #define URMC_MICROSTEP_MODE_FRAC_256    0x9 /**< \~english 1/256 step mode. \~russian Деление шага 1/256. */
//@}
/**
* @anchor EngineType_flagset
*/
//@{
    #define URMC_ENGINE_TYPE_NONE    0x0 /**< \~english A value that shouldn't be used. \~russian Это значение не нужно использовать. */
    #define URMC_ENGINE_TYPE_DC    0x1 /**< \~english DC motor. \~russian Мотор постоянного тока. */
    #define URMC_ENGINE_TYPE_STEP    0x3 /**< \~english Step motor. \~russian Шаговый мотор. */
    #define URMC_ENGINE_TYPE_TEST    0x4 /**< \~english Duty cycle are fixed. Used only manufacturer. \~russian Скважность в обмотках фиксирована. Используется только производителем. */
//@}
/**
* @anchor PowerFlags_flagset
*/
//@{
    #define URMC_POWER_REDUCT_ENABLED    0x1 /**< \~english Current reduction enabled after CurrReductDelay, if this flag is set. \~russian Если флаг установлен, уменьшить ток по прошествии CurrReductDelay. Иначе - не уменьшать. */
    #define URMC_POWER_OFF_ENABLED    0x2 /**< \~english Power off enabled after PowerOffDelay, if this flag is set. \~russian Если флаг установлен, снять напряжение с обмоток по прошествии PowerOffDelay. Иначе - не снимать. */
    #define URMC_POWER_SMOOTH_CURRENT    0x4 /**< \~english Current ramp-up/down is performed smoothly during current_set_time, if this flag is set. \~russian Если установлен, то запитывание обмоток, снятие питания или снижение/повышение тока происходят плавно со скоростью CurrentSetTime, а только потом выполняется та задача, которая вызвала это плавное изменение. */
//@}
/**
* @anchor Flags_flagset
*/
//@{
    #define URMC_ALARM_ON_DRIVER_OVERHEATING    0x1 /**< \~english If this flag is set enter Alarm state on driver overheat signal. \~russian Если флаг установлен, то войти в состояние Alarm при получении сигнала подступающего перегрева с драйвера. Иначе - игнорировать подступающий перегрев с драйвера. */
    #define URMC_LOW_UPWR_PROTECTION    0x2 /**< \~english If this flag is set turn off motor when voltage is lower than LowUpwrOff. \~russian Если установлен, то выключать силовую часть при напряжении меньшем LowUpwrOff. */
    #define URMC_H_BRIDGE_ALERT    0x4 /**< \~english If this flag is set then turn off the power unit with a signal problem in one of the transistor bridge. \~russian Если установлен, то выключать силовую часть при сигнале неполадки в одном из транзисторных мостов. */
    #define URMC_ALARM_ON_BORDERS_SWAP_MISSET    0x8 /**< \~english If this flag is set enter Alarm state on borders swap misset \~russian Если флаг установлен, то войти в состояние Alarm при получении сигнала c противоположного концевика. */
    #define URMC_ALARM_FLAGS_STICKING    0x10 /**< \~english If this flag is set only a STOP command can turn all alarms to 0 \~russian Если флаг установлен, то только по команде STOP возможен сброс всех флагов ALARM. */
    #define URMC_USB_BREAK_RECONNECT    0x20 /**< \~english If this flag is set USB brake reconnect module will be enable \~russian Если флаг установлен, то будет включен блок перезагрузки USB при поломке связи. */
//@}
/**
* @anchor BorderFlags_flagset
*/
//@{
    #define URMC_BORDER_IS_ENCODER    0x1 /**< \~english Borders are fixed by predetermined encoder values, if set; borders position on limit switches, if not set. \~russian Если флаг установлен, границы определяются предустановленными точками на шкале позиции. Если флаг сброшен, границы определяются концевыми выключателями. */
    #define URMC_BORDER_STOP_LEFT    0x2 /**< \~english Motor should stop on left border. \~russian Если флаг установлен, мотор останавливается при достижении левой границы. */
    #define URMC_BORDER_STOP_RIGHT    0x4 /**< \~english Motor should stop on right border. \~russian Если флаг установлен, мотор останавливается при достижении правой границы. */
    #define URMC_BORDERS_SWAP_MISSET_DETECTION    0x8 /**< \~english Motor should stop on both borders. Need to save motor then wrong border settings is set \~russian Если флаг установлен, мотор останавливается при достижении обоих границ. Нужен для предотвращения поломки двигателя при неправильных настройках концевиков */
//@}
/**
* @anchor EnderFlags_flagset
*/
//@{
    #define URMC_ENDER_SWAP    0x1 /**< \~english First limit switch on the right side, if set; otherwise on the left side. \~russian Если флаг установлен, первый концевой выключатель находится справа; иначе - слева. */
    #define URMC_ENDER_SW1_ACTIVE_LOW    0x2 /**< \~english 1 - Limit switch connnected to pin SW1 is triggered by a low level on pin. \~russian 1 - Концевик, подключенный к ножке SW1, считается сработавшим по низкому уровню на контакте. */
    #define URMC_ENDER_SW2_ACTIVE_LOW    0x4 /**< \~english 1 - Limit switch connnected to pin SW2 is triggered by a low level on pin. \~russian 1 - Концевик, подключенный к ножке SW2, считается сработавшим по низкому уровню на контакте. */
//@}
/**
* @anchor CTPFlags_flagset
*/
//@{
    #define URMC_CTP_ENABLED    0x1 /**< \~english Position control is enabled, if flag set. \~russian Контроль позиции включен, если флаг установлен. */
    #define URMC_CTP_BASE    0x2 /**< \~english Position control is based on revolution sensor, if this flag is set; otherwise it is based on encoder. \~russian Опорой является датчик оборотов, если флаг установлен; иначе - энкодер. */
    #define URMC_CTP_ALARM_ON_ERROR    0x4 /**< \~english Set ALARM on mismatch, if flag set. \~russian Войти в состояние ALARM при расхождении позиции, если флаг установлен. */
    #define URMC_REV_SENS_INV    0x8 /**< \~english Sensor is active when it 0 and invert makes active level 1. That is, if you do not invert, it is normal logic - 0 is the activation. \~russian Сенсор считается активным, когда на нём 0, а инвертирование делает активным уровнем 1. То есть если не инвертировать, то действует обычная логика - 0 это срабатывание/активация/активное состояние. */
    #define URMC_CTP_ERROR_CORRECTION    0x10 /**< \~english Correct errors which appear when slippage if the flag is set. It works only with the encoder. Incompatible with flag CTP_ALARM_ON_ERROR. \~russian Корректировать ошибки, возникающие при проскальзывании, если флаг установлен. Работает только с энкодером. Несовместимо с флагом CTP_ALARM_ON_ERROR. */
//@}
/**
* @anchor CtrlFlags_flagset
*/
//@{
    #define URMC_EEPROM_PRECEDENCE    0x1 /**< \~english If the flag is set settings from external EEPROM override controller settings. \~russian Если флаг установлен, то настройки в EEPROM подвижки имеют приоритет над текущими настройками и заменяют их при обнаружении EEPROM. */
//@}

/*
 * All structures below.
 */
typedef struct
{
  int32_t Position; /**< \~english Desired position (whole steps). \~russian Желаемая позиция (целая часть). */
  int16_t uPosition; /**< \~english The fractional part of a position in microsteps. Is only used with stepper motor. Range: -255..255. \~russian Дробная часть позиции в микрошагах. Используется только с шаговым двигателем. Диапазон: -255..255. */
  uint8_t reserved[6]; /**< \~english  \~russian  */

} urmc_command_move_t;

typedef struct
{
  int32_t DeltaPosition; /**< \~english Shift (delta) of position \~russian Смещение (дельта) позиции */
  int16_t uDeltaPosition; /**< \~english Fractional part of the shift in micro steps is only used with stepper motor. Range: -255..255. \~russian Дробная часть смещения в микрошагах, используется только с шаговым двигателем. Диапазон: -255..255. */
  uint8_t reserved[6]; /**< \~english  \~russian  */

} urmc_command_movr_t;

typedef struct
{
  int32_t Position; /**< \~english The position of the whole steps in the engine \~russian Позиция в основных шагах двигателя */
  int16_t uPosition; /**< \~english Microstep position is only used with stepper motors \~russian Позиция в микрошагах(используется только с шаговыми двигателями). */
  int64_t EncPosition; /**< \~english Encoder position. \~russian Позиция энкодера. */
  uint8_t reserved[6]; /**< \~english  \~russian  */

} urmc_get_position_t;

typedef struct
{
  int32_t Position; /**< \~english The position of the whole steps in the engine \~russian Позиция в основных шагах двигателя */
  int16_t uPosition; /**< \~english Microstep position is only used with stepper motors \~russian Позиция в микрошагах(используется только с шаговыми двигателями). */
  int64_t EncPosition; /**< \~english Encoder position. \~russian Позиция энкодера. */
  uint8_t PosFlags; /**< \~english Flags \~russian Флаги */
  uint8_t reserved[5]; /**< \~english  \~russian  */

} urmc_set_position_t;

typedef struct
{
  uint8_t MoveSts; /**< \~english Move state. \~russian Состояние движения. */
  uint8_t MvCmdSts; /**< \~english Move command state. \~russian Состояние команды движения (касается command_move, command_movr, command_left, command_right, command_stop, command_home, command_loft). */
  uint8_t PWRSts; /**< \~english Power state of the stepper motor (used only with stepper motor). \~russian Состояние питания шагового двигателя (используется только с шаговым двигателем). */
  uint8_t EncSts; /**< \~english Encoder state. \~russian Состояние энкодера. */
  uint8_t reserved0[1]; /**< \~english  \~russian  */
  int32_t CurPosition; /**< \~english Current position. \~russian Первичное поле, в котором хранится текущая позиция, как бы ни была устроена обратная связь. В случае работы с DC-мотором в этом поле находится текущая позиция по данным с энкодера, в случае работы с ШД-мотором в режиме, когда первичными являются импульсы, подаваемые на мотор, в этом поле содержится целое значение шагов текущей позиции. */
  int16_t uCurPosition; /**< \~english Step motor shaft position in 1/256 microsteps. Used only with stepper motor. \~russian Дробная часть текущей позиции в микрошагах (-255..255). Используется только с шаговым двигателем. */
  int64_t EncPosition; /**< \~english Current encoder position. \~russian Текущая позиция по данным с энкодера в импульсах энкодера, используется только если энкодер установлен, активизирован и не является основным датчиком положения, например при использовании энкодера совместно с шаговым двигателем для контроля проскальзования. */
  int32_t CurSpeed; /**< \~english Motor shaft speed. \~russian Текущая скорость. */
  int16_t uCurSpeed; /**< \~english Part of motor shaft speed in 1/256 microsteps. Used only with stepper motor. \~russian Дробная часть текущей скорости в микрошагах (-255..255). Используется только с шаговым двигателем. */
  uint8_t reserved1[2]; /**< \~english  \~russian  */
  int16_t Upwr; /**< \~english Power supply voltage, tens of mV. \~russian Напряжение на силовой части, десятки мВ. */
  uint8_t reserved2[6]; /**< \~english  \~russian  */
  uint32_t Flags; /**< \~english Set of flags specify motor shaft movement algorithm and list of limitations. \~russian Флаги состояний. */
  uint32_t GPIOFlags; /**< \~english Set of flags of gpio states \~russian Флаги состояний GPIO входов. */
  uint8_t reserved[5]; /**< \~english  \~russian  */

} urmc_status_impl_t;

typedef struct
{
  int32_t Speed[25]; /**< \~english Current speed. \~russian Текущая скорость. */
  int32_t Error[25]; /**< \~english Current error. \~russian Текущая скорость. */
  uint32_t Length; /**< \~english Length of actual data in buffer. \~russian Длина фактических данных в буфере. */
  uint8_t reserved[6]; /**< \~english  \~russian  */

} urmc_measurements_t;

typedef struct
{
  int16_t WindingVoltageA; /**< \~english In the case step motor, the voltage across the winding A; in the case of a brushless, the voltage on the first coil, in the case of the only DC. \~russian В случае ШД, напряжение на обмотке A; в случае бесщеточного, напряжение на первой обмотке; в случае DC на единственной. */
  int16_t WindingVoltageB; /**< \~english In the case step motor, the voltage across the winding B; in case of a brushless, the voltage on the second winding, and in the case of DC is not used. \~russian В случае ШД, напряжение на обмотке B; в случае бесщеточного, напряжение на второй обмотке; в случае DC не используется. */
  uint8_t reserved0[2]; /**< \~english  \~russian  */
  int16_t WindingCurrentA; /**< \~english In the case step motor, the current in the coil A; brushless if the current in the first coil, and in the case of a single DC. \~russian В случае ШД, ток в обмотке A; в случае бесщеточного, ток в первой обмотке; в случае DC в единственной. */
  int16_t WindingCurrentB; /**< \~english In the case step motor, the current in the coil B; brushless if the current in the second coil, and in the case of DC is not used. \~russian В случае ШД, ток в обмотке B; в случае бесщеточного, ток в второй обмотке; в случае DC не используется. */
  uint8_t reserved1[6]; /**< \~english  \~russian  */
  int16_t DutyCycle; /**< \~english Duty cycle of PWM. \~russian Коэффициент заполнения ШИМ. */
  uint8_t reserved[14]; /**< \~english  \~russian  */

} urmc_chart_data_t;

typedef struct
{
  int8_t Manufacturer[4]; /**< \~english Manufacturer \~russian Производитель */
  int8_t ManufacturerId[2]; /**< \~english Manufacturer id \~russian Идентификатор производителя */
  int8_t ProductDescription[8]; /**< \~english Product description \~russian Описание продукта */
  uint8_t Major; /**< \~english The major number of the hardware version. \~russian Основной номер версии железа. */
  uint8_t Minor; /**< \~english Minor number of the hardware version. \~russian Второстепенный номер версии железа. */
  uint16_t Release; /**< \~english Number of edits this release of hardware. \~russian Номер правок этой версии железа. */
  uint8_t reserved[12]; /**< \~english  \~russian  */

} urmc_device_information_impl_t;

typedef struct
{
  uint32_t SerialNumber; /**< \~english Board serial number. \~russian Серийный номер платы. */

} urmc_get_serial_number_t;

typedef struct
{
  uint8_t Major; /**< \~english Firmware major version number \~russian Мажорный номер версии прошивки */
  uint8_t Minor; /**< \~english Firmware minor version number \~russian Минорный номер версии прошивки */
  uint16_t Release; /**< \~english Firmware release version number \~russian Номер релиза версии прошивки */

} urmc_firmware_version_t;

typedef struct
{
  uint16_t A1Voltage_ADC; /**< \~english "Voltage on pin 1 winding A" raw data from ADC. \~russian "Выходное напряжение на 1 выводе обмотки А" необработанные данные с АЦП. */
  uint16_t A2Voltage_ADC; /**< \~english "Voltage on pin 2 winding A" raw data from ADC. \~russian "Выходное напряжение на 2 выводе обмотки А" необработанные данные с АЦП. */
  uint16_t B1Voltage_ADC; /**< \~english "Voltage on pin 1 winding B" raw data from ADC. \~russian "Выходное напряжение на 1 выводе обмотки B" необработанные данные с АЦП. */
  uint16_t B2Voltage_ADC; /**< \~english "Voltage on pin 2 winding B" raw data from ADC. \~russian "Выходное напряжение на 2 выводе обмотки B" необработанные данные с АЦП. */
  uint16_t SupVoltage_ADC; /**< \~english "Voltage on the top of MOSFET full bridge" raw data from ADC. \~russian "Напряжение питания ключей Н-моста" необработанные данные с АЦП. */
  uint16_t ACurrent_ADC; /**< \~english "Winding A current" raw data from ADC. \~russian "Ток через обмотку А" необработанные данные с АЦП. */
  uint16_t BCurrent_ADC; /**< \~english "Winding B current" raw data from ADC. \~russian "Ток через обмотку B" необработанные данные с АЦП. */
  uint8_t reserved0[8]; /**< \~english  \~russian  */
  uint16_t L5_ADC; /**< \~english USB supply voltage after the current sense resistor, from ADC. \~russian Напряжение питания USB после current sense резистора, необработанные данные с АЦП. */
  uint8_t reserved1[2]; /**< \~english  \~russian  */
  int16_t A1Voltage; /**< \~english "Voltage on pin 1 winding A" calibrated data. \~russian "Выходное напряжение на 1 выводе обмотки А" откалиброванные данные. */
  int16_t A2Voltage; /**< \~english "Voltage on pin 2 winding A" calibrated data. \~russian "Выходное напряжение на 2 выводе обмотки А" откалиброванные данные. */
  int16_t B1Voltage; /**< \~english "Voltage on pin 1 winding B" calibrated data. \~russian "Выходное напряжение на 1 выводе обмотки B" откалиброванные данные. */
  int16_t B2Voltage; /**< \~english "Voltage on pin 2 winding B" calibrated data. \~russian "Выходное напряжение на 2 выводе обмотки B" откалиброванные данные. */
  int16_t SupVoltage; /**< \~english "Voltage on the top of MOSFET full bridge" calibrated data. \~russian "Напряжение питания ключей Н-моста" откалиброванные данные. */
  int16_t ACurrent; /**< \~english "Winding A current" calibrated data. \~russian "Ток через обмотку А" откалиброванные данные. */
  int16_t BCurrent; /**< \~english "Winding B current" calibrated data. \~russian "Ток через обмотку B" откалиброванные данные. */
  uint8_t reserved2[8]; /**< \~english  \~russian  */
  int16_t L5; /**< \~english USB supply voltage after the current sense resistor. \~russian Напряжение питания USB после current sense резистора */
  uint8_t reserved3[4]; /**< \~english  \~russian  */
  int32_t R; /**< \~english Motor winding resistance in mOhms(is only used with stepper motor). \~russian Сопротивление обмоток двигателя(для шагового двигателя),  в мОм */
  int32_t L; /**< \~english Motor winding pseudo inductance in uHn(is only used with stepper motor). \~russian Псевдоиндуктивность обмоток двигателя(для шагового двигателя),  в мкГн */
  uint8_t reserved[8]; /**< \~english  \~russian  */

} urmc_analog_data_t;

typedef struct
{
  uint8_t DebugData[128]; /**< \~english Arbitrary debug data. \~russian Отладочные данные. */
  uint8_t reserved[8]; /**< \~english  \~russian  */

} urmc_debug_read_t;

typedef struct
{
  uint8_t DebugData[128]; /**< \~english Arbitrary debug data. \~russian Отладочные данные. */
  uint8_t reserved[8]; /**< \~english  \~russian  */

} urmc_debug_write_t;

typedef struct
{
  uint8_t Major; /**< \~english Bootloader major version number \~russian Мажорный номер версии загрузчика */
  uint8_t Minor; /**< \~english Bootloader minor version number \~russian Минорный номер версии загрузчика */
  uint16_t Release; /**< \~english Bootloader release version number \~russian Номер релиза версии загрузчика */

} urmc_bootloader_version_t;

typedef struct
{
  uint8_t Motor; /**< \~english Motor number which it should be switch relay on [0..1] \~russian Номер мотора, на который следует переключить реле [0..1] */
  uint8_t reserved[15]; /**< \~english  \~russian  */

} urmc_change_motor_t;

typedef struct
{
  uint16_t IPS; /**< \~english The number of encoder counts per shaft revolution. Range: 1..655535. The field is obsolete, it is recommended to write 0 to IPS and use the extended CountsPerTurn field. You may need to update the controller firmware to the latest version. \~russian Количество отсчётов энкодера на оборот вала. Диапазон: 1..65535. Поле устарело, рекомендуется записывать 0 в IPS и использовать расширенное поле CountsPerTurn. Может потребоваться обновление микропрограммы контроллера до последней версии. */
  uint8_t FeedbackType; /**< \~english Type of feedback \~russian Тип обратной связи */
  uint8_t FeedbackFlags; /**< \~english Flags \~russian Флаги */
  uint32_t CountsPerTurn; /**< \~english The number of encoder counts per shaft revolution. Range: 1..4294967295. To use the CountsPerTurn field, write 0 in the IPS field, otherwise the value from the IPS field will be used. \~russian Количество отсчётов энкодера на оборот вала. Диапазон: 1..4294967295. Для использования поля CountsPerTurn нужно записать 0 в поле IPS, иначе будет использоваться значение из поля IPS. */
  uint8_t reserved[4]; /**< \~english  \~russian  */

} urmc_feedback_settings_t;

typedef struct
{
  uint32_t FastHome; /**< \~english Speed used for first motion. Range: 0..100000. \~russian Скорость первого движения. Диапазон: 0..100000 */
  uint8_t uFastHome; /**< \~english Part of the speed for first motion, microsteps. \~russian Дробная часть скорости первого движения в микрошагах(используется только с шаговым двигателем). */
  uint32_t SlowHome; /**< \~english Speed used for second motion. Range: 0..100000. \~russian Скорость второго движения. Диапазон: 0..100000. */
  uint8_t uSlowHome; /**< \~english Part of the speed for second motion, microsteps. \~russian Дробная часть скорости второго движения в микрошагах(используется только с шаговым двигателем). */
  int32_t HomeDelta; /**< \~english Distance from break point. \~russian Расстояние отхода от точки останова. */
  int16_t uHomeDelta; /**< \~english Part of the delta distance, microsteps. Range: -255..255. \~russian Дробная часть расстояния отхода от точки останова в микрошагах(используется только с шаговым двигателем). Диапазон: -255..255. */
  uint16_t HomeFlags; /**< \~english Set of flags specify direction and stopping conditions. \~russian Набор флагов, определяющие такие параметры, как направление и условия останова. */
  uint8_t reserved[9]; /**< \~english  \~russian  */

} urmc_home_settings_t;

typedef struct
{
  uint32_t Speed; /**< \~english Target speed (for stepper motor: steps/s, for DC: rpm). Range: 0..100000. \~russian Заданная скорость (для ШД: шагов/c, для DC: rpm). Диапазон: 0..100000. */
  uint8_t uSpeed; /**< \~english Target speed in microstep fractions/s. Using with stepper motor only. \~russian Заданная скорость в единицах деления микрошага в секунду. Используется только с шаговым мотором. */
  uint16_t Accel; /**< \~english Motor shaft acceleration, steps/s^2(stepper motor) or RPM/s(DC). Range: 1..65535. \~russian Ускорение, заданное в шагах в секунду^2(ШД) или в оборотах в минуту за секунду(DC). Диапазон: 1..65535. */
  uint16_t Decel; /**< \~english Motor shaft deceleration, steps/s^2(stepper motor) or RPM/s(DC). Range: 1..65535. \~russian Торможение, заданное в шагах в секунду^2(ШД) или в оборотах в минуту за секунду(DC). Диапазон: 1..65535. */
  uint32_t AntiplaySpeed; /**< \~english Speed in antiplay mode, full steps/s(stepper motor) or RPM(DC). Range: 0..100000. \~russian Скорость в режиме антилюфта, заданная в целых шагах/c(ШД) или в оборотах/с(DC). Диапазон: 0..100000. */
  uint8_t uAntiplaySpeed; /**< \~english Speed in antiplay mode, 1/256 microsteps/s. Used with stepper motor only. \~russian Скорость в режиме антилюфта, выраженная в 1/256 микрошагах в секунду. Используется только с шаговым мотором. */
  uint8_t reserved[10]; /**< \~english  \~russian  */

} urmc_move_settings_t;

typedef struct
{
  uint16_t NomVoltage; /**< \~english Rated voltage in tens of mV. Controller will keep the voltage drop on motor below this value if ENGINE_LIMIT_VOLT flag is set (used with DC only). \~russian Номинальное напряжение мотора в десятках мВ. Контроллер будет сохранять напряжение на моторе не выше номинального, если установлен флаг ENGINE_LIMIT_VOLT (используется только с DC двигателем). */
  uint16_t NomCurrent; /**< \~english Rated current. Controller will keep current consumed by motor below this value if ENGINE_LIMIT_CURR flag is set. Range: 15..8000 \~russian Номинальный ток через мотор. Ток стабилизируется для шаговых и может быть ограничен для DC(если установлен флаг ENGINE_LIMIT_CURR). Диапазон: 15..8000 */
  uint32_t NomSpeed; /**< \~english Nominal speed (in whole steps/s or rpm for DC and stepper motor as a master encoder). Controller will keep motor shaft RPM below this value if ENGINE_LIMIT_RPM flag is set. Range: 1..100000. \~russian Номинальная скорость (в целых шагах/с или rpm для DC и шагового двигателя в режиме ведущего энкодера). Контроллер будет сохранять скорость мотора не выше номинальной, если установлен флаг ENGINE_LIMIT_RPM. Диапазон: 1..100000. */
  uint8_t uNomSpeed; /**< \~english The fractional part of a nominal speed in microsteps (is only used with stepper motor). \~russian Микрошаговая часть номинальной скорости мотора (используется только с шаговым двигателем). */
  uint16_t EngineFlags; /**< \~english Set of flags specify motor shaft movement algorithm and list of limitations \~russian Флаги, управляющие работой мотора. */
  int16_t Antiplay; /**< \~english Number of pulses or steps for backlash (play) compensation procedure. Used if ENGINE_ANTIPLAY flag is set. \~russian Количество шагов двигателя или импульсов энкодера, на которое позиционер будет отъезжать от заданной позиции для подхода к ней с одной и той же стороны. Используется, если установлен флаг ENGINE_ANTIPLAY. */
  uint8_t MicrostepMode; /**< \~english Settings of microstep mode(Used with stepper motor only). \~russian Настройки микрошагового режима(используется только с шаговым двигателем). */
  uint16_t StepsPerRev; /**< \~english Number of full steps per revolution(Used with stepper motor only). Range: 1..65535. \~russian Количество полных шагов на оборот(используется только с шаговым двигателем). Диапазон: 1..65535. */
  uint8_t reserved[12]; /**< \~english  \~russian  */

} urmc_engine_settings_t;

typedef struct
{
  uint8_t EngineType; /**< \~english Engine type \~russian Тип мотора */
  uint8_t reserved[7]; /**< \~english  \~russian  */

} urmc_entype_settings_t;

typedef struct
{
  uint8_t HoldCurrent; /**< \~english Current in holding regime, percent of nominal. Range: 0..100. \~russian Ток мотора в режиме удержания, в процентах от номинального. Диапазон: 0..100. */
  uint16_t CurrReductDelay; /**< \~english Time in ms from going to STOP state to reducting current. \~russian Время в мс от перехода в состояние STOP до уменьшения тока. */
  uint16_t PowerOffDelay; /**< \~english Time in s from going to STOP state to turning power off. \~russian Время в с от перехода в состояние STOP до отключения питания мотора. */
  uint16_t CurrentSetTime; /**< \~english Time in ms to reach nominal current. \~russian Время в мс, требуемое для набора номинального тока от 0% до 100%. */
  uint8_t PowerFlags; /**< \~english Flags with parameters of power control. \~russian Флаги параметров управления питанием. */
  uint8_t reserved[6]; /**< \~english  \~russian  */

} urmc_power_settings_t;

typedef struct
{
  uint16_t LowUpwrOff; /**< \~english Lower voltage limit to turn off the motor, tens of mV. \~russian Нижний порог напряжения на силовой части для выключения, десятки мВ. */
  uint8_t reserved0[2]; /**< \~english  \~russian  */
  uint16_t CriticalUpwr; /**< \~english Maximum motor voltage which triggers ALARM state, tens of mV. \~russian Максимальное напряжение на силовой части, вызывающее состояние ALARM, десятки мВ. */
  uint8_t reserved1[8]; /**< \~english  \~russian  */
  uint8_t Flags; /**< \~english Critical parameter flags. \~russian Флаги критических параметров. */
  uint8_t reserved[7]; /**< \~english  \~russian  */

} urmc_secure_settings_t;

typedef struct
{
  uint8_t BorderFlags; /**< \~english Border flags, specify types of borders and motor behaviour on borders. \~russian Флаги, определяющие тип границ и поведение мотора при их достижении. */
  uint8_t EnderFlags; /**< \~english Ender flags, specify electrical behaviour of limit switches like order and pulled positions. \~russian Флаги, определяющие настройки концевых выключателей. */
  int32_t LeftBorder; /**< \~english Left border position, used if BORDER_IS_ENCODER flag is set. \~russian Позиция левой границы, используется если установлен флаг BORDER_IS_ENCODER. */
  int16_t uLeftBorder; /**< \~english Left border position in 1/256 microsteps(used with stepper motor only). Range: -255..255. \~russian Позиция левой границы в 1/256 микрошагах( используется только с шаговым двигателем). Диапазон: -255..255. */
  int32_t RightBorder; /**< \~english Right border position, used if BORDER_IS_ENCODER flag is set. \~russian Позиция правой границы, используется если установлен флаг BORDER_IS_ENCODER. */
  int16_t uRightBorder; /**< \~english Right border position in 1/256 microsteps. Used with stepper motor only. Range: -255..255. \~russian Позиция правой границы в 1/256 микрошагах (используется только с шаговым двигателем). Диапазон: -255..255. */
  uint8_t reserved[6]; /**< \~english  \~russian  */

} urmc_edges_settings_t;

typedef struct
{
  uint16_t KpU; /**< \~english Proportional gain for voltage PID routine \~russian Пропорциональный коэффициент ПИД контура по напряжению */
  uint16_t KiU; /**< \~english Integral gain for voltage PID routine \~russian Интегральный коэффициент ПИД контура по напряжению */
  uint16_t KdU; /**< \~english Differential gain for voltage PID routine \~russian Дифференциальный коэффициент ПИД контура по напряжению */
  float Kpf; /**< \~english Proportional gain for BLDC position PID routine \~russian Пропорциональный коэффициент ПИД контура по позиции для BLDC */
  float Kif; /**< \~english Integral gain for BLDC position PID routine \~russian Интегральный коэффициент ПИД контура по позиции для BLDC */
  float Kdf; /**< \~english Differential gain for BLDC position PID routine \~russian Дифференциальный коэффициент ПИД контура по позиции для BLDC */
  uint8_t reserved[24]; /**< \~english  \~russian  */

} urmc_pid_settings_t;

typedef struct
{
  uint8_t reserved[22]; /**< \~english  \~russian  */

} urmc_sync_in_settings_t;

typedef struct
{
  uint8_t reserved[10]; /**< \~english  \~russian  */

} urmc_sync_out_settings_t;

typedef struct
{
  uint8_t reserved[12]; /**< \~english  \~russian  */

} urmc_extio_settings_t;

typedef struct
{
  uint8_t reserved[19]; /**< \~english  \~russian  */

} urmc_brake_settings_t;

typedef struct
{
  uint8_t reserved[87]; /**< \~english  \~russian  */

} urmc_control_settings_t;

typedef struct
{
  uint8_t reserved[16]; /**< \~english  \~russian  */

} urmc_joystick_settings_t;

typedef struct
{
  uint8_t CTPMinError; /**< \~english Minimum contrast steps from step motor encoder position, wich set STATE_CTP_ERROR flag. Measured in steps step motor. \~russian Минимальное отличие шагов ШД от положения энкодера, устанавливающее флаг STATE_RT_ERROR. Измеряется в шагах ШД. */
  uint8_t CTPFlags; /**< \~english Flags. \~russian Флаги. */
  uint8_t reserved[10]; /**< \~english  \~russian  */

} urmc_ctp_settings_t;

typedef struct
{
  uint8_t reserved[10]; /**< \~english  \~russian  */

} urmc_uart_settings_t;

typedef struct
{
  int8_t ControllerName[16]; /**< \~english User conroller name. Can be set by user for his/her convinience. Max string length: 16 chars. \~russian Пользовательское имя контроллера. Может быть установлено пользователем для его удобства. Максимальная длина строки: 16 символов. */
  uint8_t CtrlFlags; /**< \~english Internal controller settings. \~russian Настройки контроллера. */
  uint8_t reserved[7]; /**< \~english  \~russian  */

} urmc_controller_name_t;

typedef struct
{
  uint8_t reserved[30]; /**< \~english  \~russian  */

} urmc_nonvolatile_memory_t;

typedef struct
{
  uint8_t reserved[24]; /**< \~english  \~russian  */

} urmc_stage_name_t;

typedef struct
{
  uint8_t reserved[64]; /**< \~english  \~russian  */

} urmc_stage_information_t;

typedef struct
{
  uint8_t reserved[64]; /**< \~english  \~russian  */

} urmc_stage_settings_t;

typedef struct
{
  uint8_t reserved[64]; /**< \~english  \~russian  */

} urmc_motor_information_t;

typedef struct
{
  uint8_t reserved[106]; /**< \~english  \~russian  */

} urmc_motor_settings_t;

typedef struct
{
  uint8_t reserved[64]; /**< \~english  \~russian  */

} urmc_encoder_information_t;

typedef struct
{
  uint8_t reserved[48]; /**< \~english  \~russian  */

} urmc_encoder_settings_t;

typedef struct
{
  uint8_t reserved[64]; /**< \~english  \~russian  */

} urmc_hallsensor_information_t;

typedef struct
{
  uint8_t reserved[44]; /**< \~english  \~russian  */

} urmc_hallsensor_settings_t;

typedef struct
{
  uint8_t reserved[64]; /**< \~english  \~russian  */

} urmc_gear_information_t;

typedef struct
{
  uint8_t reserved[52]; /**< \~english  \~russian  */

} urmc_gear_settings_t;

typedef struct
{
  uint8_t reserved[108]; /**< \~english  \~russian  */

} urmc_accessories_settings_t;


/**
    * \~english
    * Immediately stop the engine, the transition to the STOP, mode key BREAK (winding short-circuited), the regime "retention" is deactivated for DC motors, keeping current in the windings for stepper motors (with Power management settings).
    * @param[in] id device identifier
    * \~russian
    * Немедленная остановка двигателя, переход в состояние STOP, ключи в режиме BREAK (обмотки накоротко замкнуты), режим "удержания" дезактивируется для DC двигателей, удержание тока в обмотках для шаговых двигателей (с учётом Power management настроек).
    * @param[in] id идентификатор устройства
    */
URPC_VIS result_t URPC_CALLCONV urmc_command_stop(device_t id);
/**
    * \~english
    * Immediately power off motor regardless its state. Shouldn't be used during motion as the motor could be power on again automatically to continue movement. The command is designed for manual motor power off. When automatic power off after stop is required, use power management system.
    * @param[in] id device identifier
    * \~russian
    * Немедленное отключение питания двигателя вне зависимости от его состояния. Команда предначена для ручного управления питанием двигателя. Не следует использовать эту команду для отключения двигателя во время движения, так как питание может снова включиться для завершения движения. Для автоматического управления питанием двигателя и его отключении после остановки следует использовать систему управления электропитанием.
    * @param[in] id идентификатор устройства
    */
URPC_VIS result_t URPC_CALLCONV urmc_command_power_off(device_t id);
/**
    * \~english
    * Upon receiving the command "move" the engine starts to move with pre-set parameters (speed, acceleration, retention), to the point specified to the Position, uPosition. For stepper motor uPosition sets the microstep, for DC motor this field is not used.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * При получении команды "move" двигатель начинает перемещаться (если не используется режим "ТТЛСинхроВхода"), с заранее установленными параметрами (скорость, ускорение, удержание), к точке указанной в полях Position, uPosition. Для шагового мотора uPosition задает значение микрошага, для DC мотора это поле не используется.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_command_move(device_t id, urmc_command_move_t* input);
/**
    * \~english
    * Upon receiving the command "movr" engine starts to move with pre-set parameters (speed, acceleration, hold), left or right (depending on the sign of DeltaPosition) by the number of pulses specified in the fields DeltaPosition, uDeltaPosition. For stepper motor uDeltaPosition sets the microstep, for DC motor this field is not used.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * При получении команды "movr" двигатель начинает смещаться (если не используется режим "ТТЛСинхроВхода"), с заранее установленными параметрами (скорость, ускорение, удержание), влево или вправо (зависит от знака DeltaPosition) на количество импульсов указанное в полях DeltaPosition, uDeltaPosition. Для шагового мотора uDeltaPosition задает значение микрошага, для DC мотора это поле не используется.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_command_movr(device_t id, urmc_command_movr_t* input);
/**
    * \~english
    * The positive direction is to the right. A value of zero reverses the direction of the direction of the flag, the set speed. Restriction imposed by the trailer, act the same, except that the limit switch contact does not stop. Limit the maximum speed, acceleration and deceleration function. 1) moves the motor according to the speed FastHome, uFastHome and flag HOME_DIR_FAST until limit switch, if the flag is set HOME_STOP_ENDS, until the signal from the input synchronization if the flag HOME_STOP_SYNC (as accurately as possible is important to catch the moment of operation limit switch) or until the signal is received from the speed sensor, if the flag HOME_STOP_REV_SN 2) then moves according to the speed SlowHome, uSlowHome and flag HOME_DIR_SLOW until signal from the clock input, if the flag HOME_MV_SEC. If the flag HOME_MV_SEC reset skip this paragraph. 3) then move the motor according to the speed FastHome, uFastHome and flag HOME_DIR_SLOW a distance HomeDelta, uHomeDelta. description of flags and variable see in description for commands GHOM/SHOM
    * @param[in] id device identifier
    * \~russian
    * Поля скоростей знаковые. Положительное направление это вправо. Нулевое значение флага направления инвертирует направление, заданное скоростью. Ограничение, накладываемые концевиками, действуют так же, за исключением того, что касание концевика не приводит к остановке. Ограничения максимальной скорости, ускорения и замедления действуют. 1) Двигает мотор согласно скоростям FastHome, uFastHome и флагу HOME_DIR_FAST до достижения концевика, если флаг HOME_STOP_ENDS установлен, до достижения сигнала с входа синхронизации, если установлен флаг HOME_STOP_SYNC (важно как можно точнее поймать момент срабатывания концевика) или  до поступления сигнала с датчика оборотов, если установлен флаг HOME_STOP_REV_SN 2) далее двигает согласно скоростям SlowHome, uSlowHome и флагу HOME_DIR_SLOW до достижения сигнала с входа синхронизации, если установлен флаг HOME_MV_SEC. Если флаг HOME_MV_SEC сброшен, пропускаем этот пункт. 3) далее двигает мотор согласно скоростям FastHome, uFastHome и флагу HOME_DIR_SLOW на расстояние HomeDelta, uHomeDelta. Описание флагов и переменных см. описание команд GHOM/SHOM
    * @param[in] id идентификатор устройства
    */
URPC_VIS result_t URPC_CALLCONV urmc_command_home(device_t id);
/**
    * \~english
    * Start continous moving to the left.
    * @param[in] id device identifier
    * \~russian
    * При получении команды "left" двигатель начинает смещаться, с заранее установленными параметрами (скорость, ускорение), влево.
    * @param[in] id идентификатор устройства
    */
URPC_VIS result_t URPC_CALLCONV urmc_command_left(device_t id);
/**
    * \~english
    * Start continous moving to the right.
    * @param[in] id device identifier
    * \~russian
    * При получении команды "rigt" двигатель начинает смещаться, с заранее установленными параметрами (скорость, ускорение), вправо.
    * @param[in] id идентификатор устройства
    */
URPC_VIS result_t URPC_CALLCONV urmc_command_right(device_t id);
/**
    * \~english
    * Upon receiving the command "loft" the engine is shifted from the current point to a distance GENG :: Antiplay, then move to the same point.
    * @param[in] id device identifier
    * \~russian
    * При получении команды "loft" двигатель смещается из текущей точки на расстояние GENG::Antiplay, затем двигается в ту же точку.
    * @param[in] id идентификатор устройства
    */
URPC_VIS result_t URPC_CALLCONV urmc_command_loft(device_t id);
/**
    * \~english
    * Soft stop engine. The motor stops with deceleration speed.
    * @param[in] id device identifier
    * \~russian
    * Плавная остановка. Двигатель останавливается с ускорением замедления.
    * @param[in] id идентификатор устройства
    */
URPC_VIS result_t URPC_CALLCONV urmc_command_sstp(device_t id);
/**
    * \~english
    * Reads the value position in steps and micro for stepper motor and encoder steps all engines.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Считывает значение положения в шагах и микрошагах для шагового двигателя и в шагах энкодера всех двигателей.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_position(device_t id, urmc_get_position_t* output);
/**
    * \~english
    * Sets any position value in steps and micro for stepper motor and encoder steps of all engines. It means, that changing main indicator of position.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Устанавливает произвольное значение положения в шагах и микрошагах для шагового двигателя и в шагах энкодера всех двигателей. То есть меняется основной показатель положения.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_position(device_t id, urmc_set_position_t* input);
/**
    * \~english
    * Sets the current position and the position in which the traffic moves by the move command and movr zero for all cases, except for movement to the target position. In the latter case, set the zero current position and the target position counted so that the absolute position of the destination is the same. That is, if we were at 400 and moved to 500, then the command Zero makes the current position of 0, and the position of the destination - 100. Does not change the mode of movement that is if the motion is carried, it continues, and if the engine is in the "hold", the type of retention remains.
    * @param[in] id device identifier
    * \~russian
    * Устанавливает текущую позицию и позицию в которую осуществляется движение по командам move и movr равными нулю для всех случаев, кроме движения к позиции назначения. В последнем случае установить нулём текущую позицию, а позицию назначения пересчитать так, что в абсолютном положении точка назначения не меняется. То есть если мы находились в точке 400 и двигались к 500, то команда Zero делает текущую позицию 0, а позицию назначения - 100. Не изменяет режим движения т.е. если движение осуществлялось, то оно продолжается; если мотор находился в режиме "удержания", то тип удержания сохраняется.
    * @param[in] id идентификатор устройства
    */
URPC_VIS result_t URPC_CALLCONV urmc_command_zero(device_t id);
/**
    * \~english
    * Save all settings from controller's RAM to controller's flash memory, replacing previous data in controller's flash memory.
    * @param[in] id device identifier
    * \~russian
    * При получении команды контроллер выполняет операцию сохранения текущих настроек во встроенную энергонезависимую память контроллера.
    * @param[in] id идентификатор устройства
    */
URPC_VIS result_t URPC_CALLCONV urmc_save_settings(device_t id);
/**
    * \~english
    * Read all settings from controller's flash memory to controller's RAM, replacing previous data in controller's RAM.
    * @param[in] id device identifier
    * \~russian
    * Чтение всех настроек контроллера из flash памяти в оперативную, заменяя текущие настройки.
    * @param[in] id идентификатор устройства
    */
URPC_VIS result_t URPC_CALLCONV urmc_read_settings(device_t id);
/**
    * \~english
    * Return device state. Useful function that fills structure with snapshot of controller state, including speed, position and boolean flags.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Возвращает информацию о текущем состоянии устройства.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_status_impl(device_t id, urmc_status_impl_t* output);
/**
    * \~english
    * Start measurements and buffering of speed, following error.
    * @param[in] id device identifier
    * \~russian
    * Начать измерения и буферизацию скорости, ошибки следования.
    * @param[in] id идентификатор устройства
    */
URPC_VIS result_t URPC_CALLCONV urmc_start_measurements(device_t id);
/**
    * \~english
    * A command to read the data buffer to build a speed graph and a sequence error. Filling the buffer starts with the command "start_measurements". The buffer holds 25 points, the points are taken with a period of 1 ms. To create a robust system, read data every 20 ms, if the buffer is completely full, then it is recommended to repeat the readings every 5 ms until the buffer again becomes filled with 20 points.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Команда чтения буфера данных для построения графиков скорости и ошибки следования. Заполнение буфера начинается по команде "start_measurements". Буффер вмещает 25 точек, точки снимаются с периодом 1 мс. Для создания устойчивой системы следует считывать данные каждые 20 мс, если буффер полностью заполнен, то рекомендуется повторять считывания каждые 5 мс до момента пока буффер вновь не станет заполнен 20-ю точками.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_measurements(device_t id, urmc_measurements_t* output);
/**
    * \~english
    * Return device electrical parameters, useful for charts. Useful function that fill structure with snapshot of controller voltages and currents.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Команда чтения состояния обмоток и других не часто используемых данных. Предназначена в первую очередь для получения данных для построения графиков в паре с командой GETS.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_chart_data(device_t id, urmc_chart_data_t* output);
/**
    * \~english
    * Return device information. It's available from the firmware and bootloader.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Возвращает информацию об устройстве. Доступна как из прошивки, так и из бутлоадера.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_device_information_impl(device_t id, urmc_device_information_impl_t* output);
/**
    * \~english
    * Read device serial number.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Чтение серийного номера контроллера.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_serial_number(device_t id, urmc_get_serial_number_t* output);
/**
    * \~english
    * Read controller's firmware version.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Чтение номера версии прошивки контроллера.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_firmware_version(device_t id, urmc_firmware_version_t* output);
/**
    * \~english
    * This command resets all alarm state flags.
    * @param[in] id device identifier
    * \~russian
    * Сбрасывает все флаги аварийных состояний.
    * @param[in] id идентификатор устройства
    */
URPC_VIS result_t URPC_CALLCONV urmc_reset_alarm_flags(device_t id);
/**
    * \~english
    * This command reboots the controller to the bootloader. After receiving this command, the firmware sets a flag (for bootloader), sends reply and restarts the controller.
    * @param[in] id device identifier
    * \~russian
    * Команда служит для перезагрузки контроллера в загрузчик. Получив такую команду, прошивка платы устанавливает флаг (для загрузчика), отправляет ответ и перезагружает контроллер.
    * @param[in] id идентификатор устройства
    */
URPC_VIS result_t URPC_CALLCONV urmc_reboot_to_bootloader(device_t id);
/**
    * \~english
    * This command is deprecated. It is saved in the protocol for backwards compatibility. The new devices use the command rbld.
    * @param[in] id device identifier
    * \~russian
    * Эта команда считается устаревшей. Сохраняется в протоколе для обратной совместимости. В новых устройствах следует использовать команду rbld.
    * @param[in] id идентификатор устройства
    */
URPC_VIS result_t URPC_CALLCONV urmc_update_firmware(device_t id);
/**
    * \~english
    * Read analog data structure that contains raw analog data from ADC embedded on board. This function used for device testing and deep recalibraton by manufacturer only.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Чтение аналоговых данных, содержащих данные с АЦП и нормированные значения величин. Эта функция используется для тестирования и калибровки устройства.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_analog_data(device_t id, urmc_analog_data_t* output);
/**
    * \~english
    * Read data from firmware for debug purpose. Its use depends on context, firmware version and previous history.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Чтение данных из прошивки для отладки и поиска неисправностей. Получаемые данные зависят от версии прошивки, истории и контекста использования.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_debug_read(device_t id, urmc_debug_read_t* output);
/**
    * \~english
    * Write data to firmware for debug purpose.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Запись данных в прошивку для отладки и поиска неисправностей.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_debug_write(device_t id, urmc_debug_write_t* input);
/**
    * \~english
    * The command for clear controller FRAM. The memory is cleared by filling the whole memory byte 0x00. After cleaning, the controller restarts.
    * @param[in] id device identifier
    * \~russian
    * Команда очистки FRAM контроллера. Память очищается путем заполнения всего объема памяти байтами 0x00. После очистки контроллер перезагружается.
    * @param[in] id идентификатор устройства
    */
URPC_VIS result_t URPC_CALLCONV urmc_service_command_clear_fram_impl(device_t id);
/**
    * \~english
    * Read controller's firmware version.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Чтение номера версии прошивки контроллера.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_bootloader_version(device_t id, urmc_bootloader_version_t* output);
/**
    * \~english
    * The reset command controller.
    * @param[in] id device identifier
    * \~russian
    * Команда перезагрузки контроллера.
    * @param[in] id идентификатор устройства
    */
URPC_VIS result_t URPC_CALLCONV urmc_service_command_reset_impl(device_t id);
/**
    * \~english
    * Change motor - command for switching output relay.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Сменить двигатель - команда для переключения выходного реле.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_change_motor(device_t id, urmc_change_motor_t* input);
/**
    * \~english
    * Feedback settings.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Чтение настроек обратной связи
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_feedback_settings(device_t id, urmc_feedback_settings_t* output);
/**
    * \~english
    * Feedback settings.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Запись настроек обратной связи.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_feedback_settings(device_t id, urmc_feedback_settings_t* input);
/**
    * \~english
    * Read home settings. This function fill structure with settings of calibrating position.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Команда чтения настроек для подхода в home position. Эта функция заполняет структуру настроек, использующихся для калибровки позиции, в память контроллера.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_home_settings(device_t id, urmc_home_settings_t* output);
/**
    * \~english
    * Set home settings. This function send structure with calibrating position settings to controller's memory.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Команда записи настроек для подхода в home position. Эта функция записывает структуру настроек, использующихся для калибровки позиции, в память контроллера.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_home_settings(device_t id, urmc_home_settings_t* input);
/**
    * \~english
    * Read command setup movement (speed, acceleration, threshold and etc).
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Команда чтения настроек перемещения (скорость, ускорение, threshold и скорость в режиме антилюфта).
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_move_settings(device_t id, urmc_move_settings_t* output);
/**
    * \~english
    * Set command setup movement (speed, acceleration, threshold and etc).
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Команда записи настроек перемещения (скорость, ускорение, threshold и скорость в режиме антилюфта).
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_move_settings(device_t id, urmc_move_settings_t* input);
/**
    * \~english
    * Read engine settings. This function fill structure with set of useful motor settings stored in controller's memory. These settings specify motor shaft movement algorithm, list of limitations and rated characteristics.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Чтение настроек мотора. Настройки определяют номинальные значения напряжения, тока, скорости мотора, характер движения и тип мотора. Пожалуйста, загружайте новые настройки когда вы меняете мотор, энкодер или позиционер. Помните, что неправильные настройки мотора могут повредить оборудование.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_engine_settings(device_t id, urmc_engine_settings_t* output);
/**
    * \~english
    * Set engine settings. This function send structure with set of engine settings to controller's memory. These settings specify motor shaft movement algorithm, list of limitations and rated characteristics. Use it when you change motor, encoder, positioner etc. Please note that wrong engine settings lead to device malfunction, can lead to irreversible damage of board.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Запись настроек мотора. Настройки определяют номинальные значения напряжения, тока, скорости мотора, характер движения и тип мотора. Пожалуйста, загружайте новые настройки когда вы меняете мотор, энкодер или позиционер. Помните, что неправильные настройки мотора могут повредить оборудование.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_engine_settings(device_t id, urmc_engine_settings_t* input);
/**
    * \~english
    * Return engine type and driver type.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Возвращает информацию о типе мотора и силового драйвера.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_entype_settings(device_t id, urmc_entype_settings_t* output);
/**
    * \~english
    * Set engine type and driver type.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Запись информации о типе мотора и типе силового драйвера.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_entype_settings(device_t id, urmc_entype_settings_t* input);
/**
    * \~english
    * Read settings of step motor power control. Used with stepper motor only.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Команда чтения параметров питания мотора. Используется только с шаговым двигателем. Используется только с шаговым двигателем.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_power_settings(device_t id, urmc_power_settings_t* output);
/**
    * \~english
    * Set settings of step motor power control. Used with stepper motor only.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Команда записи параметров питания мотора. Используется только с шаговым двигателем.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_power_settings(device_t id, urmc_power_settings_t* input);
/**
    * \~english
    * Read protection settings.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Команда записи установок защит.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_secure_settings(device_t id, urmc_secure_settings_t* output);
/**
    * \~english
    * Set protection settings.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Команда записи установок защит.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_secure_settings(device_t id, urmc_secure_settings_t* input);
/**
    * \~english
    * Read border and limit switches settings.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Чтение настроек границ и концевых выключателей.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_edges_settings(device_t id, urmc_edges_settings_t* output);
/**
    * \~english
    * Set border and limit switches settings.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Запись настроек границ и концевых выключателей.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_edges_settings(device_t id, urmc_edges_settings_t* input);
/**
    * \~english
    * Read PID settings. This function fill structure with set of motor PID settings stored in controller's memory. These settings specify behaviour of PID routine for positioner. These factors are slightly different for different positioners. All boards are supplied with standard set of PID setting on controller's flash memory.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Чтение ПИД коэффициентов. Эти коэффициенты определяют поведение позиционера. Коэффициенты различны для разных позиционеров.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_pid_settings(device_t id, urmc_pid_settings_t* output);
/**
    * \~english
    * Set PID settings. This function send structure with set of PID factors to controller's memory. These settings specify behaviour of PID routine for positioner. These factors are slightly different for different positioners. All boards are supplied with standard set of PID setting on controller's flash memory. Please use it for loading new PID settings when you change positioner. Please note that wrong PID settings lead to device malfunction.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Запись ПИД коэффициентов. Эти коэффициенты определяют поведение позиционера. Коэффициенты различны для разных позиционеров. Пожалуйста, загружайте новые настройки, когда вы меняете мотор или позиционер.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_pid_settings(device_t id, urmc_pid_settings_t* input);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_sync_in_settings(device_t id, urmc_sync_in_settings_t* output);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_sync_in_settings(device_t id, urmc_sync_in_settings_t* input);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_sync_out_settings(device_t id, urmc_sync_out_settings_t* output);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_sync_out_settings(device_t id, urmc_sync_out_settings_t* input);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_extio_settings(device_t id, urmc_extio_settings_t* output);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_extio_settings(device_t id, urmc_extio_settings_t* input);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_brake_settings(device_t id, urmc_brake_settings_t* output);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_brake_settings(device_t id, urmc_brake_settings_t* input);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_control_settings(device_t id, urmc_control_settings_t* output);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_control_settings(device_t id, urmc_control_settings_t* input);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_joystick_settings(device_t id, urmc_joystick_settings_t* output);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_joystick_settings(device_t id, urmc_joystick_settings_t* input);
/**
    * \~english
    * Read settings of control position(is only used with stepper motor). When controlling the step motor with encoder (CTP_BASE 0) it is possible to detect the loss of steps. The controller knows the number of steps per revolution (GENG :: StepsPerRev) and the encoder resolution (GFBS :: IPT). When the control (flag CTP_ENABLED), the controller stores the current position in the footsteps of SM and the current position of the encoder. Further, at each step of the position encoder is converted into steps and if the difference is greater CTPMinError, a flag STATE_CTP_ERROR. When controlling the step motor with speed sensor (CTP_BASE 1), the position is controlled by him. The active edge of input clock controller stores the current value of steps. Further, at each turn checks how many steps shifted. When a mismatch CTPMinError a flag STATE_CTP_ERROR.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Чтение настроек контроля позиции(для шагового двигателя). При управлении ШД с энкодером (CTP_BASE 0) появляется возможность обнаруживать потерю шагов. Контроллер знает кол-во шагов на оборот (GENG::StepsPerRev) и разрешение энкодера (GFBS::IPT). При включении контроля (флаг CTP_ENABLED), контроллер запоминает текущую позицию в шагах ШД и текущую позицию энкодера. Далее, на каждом шаге позиция энкодера преобразовывается в шаги и если разница оказывается больше CTPMinError, устанавливается флаг STATE_CTP_ERROR. При управлении ШД с датчиком оборотов (CTP_BASE 1), позиция контролируется по нему. По активному фронту на входе синхронизации контроллер запоминает текущее значение шагов. Далее, при каждом обороте проверяет, на сколько шагов сместились. При рассогласовании более CTPMinError устанавливается флаг STATE_CTP_ERROR.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_ctp_settings(device_t id, urmc_ctp_settings_t* output);
/**
    * \~english
    * Set settings of control position(is only used with stepper motor). When controlling the step motor with encoder (CTP_BASE 0) it is possible to detect the loss of steps. The controller knows the number of steps per revolution (GENG :: StepsPerRev) and the encoder resolution (GFBS :: IPT). When the control (flag CTP_ENABLED), the controller stores the current position in the footsteps of SM and the current position of the encoder. Further, at each step of the position encoder is converted into steps and if the difference is greater CTPMinError, a flag STATE_CTP_ERROR. When controlling the step motor with speed sensor (CTP_BASE 1), the position is controlled by him. The active edge of input clock controller stores the current value of steps. Further, at each turn checks how many steps shifted. When a mismatch CTPMinError a flag STATE_CTP_ERROR.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Запись настроек контроля позиции(для шагового двигателя). При управлении ШД с энкодером (CTP_BASE 0) появляется возможность обнаруживать потерю шагов. Контроллер знает кол-во шагов на оборот (GENG::StepsPerRev) и разрешение энкодера (GFBS::IPT). При включении контроля (флаг CTP_ENABLED), контроллер запоминает текущую позицию в шагах ШД и текущую позицию энкодера. Далее, на каждом шаге позиция энкодера преобразовывается в шаги и если разница оказывается больше CTPMinError, устанавливается флаг STATE_CTP_ERROR. При управлении ШД с датчиком оборотов (CTP_BASE 1), позиция контролируется по нему. По активному фронту на входе синхронизации контроллер запоминает текущее значение шагов. Далее, при каждом обороте проверяет, на сколько шагов сместились. При рассогласовании более CTPMinError устанавливается флаг STATE_CTP_ERROR.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_ctp_settings(device_t id, urmc_ctp_settings_t* input);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_uart_settings(device_t id, urmc_uart_settings_t* output);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_uart_settings(device_t id, urmc_uart_settings_t* input);
/**
    * \~english
    * Read user controller name and flags of setting from FRAM.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Чтение пользовательского имени контроллера и настроек из FRAM.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_controller_name(device_t id, urmc_controller_name_t* output);
/**
    * \~english
    * Write user controller name and flags of setting from FRAM.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Запись пользовательского имени контроллера и настроек в FRAM.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_controller_name(device_t id, urmc_controller_name_t* input);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_nonvolatile_memory(device_t id, urmc_nonvolatile_memory_t* output);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_nonvolatile_memory(device_t id, urmc_nonvolatile_memory_t* input);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_stage_name(device_t id, urmc_stage_name_t* output);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_stage_name(device_t id, urmc_stage_name_t* input);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_stage_information(device_t id, urmc_stage_information_t* output);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_stage_information(device_t id, urmc_stage_information_t* input);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_stage_settings(device_t id, urmc_stage_settings_t* output);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_stage_settings(device_t id, urmc_stage_settings_t* input);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_motor_information(device_t id, urmc_motor_information_t* output);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_motor_information(device_t id, urmc_motor_information_t* input);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_motor_settings(device_t id, urmc_motor_settings_t* output);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_motor_settings(device_t id, urmc_motor_settings_t* input);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_encoder_information(device_t id, urmc_encoder_information_t* output);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_encoder_information(device_t id, urmc_encoder_information_t* input);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_encoder_settings(device_t id, urmc_encoder_settings_t* output);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_encoder_settings(device_t id, urmc_encoder_settings_t* input);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_hallsensor_information(device_t id, urmc_hallsensor_information_t* output);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_hallsensor_information(device_t id, urmc_hallsensor_information_t* input);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_hallsensor_settings(device_t id, urmc_hallsensor_settings_t* output);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_hallsensor_settings(device_t id, urmc_hallsensor_settings_t* input);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_gear_information(device_t id, urmc_gear_information_t* output);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_gear_information(device_t id, urmc_gear_information_t* input);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_gear_settings(device_t id, urmc_gear_settings_t* output);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_gear_settings(device_t id, urmc_gear_settings_t* input);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[out] output Function call result storage
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[out] output Хранилище для результатов вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_get_accessories_settings(device_t id, urmc_accessories_settings_t* output);
/**
    * \~english
    * Not supported on your device.
    * @param[in] id device identifier
    * @param[in] input Function call arguments
    * \~russian
    * Не поддерживается на вашем устройстве.
    * @param[in] id идентификатор устройства
    * @param[in] input Входные аргументы вызова
    */
URPC_VIS result_t URPC_CALLCONV urmc_set_accessories_settings(device_t id, urmc_accessories_settings_t* input);

/**
	* \~english
	* Open a device by URI \a and returns identifier of the device which can be used in calls.
	* @param[in] name a device URI
	* Device name has form "com:path", or "xi-net://host/serial" for network device.
    * Now in case of virtual port open_device returns only one virtual device at each call.
	* In case of USB-COM port the "path" is the OS device name.
    * In case of network devices host is IP address or DNS-resolvable host name and serial is some implementation-defined device id on the server.
	* For example:
    *   "com:///\\.\COM12",
    *   "com:\\.\COM12",
    *   "com:///dev/tty/ttyACM34",
    *   "xi-net://172.16.3.185/00000008"
	* \~russian
	* Открывает устройство по URI \a и возвращает идентификатор, который будет использоваться для обращения к устройству.
	* @param[in] name - URI устройства
	* Имя устройства имеет вид "com://path" или "xi-net://host/serial" для сетевых устройств.
    * Сейчас open_device возвращает только одно устройство при каждом вызове.
	* Для USB-COM устройства "path" это имя устройства в ОС.
    * Для сетевыы устройств "host" это либо IP-адрес, либо DNS-имя, а serial - зависящий от реализации сервера идентификатор устройства.
	* Например:
    *   "com:///\\.\COM12",
    *   "com:\\.\COM12",
    *   "com:///dev/tty/ttyACM34",
    *   "xi-net://172.16.3.185/00000008"
	*/
URPC_VIS device_t URPC_CALLCONV urmc_open_device(const char* name);

/**
	* \~english
	* Close specified device
	* @param id an identifier of device
	* \~russian
	* Закрывает устройство
	* @param id - идентификатор устройства
	*/
URPC_VIS result_t URPC_CALLCONV urmc_close_device(device_t* id);

/**
	* \~english
	* Reset library locks in a case of deadlock.
	* \~russian
	* Снимает блокировку библиотеки в экстренном случае.
	*/
URPC_VIS result_t URPC_CALLCONV urmc_reset_locks();

/**
	* \~english
	* Fix for errors in Windows USB driver stack.
	* USB subsystem on Windows does not always work correctly. The following bugs are possible:
	* the device cannot be opened at all, or
	* the device can be opened and written to, but it will not respond with data.
	* These errors can be fixed by device reconnection or removal-rescan in device manager.
	* urmc_fix_usbser_sys() is a shortcut function to do the remove-rescan process.
	* You should call this function if urmc library cannot open the device which was not physically removed from the system or if the device does not respond.
	* \~russian
	* Исправление ошибки драйвера USB в Windows.
	* Подсистема USB-COM на Windows не всегда работает корректно. При работе возможны следующие неисправности:
	* все попытки открыть устройство заканчиваются неудачно, или
	* устройство можно открыть и писать в него данные, но в ответ данные не приходят.
	* Эти проблемы лечатся переподключением устройства или удалением и повторным поиском устройства в диспетчере устройств.
	* Функция urmc_fix_usbser_sys() автоматизирует процесс удаления-обнаружения.
	* Имеет смысл вызывать эту функцию, если библиотека не может открыть устройство, при том что оно физически не было удалено из системы, или если устройство не отвечает.
	*/
URPC_VIS result_t URPC_CALLCONV urmc_fix_usbser_sys(const char* device_name);

/**
	* \~english
	* Sleeps for a specified amount of time
	* @param msec time in milliseconds
	* \~russian
	* Приостанавливает работу на указанное время
	* @param msec время в миллисекундах
	*/
URPC_VIS void URPC_CALLCONV msec_sleep (unsigned int msec);

/**
	* \~english
	* Returns a library version
	* @param version a buffer to hold a version string, 32 bytes is enough
	* \~russian
	* Возвращает версию библиотеки
	* @param version буфер для строки с версией, 32 байт достаточно
	*/
URPC_VIS void URPC_CALLCONV urmc_version (char* version);

/**
	* \~english
	* Load profile from device
	* @param[in] device_id - device id
	* @param[out] buffer - pointer to output char* buffer. Memory for char* pointer must be allocated
	* @param[out] allocate - function for memory allocate
	* \~russian
	* Загружает профиль с устройства
	* @param[in] device_id - идентификатор устройства
	* @param[out] buffer -  адрес указателя на выходной буфер. Память для указателя на char* должна быть выделена
	* @param[out] allocate - функция для выделения памяти
	*/
URPC_VIS result_t URPC_CALLCONV urmc_get_profile(device_t device_id, char** buffer, void *(*allocate)(size_t));

/**
	* \~english
	* Save profile to device
	* @param[in] device_id - device id
	* @param[in] buffer - input char* buffer
	* \~russian
	* Загружает профиль с устройства
	* @param[in] device_id - идентификатор устройства
	* @param[in] buffer - входной буфер, откуда будет считан профиль
	*/
URPC_VIS result_t URPC_CALLCONV urmc_set_profile(device_t device_id, char *buffer);

#if !defined(MATLAB_IMPORT) && !defined(LABVIEW64_IMPORT) && !defined(LABVIEW32_IMPORT)
#include <wchar.h>
/**
	* \~english
	* Logging callback prototype
	* @param loglevel a loglevel
	* @param message a message
	* \~russian
	* Прототип функции обратного вызова для логирования
	* @param loglevel уровень логирования
	* @param message сообщение
	*/
typedef void (URPC_CALLCONV *urmc_logging_callback_t)(int loglevel, const wchar_t* message, void* user_data);

/**
	* \~english
	* Simple callback for logging to stderr in wide chars
	* @param loglevel a loglevel
	* @param message a message
	* \~russian
	* Простая функция логирования на stderr в широких символах
	* @param loglevel уровень логирования
	* @param message сообщение
	*/
URPC_VIS void URPC_CALLCONV urmc_logging_callback_stderr_wide(int loglevel, const wchar_t* message, void* user_data);

/**
	* \~english
	* Simple callback for logging to stderr in narrow (single byte) chars
	* @param loglevel a loglevel
	* @param message a message
	* \~russian
	* Простая функция логирования на stderr в узких (однобайтных) символах
	* @param loglevel уровень логирования
	* @param message сообщение
	*/
URPC_VIS void URPC_CALLCONV urmc_logging_callback_stderr_narrow(int loglevel, const wchar_t* message, void* user_data);

/**
	* \~english
	* Sets a logging callback.
	* Call resets a callback to default (stderr, syslog) if NULL passed.
	* @param logging_callback a callback for log messages
	* \~russian
	* Устанавливает функцию обратного вызова для логирования.
	* Вызов назначает стандартный логгер (stderr, syslog), если передан NULL
	* @param logging_callback указатель на функцию обратного вызова
	*/
URPC_VIS void URPC_CALLCONV urmc_set_logging_callback(urmc_logging_callback_t logging_callback, void* user_data);

#endif

#ifdef __cplusplus
}
#endif
#endif
