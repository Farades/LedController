#include <IRremote.h>
#include <IRremoteInt.h>
#include <EEPROM.h>

#define IR_PIN        A5  //PIN данных ИК фототранзистора
#define RED_PIN       10   //PIN управления транзистором красного канала
#define GREEN_PIN     11   //PIN управления транзистором зеленого канала
#define BLUE_PIN      9   //PIN управления транзистором синего канала
#define AUTO_MODE_LED_PIN  3   //PIN сигнализирующий о включении авто мода
#define PHOTO_RES_PIN A0  //PIN аналогового входа фоторезистора 
#define LIGHT_SW_ON   200 //Порог включения светодиодной ленты
#define LIGHT_SW_OFF  100 //Порог выключения светодиодной ленты

#define BUT_OFF 		0xF740BF
#define BUT_ON  		0xF7C03F
#define BUT_W   		0xF7E01F
#define BUT_R   		0xF720DF
#define BUT_G       0xF7A05F
#define BUT_B       0xF7609F
#define BUT_FLASH   0xF7D02F

#define BUT_1   0xF710EF
#define BUT_2   0xF7906F
#define BUT_3   0xF750AF
#define BUT_4   0xF730CF
#define BUT_5   0xF7B04F
#define BUT_6   0xF7708F
#define BUT_7   0xF708F7
#define BUT_8   0xF78877
#define BUT_9   0xF748B7
#define BUT_10   0xF728D7
#define BUT_11   0xF7A857
#define BUT_12   0xF76897

#define RED_ADDR 1
#define GREEN_ADDR 2
#define BLUE_ADDR 3
#define AUTO_MODE_ADDR 4

const int RED   = 0;
const int GREEN = 1;
const int BLUE  = 2;
const int WHITE  = 3;
const int BLACK  = 4;
const int COL_1  = 5;
const int COL_2  = 6;
const int COL_3  = 7;
const int COL_4  = 8;
const int COL_5  = 9;
const int COL_6  = 10;
const int COL_7  = 11;
const int COL_8  = 12;
const int COL_9  = 13;
const int COL_10  = 14;
const int COL_11  = 15;
const int COL_12  = 16;


boolean autoMode = false;
int val = 0; //значение фоторезистора

IRrecv irrecv(IR_PIN);
decode_results results;

void setup() {
	pinMode(RED_PIN, OUTPUT);
	pinMode(GREEN_PIN, OUTPUT);
	pinMode(BLUE_PIN, OUTPUT);
	pinMode(AUTO_MODE_LED_PIN, OUTPUT);
	pinMode(PHOTO_RES_PIN, INPUT);

	initRGBFromEEPROM();

	//Частота ШИМ
	// TCCR1B = TCCR1B & 0b11111000 | 0x03;
	// TCCR2B = TCCR2B & 0b11111000 | 0x04;

	Serial.begin(9600);
	irrecv.enableIRIn(); 
}

void initRGBFromEEPROM() {
	byte r = EEPROM.read(RED_ADDR);
	byte g = EEPROM.read(GREEN_ADDR);
	byte b = EEPROM.read(BLUE_ADDR);
	autoMode = EEPROM.read(AUTO_MODE_ADDR);
	if (autoMode) {
		digitalWrite(AUTO_MODE_LED_PIN, HIGH);
	} else {
		changeColor(r, g, b);
	}
}

void loop() {
	if (autoMode) {
		checkLightLevel();
	}
	checkIR();
	
}

