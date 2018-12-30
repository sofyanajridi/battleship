// Created by Sofyan Ajridi, Structuur van Computerprogrammas 2




#include "MicroBit.h"

MicroBit uBit;


#define MAX_FIELD_WIDTH 4
#define MAX_FIELD_HEIGHT 4
#define kleine_schip_aantal 3
#define grote_schip_aantal 1
#define kleine_schip_lengte 2
#define grote_schip_lengte 3

int cursor_x = 0;
int cursor_y = 0;
int kleine_schip_x = uBit.random(5);
int kleine_schip_y = uBit.random(5);
int grote_schip_x = uBit.random(5);
int grote_schip_y = uBit.random(5);
int richting = uBit.random(2);


int should_continue = 1;
int multiplayer = 0;
int singleplayer= 0;
int multiplayer_start = 0;
int start = 1;

int player = 1;
int direction_start = 0;
int cursor_helderheid = 130;
int current_length = 3;
int boten_geplaatst = 8; // aantal boten die worden geplaatst bij het begin van de multiplayer
int pauze = 0;

int blinking = 0;
int blinking_helderheid = 0;


int zee_singleplayer[5][5] = {{0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
};


int zee_player1[5][5] = {{0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
};

int zee_player2[5][5] = {{0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
};


int (*zee)[5];

// Procedure die hele 2d array door gaat en gaat kijken of er een 1 erin zet en dus een pixel tekent op die positie
void draw_schip() {
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (zee[i][j] == 1){
                uBit.display.image.setPixelValue(j, i, 20);
            }
        }
    }
}

// Deze procedure gaat de cursor displayen op de microbit
void display_cursor() {
    uBit.display.clear();
    if (blinking == 1) { // hier zorgen we ervoor dat onze cursor kan blinken, als hij de vorige loop heeft geblinked zetten we de helderheid op 0 en anders terug op 150
        blinking = 0;
        blinking_helderheid = 150;
    } else if (blinking == 0){
        blinking = 1;
        blinking_helderheid = 0;
    }
    
    uBit.display.image.setPixelValue( cursor_x, cursor_y, blinking_helderheid);
}
// Deze procedure gaat de schip cursor die voorkomt bij het begin van de multiplayer displayen.
void display_cursor_schip() {
    int lengte = current_length;
    uBit.display.clear();
    if (blinking) { // Ook hier zorgen we ervoor dat die cursor kan blinken
        blinking = 0;
        blinking_helderheid = 180;
    }else {
        blinking = 1;
        blinking_helderheid = 0;}
    
    if (direction_start) { // afhankelijk van onze richting displayen we de cursor horizontaal of verticaal, we doen dit met een for loop en houden rekening met de huidige lengte van onze schip
        for (int i = 0; i < lengte; i++) {
            uBit.display.image.setPixelValue(cursor_x + i, cursor_y, blinking_helderheid);
        }
    }else {
        for (int i = 0; i < lengte; i++) {
            uBit.display.image.setPixelValue(cursor_x, cursor_y + i, blinking_helderheid);
        }
    }
}

void cursor_schip_beweeg_rechts(int lengte) { // hiermee bewegen we onze cursor schip afhankelijk van de huidige lengte, we houden ook rekening of de schip verticaal of horizontaal staat
    if (direction_start) {
        if (cursor_x + lengte -1 < MAX_FIELD_WIDTH) {
            cursor_x++;
        }else cursor_x = 0;
    }else {
        if (cursor_x < MAX_FIELD_WIDTH) {
            cursor_x++;
        }else cursor_x = 0; }
}

void cursor_schip_beweeg_onder(int lengte) { // zelfde procedure als boven alleen beweegt men hier naar beneden.
    if (direction_start) {
        if (cursor_y < MAX_FIELD_WIDTH) {
            cursor_y++;
        }else cursor_y = 0;
    }else {
        if (cursor_y + lengte - 1 < MAX_FIELD_WIDTH) {
            cursor_y++;
        }else cursor_y = 0; }
}

void cursor_beweeg_rechts() { // hier verhogen we of verlagen we de cursor_x waarden
    if (cursor_x < MAX_FIELD_WIDTH) {
        cursor_x++;
    } else cursor_x = 0;
}

void cursor_beweeg_onder() { // hier verhogen we of verlagen we de cursor_y waarden
    if (cursor_y < MAX_FIELD_HEIGHT) {
        cursor_y++;
    } else cursor_y = 0;
}


// Voordat we een schip plaatsen voor zowel multiplayers als singleplayer kijken we eerst of de positie waar we een schip willen plaatsen al een schip bevat
int check_overlapping_rechts(int x, int y, int offset ) {
    if (x + offset < 5)  {
        for (int i= 0; i < offset + 1 ; i++) {
            if (zee[y][x + i] == 1) { // hier kijken we in de 2d array of we een 1 tegenkomen, is dat het geval dan hebben we een overlapping en returnen we 0 om aan te duiden dat men hier geen schip mag plaatsen
                return 0;
            }
        }
        return 1;
    }else return 0;
}
// Zelfde procedure als boven maar kijkt naar overlapping onder
int check_overlapping_onder(int x, int y, int offset ) {
    if (y + offset < 5)  {
        for (int i= 0; i < offset + 1 ; i++) {
            if (zee[y + i][x] == 1) {
                return 0;}
        }
        return 1;
    }else return 0;
}

// Hier tekenen we effectief de schip
int teken_schip_horizontaal(int lengte,int x, int y) {
    if(check_overlapping_rechts(x, y, lengte-1)){ // als we op de hudige positie geen overlapping hebben tekenen we onze positie door in de 2d array afhankelijk van de lengte meerdere 1 plaatsen
        for (int i = 0; i < lengte; i++) {
            zee[y][x + i] = 1; }
        if (multiplayer_start) { // Als het plaatsen is gelukt in onze multiplayer mode returnen we een nul.
            return 1;}
        
    }else { // Als het plaatsen niet is gelukt returnen we in onze multiplayer een 0, en in de singleplayer roepen we de functie opnieuw op maar met andere random x en y waarden.
        if (multiplayer_start) {
            return 0;
        }else {
            x = uBit.random(5);
            y = uBit.random(5);
            return teken_schip_horizontaal(lengte,x ,y ); }
    }
}

//zelfde procedure als boven maar voor verticalen schepen
int teken_schip_verticaal(int lengte, int x, int y) {
    if(check_overlapping_onder(x, y, lengte-1) ){
        for (int i = 0; i < lengte; i++) {
            zee[y + i][x] = 1; }
        if (multiplayer_start) {
            return 1;}
    }else {
        if (multiplayer_start) {
            return 0;
            
        }else {
            x = uBit.random(5);
            y = uBit.random(5);
            return teken_schip_verticaal(lengte,x ,y ); }
    }
}


void random_schip_generator () { // deze procedure gaat in het begin van singleplayer random schepen plaatsen
    int kleine_schip_count = kleine_schip_aantal;
    int grote_schip_count = grote_schip_aantal;
    
    while (kleine_schip_count > 0) { // we kunnen zelf bepalen hoeveel kleine of grote schepen we willen
        if (uBit.random(2) == 0) { // de richting wordt steeds ook random uitgekozen
            teken_schip_verticaal(kleine_schip_lengte,kleine_schip_x,kleine_schip_y);
            kleine_schip_count--;
        }else {
            teken_schip_horizontaal(kleine_schip_lengte,kleine_schip_x,kleine_schip_y);
            kleine_schip_count--;
        }
    }
    while(grote_schip_count > 0) {
        if (uBit.random(2) == 0) {
            teken_schip_verticaal(grote_schip_lengte,grote_schip_x,grote_schip_y);
            grote_schip_count--;
        } else {
            teken_schip_horizontaal(grote_schip_lengte,grote_schip_x,grote_schip_y);
            grote_schip_count--;
        }
    }
    
}


int check_end_game() { // Hier kijken we of het spel klaar is
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (zee[i][j] == 1){ // als we heel onze 2d array langs gaan en een 1 tegenkomen zijn alle schepen nogsteeds niet gezonken
                return 1;}
        }
    }
    return 0; // als de for loop klaar is en we geen 1 meer tegenkomen is de huidge zee of 2d array leeg en zijn dus alle schepen gezonken
}


