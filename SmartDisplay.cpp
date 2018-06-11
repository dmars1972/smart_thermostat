#include "SmartDisplay.h"

SmartDisplay::SmartDisplay(Settings *settings, SmartRoom r[])
:weatherIcon(10, 10, 50, 50),
 outsideTempText(6, 80, 25, 54),
 timeText (70, 20, 38, 180),
 setTempLabel(122, 206, 34, 115),
 setTempText (232, 195, 34, 26*3),
 settingsIcon (260, 10, 35, 35)
{
  s = settings;
  rooms = r;

  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  card.init(SPI_FULL_SPEED, SD_CS); 
    
  if(!SD.begin(SD_CS)) {
    Serial.println("SD card failure!");
    while(1);
  }

  hasNotification = false;

  return;
}

bool SmartDisplay::begin()
{
  String str;
  if(!spitouch.begin()) {
    Serial.println("STMPE not found.");
    return false;
  }

  spitouch.begin();

  // backlight on
  spitouch.writeRegister8(STMPE_GPIO_SET_PIN, _BV(2));

  tft.begin();
  tft.setTextWrap(false);
  tft.fillScreen(ILI9341_BLACK);

  tft.setRotation(3);

  outsideTempText.setTextColor(ILI9341_CYAN);
  outsideTempText.setAlignment(CENTER);
  outsideTempText.setBackgroundColor(backgroundColor);
  outsideTempText.setFontSize((unsigned char)18);
  outsideTempText.setAlignment(CENTER);

  timeText.setTextColor(ILI9341_CYAN);
  timeText.setBackgroundColor(backgroundColor);
  timeText.setFontSize((unsigned char)24);
  timeText.setHorizontalAlignment(H_CENTER);
  timeText.setAlignment(CENTER);

  setTempLabel.setTextColor(ILI9341_DARKCYAN);

  setTempLabel.setAlignment(RIGHT);
  setTempLabel.setBackgroundColor(backgroundColor);
  setTempLabel.setFontSize((unsigned char)12);
  setTempLabel.setText(&tft, "Set Temp:");

  setTempText.setTextColor(ILI9341_CYAN);
  setTempText.setBackgroundColor(backgroundColor);
  setTempText.setFontSize((unsigned char)24);
  setTempText.setAlignment(RIGHT);

  weatherIcon.setBackgroundColor(backgroundColor);

  settingsIcon.setBackgroundColor(backgroundColor);

  settingsIcon.setImage(&tft, "settings.bmp");
  
  return true;
}

void SmartDisplay::setTextColor(uint16_t c)
{
  textColor = c;

  tft.setTextColor(textColor, backgroundColor);

  return;
}

void SmartDisplay::setBackgroundColor(uint16_t c)
{
  backgroundColor = c;

  tft.setTextColor(textColor, backgroundColor);

  return;
}

void SmartDisplay::setWeatherIcon(char *i)
{
  sprintf(imagefile, "%s.bmp", i);

  weatherIcon.setImage(&tft, imagefile);

  return;
}

void SmartDisplay::setOutsideTemp(char *t)
{
  outsideTempText.setText(&tft, t);

  return;
}

void SmartDisplay::setSetTemp(unsigned char t)
{
  char temp[4];

  sprintf(temp, "%d", t);

  setTempText.setText(&tft, temp);

  return;
}

void SmartDisplay::setTime(char *t)
{
  timeText.setText(&tft, t);

  return;
}

void SmartDisplay::handleTouch()
{
  TS_Point p;
  int touchx;
  int touchy;

  if (!spitouch.bufferEmpty()) {
    // read x & y & z;
    p = spitouch.getPoint();

    touchx = map(p.y, TS_MAXY, TS_MINY, 0, tft.width()); 
    touchy = map(p.x, TS_MAXX, TS_MINX, 0, tft.height());

    // clear the buffer
    clearBuffer();

    if(weatherIcon.touched(touchx, touchy))
      weatherIconTouched();
    else if(outsideTempText.touched(touchx, touchy))
      weatherIconTouched();
    else if(timeText.touched(touchx, touchy))
      timeTouched();
    else if(setTempText.touched(touchx, touchy))
      setTempTouched();
    else if(settingsIcon.touched(touchx, touchy))
      settingsTouched();
  }

  return;
}