void checkIR() {
	if (irrecv.decode(&results)) {
		switch (results.value) {
		    case BUT_OFF:
		      Serial.println("Button Off");
					colorProcessing(BLACK);
		      break;
		    case BUT_ON:
		      Serial.println("Button On");
					colorProcessing(WHITE);
		      break;
		    case BUT_W:
		      Serial.println("WHITE mode on");
					colorProcessing(WHITE);
		      break;
		    case BUT_R:
		      Serial.println("Red mode on");
					colorProcessing(RED);
		      break;
		    case BUT_G:
		      Serial.println("Green mode on");
					colorProcessing(GREEN);
		      break;
		    case BUT_B:
		      Serial.println("Blue mode on");
					colorProcessing(BLUE);
		      break;
		    case BUT_FLASH:
					changeMode();
		      break;
		    case BUT_1:
		    	buttonPressMsg("1");
					//colorProcessing(COL_1);
		      break;
		   //  case BUT_2:
		   //  	buttonPressMsg("2");
					// colorProcessing(COL_2);
		   //    break;
		   //  case BUT_3:
		   //    buttonPressMsg("3");
					// colorProcessing(COL_3);
		   //    break;
		   //  case BUT_4:
		   //    buttonPressMsg("4");
					// colorProcessing(COL_4);
		   //    break;
		   //  case BUT_5:
		   //    buttonPressMsg("5");
					// colorProcessing(COL_5);
		   //    break;
		   //  case BUT_6:
		   //    buttonPressMsg("6");
					// colorProcessing(COL_6);
		   //    break;
		   //  case BUT_7:
		   //    buttonPressMsg("7");
					// colorProcessing(COL_7);
		   //    break;
		   //  case BUT_8:
		   //    buttonPressMsg("8");
					// colorProcessing(COL_8);
		   //    break;
		   //  case BUT_9:
		   //    buttonPressMsg("9");
					// colorProcessing(COL_9);
		   //    break;
		   //  case BUT_10:
		   //    buttonPressMsg("10");
					// colorProcessing(COL_10);
		   //    break;
		   //  case BUT_11:
		   //    buttonPressMsg("11");
					// colorProcessing(COL_11);
		   //    break;
		   //  case BUT_12:
		   //    buttonPressMsg("12");
					// colorProcessing(COL_12);
		   //    break;
		    default:
		    ;
		      // do something
		}

		Serial.println(results.value, HEX);
		irrecv.resume();
	}
}

void buttonPressMsg(String button) {
	Serial.print("Button "); Serial.print(button); Serial.println(" pressed");
} 

void changeMode() {
	Serial.println("Change mode");
	if (autoMode) {
		autoMode = !autoMode;
		digitalWrite(AUTO_MODE_LED_PIN, LOW);
		EEPROM.write(AUTO_MODE_ADDR, 0);
	} else {
		autoMode = !autoMode;
		digitalWrite(AUTO_MODE_LED_PIN, HIGH);
		EEPROM.write(AUTO_MODE_ADDR, 1);
	}
}

void checkLightLevel() {
	val = analogRead(PHOTO_RES_PIN);
	if (val < 100) {
		ledOn();
	} else if (val > 200) {
		ledOff();
	}
}



void colorProcessing(int color) {
	if (autoMode) {
		changeMode();
	}

	switch (color) {
	    case RED:
	      changeColor(255, 0, 0);
	      break;
	    case GREEN:
	      changeColor(0, 255, 0);
	      break;
			case BLUE:
				changeColor(0, 0, 255);
				break;
			case WHITE:
				changeColor(255, 255, 255);
				break;
			case BLACK:
				changeColor(0, 0, 0);
				break;
			case COL_1:
				changeColor(208, 69, 20);
				break;
			case COL_2:
				changeColor(0, 157, 60);
				break;
		  case COL_3:
				changeColor(0, 58, 165);
				break;
		  case COL_4:
				changeColor(255, 135, 0);
				break;
		  case COL_5:
				changeColor(0, 150, 154);
				break;
			case COL_6:
				changeColor(41, 30, 72);
				break;
			case COL_7:
				changeColor(222, 145, 31);
				break;
			case COL_8:
				changeColor(0, 103, 134);
				break;
			case COL_9:
				changeColor(79, 38, 80);
				break;
			case COL_10:
				changeColor(255, 255, 0);
				break;
			case COL_11:
				changeColor(0, 79, 138);
				break;
		  case COL_12:
				changeColor(175, 46, 134);
				break;
	    default:
	      ;
	}
}

void changeColor(byte r, byte g, byte b) {
	analogWrite(RED_PIN, r);
	analogWrite(GREEN_PIN, g);
	analogWrite(BLUE_PIN, b);

	Serial.println(r);
	Serial.println(g);
	Serial.println(b);

	//saveRGBToEEPROM(r, g, b);
}

void saveRGBToEEPROM(byte r, byte g, byte b) {
	EEPROM.write(RED_ADDR, r);
	EEPROM.write(GREEN_ADDR, g);
	EEPROM.write(BLUE_ADDR, b);
}

void ledOn() {
		analogWrite(RED_PIN, 255);
		analogWrite(GREEN_PIN, 255);
		analogWrite(BLUE_PIN, 255);
}

void ledOff() {
	analogWrite(RED_PIN, 0);
	analogWrite(GREEN_PIN, 0);
	analogWrite(BLUE_PIN, 0);
}