void drop_bom() {
    if (zee[cursor_y][cursor_x] == 1) { // als de huidge positie waar we een bom hebben gedropped overeenkomt met een 1 in onze matrix , betekend het dat we een schip hebben geraakt.
        zee[cursor_y][cursor_x] = 0;
        blinking = 2;
        blinking_helderheid = 255; // bij een hit gaan we onze cursor voor een korte tijd een grote helderheid geven om aan te tonen aan de speler dat we een hit hebben
        uBit.sleep(800);
        blinking = 0;

        
        if (check_end_game() == 1) { // na iedere bom drop kijken we of alle schepen gezonken zijn of niet
        }else {
            pauze = 1;
            multiplayer = 0; // beindigen onze spel
            uBit.display.scroll("W"); // displayen de winnaar
            uBit.display.scroll(player);
        }
        
        if (multiplayer) { // als we in de multiplayer modus zitten verandert de speler na iedere bom drop
            if (player == 2) {
                player = 1; // veranderen de speler
                zee = zee_player2; // veranderen onze huidige veld
            }else {
                player = 2;
                zee = zee_player1;
            }
            pauze = 1;
            uBit.display.scroll(player); // displayen wie nu aan de buurt is
            pauze = 0;
            cursor_x = 0;
            cursor_y = 0;
        }
    }else { // zelfde procedure als we een schip hitten alleen wordt hier de helderheid laag om aan te tonen dat we geen schip hebben geraakt
        blinking = 2;
        blinking_helderheid = 20;
        uBit.sleep(800);
        blinking = 0;
        
        
        if (multiplayer) {
            if (player == 2) {
                player = 1;
                zee = zee_player2;
            }else {
                player = 2;
                zee = zee_player1;
            }
            cursor_x = 0;
            cursor_y = 0;
            pauze = 1;
            uBit.display.scroll(player);
            pauze = 0;
        }
    }
}