void SmartDisplay::weatherIconTouched()
{
  Serial.println("Weather Icon callback");
} 

void SmartDisplay::timeTouched()
{
  Serial.println("Time callback");
}

void SmartDisplay::setTempTouched()
{
  TS_Point p;
  uint16_t touchx, touchy;
  bool OK = false;
  unsigned char setTemp = s->getCurrentSetTemp();
  char t[4];

  sprintf(t, "%d", setTemp);

  tft.fillScreen(backgroundColor);

  TextRect currentBox(110, 63, 50, 70);
  TextRect downBox(50, 63, 50, 50);
  TextRect upBox(195, 63, 50, 50);
  TextRect okBox(50, 153, 50, 80);
  TextRect cancelBox(245-80, 153, 50, 80);

  currentBox.setBackgroundColor(backgroundColor);
  currentBox.setFontSize((unsigned char)18);
  currentBox.setTextColor(ILI9341_CYAN);
  currentBox.setAlignment(CENTER);
  currentBox.setHorizontalAlignment(H_CENTER);
  currentBox.setText(&tft, t);

  upBox.setBackgroundColor(backgroundColor);
  upBox.setFontSize((unsigned char)18);
  upBox.setTextColor(ILI9341_BLACK);
  upBox.setAlignment(CENTER);
  upBox.setButton(&tft, ">", ILI9341_CYAN, ILI9341_DARKCYAN, ILI9341_BLACK);

  downBox.setBackgroundColor(backgroundColor);
  downBox.setFontSize((unsigned char)18);
  downBox.setTextColor(ILI9341_BLACK);
  downBox.setAlignment(CENTER);
  downBox.setButton(&tft, "<", ILI9341_CYAN, ILI9341_DARKCYAN, ILI9341_BLACK);

  okBox.setBackgroundColor(backgroundColor);
  okBox.setFontSize((unsigned char)12);
  okBox.setTextColor(ILI9341_BLACK);
  okBox.setAlignment(CENTER);
  okBox.setButton(&tft, "OK", ILI9341_CYAN, ILI9341_DARKCYAN, ILI9341_BLACK);

  cancelBox.setBackgroundColor(backgroundColor);
  cancelBox.setFontSize((unsigned char)12);
  cancelBox.setTextColor(ILI9341_BLACK);
  cancelBox.setAlignment(CENTER);
  cancelBox.setButton(&tft, "Cancel", ILI9341_CYAN, ILI9341_DARKCYAN, ILI9341_BLACK);

  cancelBox.onTouch([]() {
    Serial.println("Hit cancel");
  });

// empty the buffer
  clearBuffer();

  do {
    if (!spitouch.bufferEmpty()) {
      p = spitouch.getPoint();

      touchx = map(p.y, TS_MAXY, TS_MINY, 0, tft.width()); 
      touchy = map(p.x, TS_MAXX, TS_MINX, 0, tft.height());

      if(upBox.touched(touchx, touchy)) {
        setTemp++;
        sprintf(t, "%d", setTemp);
        currentBox.setText(&tft, t);
      } else if(downBox.touched(touchx, touchy)) {
        setTemp--;
        sprintf(t, "%d", setTemp);
        currentBox.setText(&tft, t);
      } else if(okBox.touched(touchx, touchy)) {
        OK = true;
        break;
      } else if(cancelBox.touched(touchx, touchy)) {
        OK = false;
        break;
      }

      delay(200);
      clearBuffer();
    }

    yield();

  } while(true);

  if(OK)
    s->setCurrentSetTemp(setTemp);

  refreshScreen();
}

