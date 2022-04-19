void enableOneButton(int); // het meest linkse knopje op je shield = button 1
void enableMultipleButtons(uint8_t);
void enableAllButtons();
void disableOneButton(int);
void disableAllButtons ();
int buttonPushed(int); // het meest linkse knopje op je shield = button 1
int multipleButtonsPushed (uint8_t);
int allButtonsPushed ();
int buttonReleased(int); // het meest linkse knopje op je shield = button 1
int multipleButtonsReleased (uint8_t);
int allButtonsReleased (); 
void enableButtonInterrupt(int);
int buttonPushedAndDebounced(int);
int buttonReleasedAndDebounced(int);
void waitUntilButtonReleased(int);