void knop_a(MicroBitEvent e) {
    if (start) { // bij het begin van het spel wordt de knop a gebruikt om de modus te kiezen
        singleplayer = 1;
        zee = zee_singleplayer;
        random_schip_generator();
        start = 0;
    }else if (multiplayer_start){ // tijdens de start fase van de multiplayer wordt a gebruikt om de cursor schip te bewegen
        cursor_schip_beweeg_rechts(current_length);
    }else cursor_beweeg_rechts(); // tijdens het spel zelf kan je de schip naar rechts bewegen
}

// Zelfde alleen kan je in het begin multiplayer modus kiezen en kan je de schip of cursor naar beneden bewegen
void knop_b(MicroBitEvent e) {
    if (start) {
        multiplayer_start = 1;
        start = 0;
        zee = zee_player1;
    } else if (multiplayer_start){
        cursor_schip_beweeg_onder(current_length);
    }else cursor_beweeg_onder();
}

void knop_AB(MicroBitEvent e) {
    
    if (multiplayer_start) { // tijdens de start fase van de multiplayer kan je via AB de richting van de schip veranderen
        if (current_length == 3) {
            if (direction_start && cursor_y < current_length){ // we hebben hier 2 if testen om te voor zorgen dat de schip niet kan draaien als het in de hoek zit
                direction_start = 0;
                
            } else if (direction_start == 0 && cursor_x < current_length  ) {
                direction_start = 1;}
            
        } else {
            if (direction_start && cursor_y != MAX_FIELD_HEIGHT){
                direction_start = 0;
                
            } else if (direction_start == 0 && cursor_x != MAX_FIELD_WIDTH  ) {
                direction_start = 1;}
            
        }
    }else drop_bom(); // tijdens het spel zelf wordt AB gebruikt om een bom te droppen
    
}

// deze procedure gaat er voor zorgen dat in het begin van de multiplayer er om de beurt eerst 4 boten geplaatst moeten worden voordat de echte multiplayer kan beginnen
void schip_multiplayer_counter () {
    if (boten_geplaatst != 0) {
        if (boten_geplaatst == 4) { // nadat er 4 schepen zijn geplaatst is speler 2 aan de beurt
            player = 2;
            zee = zee_player2;
            pauze = 1;
            uBit.display.scroll(player);
            pauze = 0;
        }
        if (boten_geplaatst == 4 || boten_geplaatst == 8 ) { // de eerste schip is altijd een grote schip
            current_length = grote_schip_lengte;
            cursor_x = 0;
            cursor_y = 0;
            
        }else {
            current_length = kleine_schip_lengte;
            cursor_x = 0;
            cursor_y = 0;
        }
        
    }else { // wanneer iedere speler 4 boten heeft geplaatst begint de echte multiplayer
        multiplayer_start = 0;
        multiplayer = 1;
        player = 1;
        zee = zee_player2;
        pauze = 1;
        uBit.display.scroll(player);
        pauze = 0;
        cursor_x = 0;
        cursor_y = 0;
    }
    
}



void plaats_schip (MicroBitEvent e){ // tijdens de multplayer fase kan je door a lang ingedrukt te houden een schip effectief plaatsen
    if (multiplayer_start) {
        if(direction_start) {
            if (teken_schip_horizontaal(current_length,cursor_x,cursor_y) == 1) {
                boten_geplaatst--;
                schip_multiplayer_counter();
            }
        }else if (teken_schip_verticaal(current_length,cursor_x,cursor_y) == 1) {
        boten_geplaatst--;
        schip_multiplayer_counter();
        }
    }
}

void game_loop() {
    while (start) { // begin van het spel laten we zien dat men via a en b de game modus kan kiezen
        uBit.display.scroll("A->S");
        uBit.display.scroll("B->M");
    }
    
    if (multiplayer_start) {
        while (multiplayer_start) {// loop voor de start van de multiplayer om boten te kunnen plaatsen
            if (pauze == 0) {
                uBit.sleep(200);
                display_cursor_schip();
                draw_schip();
            }
            uBit.sleep(200);
        }
        while (multiplayer) { // loop tijdens de multiplayer
            if (pauze == 0) {
                uBit.sleep(200);
                display_cursor();
            }
            uBit.sleep(200);
        }
    }else if (singleplayer)  {
        while (should_continue) { // loop voor de singleplayer
            if (pauze == 0) {
            uBit.sleep(200);
            display_cursor();
            }
            uBit.sleep(200);
        }
    }
}


int main() {
    uBit.init();
    uBit.display.setDisplayMode(DISPLAY_MODE_GREYSCALE);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, knop_a);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, knop_b);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_AB, MICROBIT_BUTTON_EVT_CLICK, knop_AB);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_LONG_CLICK, plaats_schip);
    game_loop();
    release_fiber();
}