void SmartDisplay::settingsTouched()
{
  TextRect roomsButton(10, 10, 40, 300);
  TextRect backButton(200, 200, 40, 100);

  tft.fillScreen(backgroundColor);

  roomsButton.setBackgroundColor(backgroundColor);
  roomsButton.setFontSize((unsigned char)12);
  roomsButton.setTextColor(ILI9341_CYAN);
  roomsButton.setButton(&tft, "Rooms", ILI9341_CYAN, ILI9341_DARKCYAN, ILI9341_BLACK);

  backButton.setBackgroundColor(backgroundColor);
  backButton.setFontSize((unsigned char)18);
  backButton.setButton(&tft, "Back", ILI9341_CYAN, ILI9341_DARKCYAN, ILI9341_BLACK);

  clearBuffer();

  do {
    if (!spitouch.bufferEmpty()) {
      TS_Point p;
      uint16_t touchx, touchy;
      p = spitouch.getPoint();

      touchx = map(p.y, TS_MAXY, TS_MINY, 0, tft.width()); 
      touchy = map(p.x, TS_MAXX, TS_MINX, 0, tft.height());

      if(backButton.touched(touchx, touchy))
        break;
      else if (roomsButton.touched(touchx, touchy)) {
        displayRooms();
        roomsButton.refresh(&tft);
        backButton.refresh(&tft);
      }

      clearBuffer();
    }
    yield();
  } while(true);

  delay(250);
Serial.println("Clearing buffer");
  clearBuffer();
Serial.println("Broke");
  refreshScreen();
}

void SmartDisplay::displayRooms()
{
  int index = 0;
  bool lr = true;
  uint8_t row = 10;
  int rn = 0;
  TextRect *roomButton[MAX_ROOMS];
  TextRect addButton(30, 200, 40, 100);
  TextRect endButton(190, 200, 40, 100);

  tft.fillScreen(backgroundColor);

  addButton.setBackgroundColor(backgroundColor);
  addButton.setFontSize((unsigned char)18);
  addButton.setTextColor(ILI9341_CYAN);
  addButton.setButton(&tft, "New", ILI9341_CYAN, ILI9341_DARKCYAN, ILI9341_BLACK);

  endButton.setBackgroundColor(backgroundColor);
  endButton.setFontSize((unsigned char)18);
  endButton.setTextColor(ILI9341_CYAN);
  endButton.setButton(&tft, "Done", ILI9341_CYAN, ILI9341_DARKCYAN, ILI9341_BLACK);

  for(index = 0; index < MAX_ROOMS; index++) {
    if(!rooms[index].exists)
      break;

    rn++;
    roomButton[index] = new TextRect(lr ? 10 : 170, row, 30, 150);
    lr = !lr;
    if(lr)
      row += 40;

    roomButton[index]->setBackgroundColor(backgroundColor);
    roomButton[index]->setFontSize((unsigned char)12);
    roomButton[index]->setTextColor(ILI9341_CYAN);
    roomButton[index]->setButton(&tft, rooms[index].getRoomName(), ILI9341_CYAN, ILI9341_DARKCYAN, ILI9341_BLACK);
  }

  clearBuffer();

  do {
    if (!spitouch.bufferEmpty()) {
      TS_Point p;
      uint16_t touchx, touchy;
      p = spitouch.getPoint();

      clearBuffer();

      touchx = map(p.y, TS_MAXY, TS_MINY, 0, tft.width()); 
      touchy = map(p.x, TS_MAXX, TS_MINX, 0, tft.height());

      if(addButton.touched(touchx, touchy)) {
        String newRoom;
        tft.setFont(&FreeSans12pt7b);
        SoftKeyboard kb(&tft, &spitouch, 10, 30, 300, 130, TS_MAXX, TS_MINX, TS_MAXY, TS_MINY, newRoom);
        Serial.print("Adding ");
        Serial.print(newRoom);
        Serial.print(" at ");
        Serial.println(rn);
        rooms[rn].setRoomName((char *)newRoom.c_str());
        rooms[rn].save();
        roomButton[rn] = new TextRect(lr ? 10 : 170, row, 30, 150);
        lr = !lr;
        if(lr)
          row += 40;

        roomButton[rn]->setBackgroundColor(backgroundColor);
        roomButton[rn]->setFontSize((unsigned char)12);
        roomButton[rn]->setTextColor(ILI9341_CYAN);
        roomButton[rn]->setButton(&tft, rooms[rn].getRoomName(), ILI9341_CYAN, ILI9341_DARKCYAN, ILI9341_BLACK);
        rn++;
        for(int index = 0; index < rn; index++)
          roomButton[index]->refresh(&tft);
        addButton.refresh(&tft);
        endButton.refresh(&tft);
        
      } else if (endButton.touched(touchx, touchy))
        break;
      else {
        for(int index = 0; index < rn; index++) {
          if(roomButton[index]->touched(touchx, touchy)) {
            char str[64];
            sprintf(str, "Touched room %s", rooms[index].getRoomName());
            Serial.println(str);
            displayRoom(rooms[index]);
            rooms[index].reload();
            roomButton[index]->setButton(&tft, rooms[index].getRoomName(), ILI9341_CYAN, ILI9341_DARKCYAN, ILI9341_BLACK);
            roomButton[index]->refresh(&tft);
            addButton.refresh(&tft);
            endButton.refresh(&tft);
          }
        }
      }

      clearBuffer();
    }
    yield();
  } while(true);

  tft.fillScreen(backgroundColor);

  return;
}

