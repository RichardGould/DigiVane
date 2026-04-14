/*  Definition Section  */
// Small edit by Jim 28/02/2026 17:38 (line 36)

#define PCF8574 0x27
#define sda D2
#define scl D1
#define freq 100000

#define MSG_BUFFER_SIZE 50
#define MQTT_SIZE 20

#define SizePass 20

/*  variable instantiation  */

const char* Version       = "1.0";

/*  Network Credentials */
const char* Home_WiFi     = "GOULD_TP";
char Home_Pass[SizePass]  = "pr`rxgvx";
const char* Home_MQTT     = "192.168.1.178";

const char* MiS_WiFi      = "BTB-NTCHT6";
char MiS_Pass[SizePass]   = "RHCagINtyI}`k>";
const char* MiS_MQTT      = "192.168.1.249";

const char* Jim_WiFi      = "BT-S7AT5Q";
char Jim_Pass[SizePass]   = "vHqbS8{:YqKX@q";
const char* Jim_MQTT      = "192.168.1.165";

const char* Rich_WiFi     = "GOULDWAN24";
char Rich_Pass[SizePass]  = "Tgkf2;47cd";
const char* Rich_MQTT     = "192.168.1.178";

const char* MiS_BASE      = "MiS";
const char* MiS_DEVICE    = "VANE";
const char* OUT_TOPIC     = "Data";
const char* RST_TOPIC     = "RST";
const char* STAT_IN_TOPIC = "STAT/IN";
const char* STAT_OUT_TOPIC = "STAT/OUT";
const char* DEV_TOPIC     = "DEVICES";
const char* TIME_TOPIC    = "TIME";

int MQTT_PORT = 1883, MQTT_in_flag, MQTT_in_length;
char MQTT_server[20];
char MQTT_PUB[200], MQTT_IN[MQTT_SIZE], MQTT_OUT[MQTT_SIZE], MQTT_RST[MQTT_SIZE], MQTT_STATIN[MQTT_SIZE];
char MQTT_STATOUT[ MQTT_SIZE], MiS_HEAD[10], MQTT_DEVICES[ 30 ];
char MQTT_in_buffer[MSG_BUFFER_SIZE];
char MQTT_in_topic[20], MQTT_TIME[ MQTT_SIZE ];

int dir[ 30 ], sequence = 0;

char my_dir[ 10 ];
char WiFi_Pass[20];
char PUB_message[MSG_BUFFER_SIZE], SUB_message[MSG_BUFFER_SIZE];

char my_IP_Address[20], my_MAC_Address[20];

unsigned long epoch, pulse, payload_time;
uint8_t i = 0, rtn = 0, count = 0;
int net, network, networks;
int in, now, then, pa, period, time_count = 0;
char * q;

int ON  = 1;
int OFF = 0;

unsigned long ota_progress_millis = 0;

/*  time variables */
char a_timestamp[ 20 ], a_year[ 6 ], a_month[ 4 ], a_day[ 4 ], a_hour[ 4 ], a_minute[ 4 ], a_second[ 4 ];
char a_date[ 32 ] = "01/01/1970 00:00:00";
long int timestamp;
int year, month, day, hour, minute, second;
int servo = 0;
int time_flag = 0, msg_flag = 0;