void SmartDisplay::displayRoom(SmartRoom r)
{
  TextRect nameLabel(20, 10, 40, 80);
  TextRect nameText(160, 10, 40, 160);
  TextRect currentTempLabel(20, 50, 40, 80);
  TextRect currentTempText(160, 50, 40, 160);
  TextRect endButton(190, 200, 40, 100);
  TextRect addVentButton(20, 200, 40, 100);
  char currentTempStr[4];

  sprintf(currentTempStr, "%d", r.getCurrentTemperature());

  tft.fillScreen(backgroundColor);

  nameLabel.setBackgroundColor(backgroundColor);
  nameLabel.setFontSize((unsigned char)12);
  nameLabel.setTextColor(ILI9341_CYAN);
  nameLabel.setHorizontalAlignment(H_CENTER);
  nameLabel.setText(&tft, "Room:");

  nameText.setBackgroundColor(backgroundColor);
  nameText.setFontSize((unsigned char)12);
  nameText.setTextColor(ILI9341_CYAN);
  nameText.setHorizontalAlignment(H_CENTER);
  nameText.setText(&tft, r.getRoomName());

  currentTempLabel.setBackgroundColor(backgroundColor);
  currentTempLabel.setFontSize((unsigned char)12);
  currentTempLabel.setTextColor(ILI9341_CYAN);
  currentTempLabel.setHorizontalAlignment(H_CENTER);
  currentTempLabel.setText(&tft, "Temp:");

  currentTempText.setBackgroundColor(backgroundColor);
  currentTempText.setFontSize((unsigned char)12);
  currentTempText.setTextColor(ILI9341_CYAN);
  currentTempText.setHorizontalAlignment(H_CENTER);
  currentTempText.setText(&tft, currentTempStr);

  endButton.setBackgroundColor(backgroundColor);
  endButton.setFontSize((unsigned char)18);
  endButton.setTextColor(ILI9341_CYAN);
  endButton.setButton(&tft, "Done", ILI9341_CYAN, ILI9341_DARKCYAN, ILI9341_BLACK);

  addVentButton.setBackgroundColor(backgroundColor);
  addVentButton.setFontSize((unsigned char)12);
  addVentButton.setTextColor(ILI9341_CYAN);
  addVentButton.setButton(&tft, "Add Vent", ILI9341_CYAN, ILI9341_DARKCYAN, ILI9341_BLACK);

  clearBuffer();

  do {
    if (!spitouch.bufferEmpty()) {
      TS_Point p;
      uint16_t touchx, touchy;
      p = spitouch.getPoint();

      touchx = map(p.y, TS_MAXY, TS_MINY, 0, tft.width()); 
      touchy = map(p.x, TS_MAXX, TS_MINX, 0, tft.height());

      if(nameText.touched(touchx, touchy)) {
        String roomN(r.getRoomName());
        tft.setFont(&FreeSans12pt7b);
        SoftKeyboard kb(&tft, &spitouch, 10, 30, 300, 130, TS_MAXX, TS_MINX, TS_MAXY, TS_MINY, roomN);
        if(roomN.length() != 0) {
          r.setRoomName((char *)roomN.c_str());
          r.save();
          r.reload();
        }

        nameText.setText(&tft, r.getRoomName());
        nameText.refresh(&tft);
        nameLabel.refresh(&tft);
        endButton.refresh(&tft);
        addVentButton.refresh(&tft);
      } else if (endButton.touched(touchx, touchy)) {
        break;
      } else if (addVentButton.touched(touchx, touchy)) {
        addUnknownToRoom(r);
        r.reload();

        nameLabel.refresh(&tft);
        nameText.refresh(&tft);
        endButton.refresh(&tft);
      }
      delay(200);
      clearBuffer();
    }

    yield();
  } while(true);

  clearBuffer();
  tft.fillScreen(backgroundColor);
}

void SmartDisplay::clearBuffer()
{
  while(!spitouch.bufferEmpty()) {
    spitouch.getPoint();
  }
}

void SmartDisplay::refreshScreen()
{
  tft.fillScreen(backgroundColor);

  weatherIcon.refresh(&tft);
  
  outsideTempText.refresh(&tft);

  timeText.refresh(&tft);

  setTempText.refresh(&tft);

  setTempLabel.refresh(&tft);

  settingsIcon.refresh(&tft);
}

void SmartDisplay::addNotification()
{
  hasNotification = true;
  settingsIcon.setImage(&tft, "notify.bmp");
}

void SmartDisplay::addUnknownVent(char *uv)
{
  for(std::vector<String>::iterator it = unknownVents.begin() ; it != unknownVents.end(); ++it) {
    if((*it).compareTo(uv) == 0)
      return;
  }

  unknownVents.push_back(uv);
}

void SmartDisplay::addUnknownToRoom(SmartRoom r)
{
  int index = 0;
  bool lr = true;
  uint8_t row = 10;
  int rn = 0;
  std::vector<TextRect *> ventButtons;
  TextRect *thisVentButton;
  TextRect endButton(190, 200, 40, 100);

  tft.fillScreen(backgroundColor);

  endButton.setBackgroundColor(backgroundColor);
  endButton.setFontSize((unsigned char)18);
  endButton.setTextColor(ILI9341_CYAN);
  endButton.setButton(&tft, "Done", ILI9341_CYAN, ILI9341_DARKCYAN, ILI9341_BLACK);

  for(std::vector<String>::iterator it = unknownVents.begin(); it != unknownVents.end(); ++it) {
    thisVentButton = new TextRect(lr ? 10 : 170, row, 30, 150);
    thisVentButton->setButton(&tft, (char *)(*it).c_str(), ILI9341_CYAN, ILI9341_DARKCYAN, ILI9341_BLACK);
    ventButtons.push_back(thisVentButton);
    lr = !lr;
    if(lr)
      row += 40;
  }

  clearBuffer();

  do {
    if (!spitouch.bufferEmpty()) {
      TS_Point p;
      uint16_t touchx, touchy;
      p = spitouch.getPoint();

      touchx = map(p.y, TS_MAXY, TS_MINY, 0, tft.width()); 
      touchy = map(p.x, TS_MAXX, TS_MINX, 0, tft.height());

      for(std::vector<TextRect *>::iterator it = ventButtons.begin(); it != ventButtons.end(); ++it) {
        if((*it)->touched(touchx, touchy)) {
          r.addVent((*it)->getText());
          r.save();
        }
      }
      if(endButton.touched(touchx, touchy))
        break;

      delay(200);
      clearBuffer();
    }
    yield();
  } while(true);

  clearBuffer();
  tft.fillScreen(backgroundColor);
}
