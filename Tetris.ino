#include <FastLED.h>

#define NUM_LEDS 210
#define NUM_LEDSI 54
#define DATA_PIN A2
#define COLOR_ORDER GRB
#define DATA_PIN_INFO A7

CRGB leds[NUM_LEDS];
CRGB ledsI[NUM_LEDSI];

const int rotLeftB = 8;
const int rotRightB = 6;
const int dropB = 7;
const int holdB = 5;
const int startB = 4;

const int joyUp = 3;
const int joyDown = 2;
const int joyLeft = A1;
const int joyRight = A0;

uint8_t iPieceBoard[20][4];
uint8_t iBoard[20][4];
uint8_t iABoard[20][4];

uint8_t gBoard[11][40];
uint8_t block[11][40];
uint8_t pile[11][40];
uint8_t tPoint[11][40];
bool del[22];
uint8_t newPlace[11][40];
uint8_t aBoard[11][60];
uint8_t ghBoard[11][40];
uint8_t pQueque[16];
uint8_t shuffle[] = { 1, 2, 3, 4, 5, 6, 7 };

long delay_ = 0;
long delayAnimation = 0;
long delayAnimationDeath = 0;
long delayAnimationLineClear = 0;
long delayNewBlock = 0;
long delayPause = 0;


int delayLevel = 1000;
int coolDown = 0;
int coolDownDeath = 0;
bool start = false;
int nextP = 7;

int pieceC = 0;
int currentRot = 0;
int held = 0;
bool active = false;

int xSpin = 0;
int ySpin = 0;
int nRotation;
bool cturn;
bool rotated = false;
int xOffset = 0;
int yOffset = 0;

int currentLine = 0;
int currentScore = 0;
int currentLevel = 1;

bool go = true;
bool animationClearLatch = false;
bool stickyLeft = true;
bool stickyRight = true;
bool stickyHold = true;
bool stickyDrop = true;
bool stickyHeld = true;
bool firstHeld = true;
bool stickyStart = true;

void setup()
{
    Serial.begin(9600);
    FastLED.addLeds<WS2812B, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.addLeds<WS2812B, DATA_PIN_INFO, COLOR_ORDER>(ledsI, NUM_LEDSI);
    FastLED.setBrightness(50);
    FastLED.clear();

    pinMode(rotLeftB, INPUT);
    pinMode(rotRightB, INPUT);
    pinMode(dropB, INPUT);
    pinMode(holdB, INPUT);
    pinMode(startB, INPUT);

    pinMode(joyUp, INPUT);
    pinMode(joyDown, INPUT);
    pinMode(joyLeft, INPUT);
    pinMode(joyRight, INPUT);

    randomSeed(analogRead(A5));


}
void shuffleDeck()
{
    for (int k = 6; k > 0; k--)
    {

        int randNumber = random(0, 7);
        int temp = shuffle[k];
        shuffle[k] = shuffle[randNumber];
        shuffle[randNumber] = temp;
    }
}
int nextPiece()
{
    nextP++;
    for (int x = 0; x <= 14; x++)
    {
        pQueque[x] = pQueque[x + 1];
        //Serial.print(pQueque[x]);

    }
    //Serial.println(" ");
    if (nextP == 8)
    {
        shuffleDeck();
        for (int x = 0; x <= 6; x++)
        {
            pQueque[x] = shuffle[x];
        }
        shuffleDeck();
        for (int x = 0; x <= 6; x++)
        {
            pQueque[x + 7] = shuffle[x];
        }
        nextP = 0;
    }
    if (pQueque[7] == 0)
    {
        shuffleDeck();
        for (int x = 0; x <= 6; x++)
        {
            pQueque[x + 7] = shuffle[x];
        }
        nextP = -1;
    }
    //int y = 2;
    //return y;
    return pQueque[0];
    /*nextP++;
    if (nextP > 6)
    {
        shuffleDeck();
        nextP = 0;
    }
    //Serial.println(nextP);
    return shuffle[nextP];*/

}
void hold()
{
    if (stickyHeld)
    {
        stickyHeld = false;
        int nextHeld = 0;
        nextHeld = pieceC;
        for (int y = 0; y <= 25; y++)
        {
            for (int x = 0; x <= 9; x++)
            {
                block[x][y] = 0;
                tPoint[x][y] = 0;
            }
        }
        piece(held);
        held = nextHeld;
    }
    if (firstHeld)
    {
        firstHeld = false;
        piece(nextPiece());
        stickyHeld = true;
    }
}
void newBlock()
{
    if (go && gameOverTest())
    {
        piece(nextPiece());
        go = false;
    }
    if (delayNewBlock < millis())
    {
        stickyHeld = true;
        for (int y = 0; y <= 21; y++)
        {
            for (int x = 0; x <= 9; x++)
            {
                if (block[x][y] > 0)
                {
                    pile[x][y] = block[x][y];
                    block[x][y] = 0;
                    tPoint[x][y] = 0;
                    go = true;
                }
                block[x][y] = 0;
            }
        }
        //Serial.println("New Block");
    }
}
void piece(int x)
{
    pieceC = x;
    //Serial.println(x);
    switch (x)
    {
        case 1://O
            block[4][21] = 1;
            block[5][21] = 1;
            block[4][20] = 1;
            block[5][20] = 1;
            currentRot = 0;
            break;
        case 2://I
            block[3][21] = 2;
            block[4][21] = 2;
            block[5][21] = 2;
            block[6][21] = 2;
            currentRot = 0;
            tPoint[5][21] = 1;
            break;
        case 3://S
            block[6][20] = 3;
            block[5][20] = 3;
            block[5][21] = 3;
            block[4][21] = 3;
            currentRot = 0;
            tPoint[5][20] = 1;
            break;
        case 4://Z
            block[6][21] = 4;
            block[5][21] = 4;
            block[5][20] = 4;
            block[4][20] = 4;
            currentRot = 0;
            tPoint[5][20] = 1;
            break;
        case 5://L
            block[6][21] = 5;
            block[5][21] = 5;
            block[4][21] = 5;
            block[6][20] = 5;
            currentRot = 0;
            tPoint[5][21] = 1;
            break;
        case 6://J
            block[6][21] = 6;
            block[5][21] = 6;
            block[4][21] = 6;
            block[4][20] = 6;
            currentRot = 0;
            tPoint[5][21] = 1;
            break;
        case 7://T
            block[6][21] = 7;
            block[5][21] = 7;
            block[4][21] = 7;
            block[5][20] = 7;
            currentRot = 0;
            tPoint[5][21] = 1;
            break;
    }
}

bool stick()
{
    if (active == false)
    {
        return false;
    }
    if (digitalRead(joyUp) == LOW)
    {

    }
    if (digitalRead(joyLeft) == LOW)
    {
        moveLeft();
        return true;
    }
    if (digitalRead(joyRight) == LOW)
    {
        moveRight();
        return true;
    }
    if (digitalRead(joyDown) == LOW)
    {
        moveDown(true);
        return true;
    }
    return false;
}
void buttons()
{

    if ((digitalRead(startB) == LOW) && (stickyStart == true))
    {
        //Serial.println("PAUSE");
        pause();
        //go = true;
        for (int y = 0; y <= 25; y++)
        {
            for (int x = 0; x <= 9; x++)
            {
                aBoard[x][y] = 0;
            }
        }
        //start = true;
        //Serial.println("HEYO");
        stickyStart = true;

    }
    if (digitalRead(startB) == HIGH)
    {
        //Serial.println("I count too");
        stickyStart == true;
    }

    if (active == false)
    {
        return;
    }
    if (digitalRead(rotLeftB) == LOW && stickyLeft == true)
    {
        rotate(true);
        stickyLeft = false;

    }
    if (digitalRead(rotLeftB) == HIGH)
    {
        stickyLeft = true;

    }
    if (digitalRead(rotRightB) == LOW && stickyRight == true)
    {
        rotate(false);
        stickyRight = false;
    }
    if (digitalRead(rotRightB) == HIGH)
    {
        stickyRight = true;
    }
    if (digitalRead(dropB) == LOW && stickyDrop == true)
    {
        hardDrop();
        stickyDrop = false;
    }
    if (digitalRead(dropB) == HIGH)
    {
        stickyDrop = true;
    }
    if (digitalRead(holdB) == LOW && stickyHold == true)
    {
        hold();
        stickyHold = false;
    }
    if (digitalRead(holdB) == HIGH)
    {
        stickyHold = true;
    }

}

bool moveDown(bool stickk)
{
    ghostP();
    if (canDown())
    {
        int z = 0;
        for (int y = 0; y <= 25; y++)
        {
            for (int x = 0; x <= 9; x++)
            {
                if (block[x][y] > 0)
                {


                    z = block[x][y];

                    block[x][y - 1] = z;
                    block[x][y] = 0;
                    if (tPoint[x][y] > 0)
                    {
                        tPoint[x][y - 1] = tPoint[x][y];
                        tPoint[x][y] = 0;
                        //Serial.println(y);
                    }
                }

            }
        }
        if (stickk)
        {
            score(1, 0);
        }
    }
}
void moveLeft()
{
    ghostP();
    if (canLeft())
    {
        int t = 0;
        for (int x = 9; x >= 0; x--)
        {
            for (int y = 0; y <= 25; y++)
            {
                if (block[x][y] > 0)
                {


                    t = block[x][y];

                    block[x + 1][y] = t;
                    block[x][y] = 0;
                    if (tPoint[x][y] > 0)
                    {
                        tPoint[x + 1][y] = tPoint[x][y];
                        tPoint[x][y] = 0;
                    }

                }
            }
        }
    }
}
void moveRight()
{
    ghostP();
    if (canRight())
    {
        int r = 0;
        for (int x = 0; x <= 9; x++)
        {
            for (int y = 0; y <= 25; y++)
            {
                if (block[x][y] > 0)
                {


                    r = block[x][y];

                    block[x - 1][y] = r;
                    block[x][y] = 0;
                    if (tPoint[x][y] > 0)
                    {
                        tPoint[x - 1][y] = tPoint[x][y];
                        tPoint[x][y] = 0;
                    }
                }
            }
        }
    }
}
void hardDrop()
{
    int distance = 100;
    for (int y = 0; y <= 25; y++)
    {
        for (int x = 0; x <= 9; x++)
        {
            if (block[x][y] > 0)
            {
                for (int z = 20; z >= 0; z--)
                {
                    if (pile[x][z] > 0)
                    {

                        if (distance > (y - z))
                        {
                            distance = y - z;

                        }
                    }
                }
            }
        }
    }
    for (int y = distance; y >= 0; y--)
    {
        moveDown(false);
    }
    delayNewBlock = delayNewBlock - 500;
}

bool canDown()
{

    if (!(active))
    {
        return false;
    }
    for (int y = 25; y >= 0; y--)
    {
        for (int x = 0; x <= 9; x++)
        {

            if ((block[x][y] > 0) && (y == 0))
            {

                return false;

            }
            if ((block[x][y] > 0) && (pile[x][y - 1] > 0))
            {//This might be a problem for you later so watch out

                return false;

            }

        }

    }
    return true;

}
bool canLeft()
{
    for (int x = 9; x >= 0; x--)
    {
        for (int y = 0; y <= 25; y++)
        {
            if ((block[x][y] > 0) && (x == 9))
            {
                return false;
            }
            if ((block[x][y] > 0) && (pile[x + 1][y] > 0))
            {
                return false;
            }
        }
    }
    return true;
}
bool canRight()
{
    for (int x = 9; x >= 0; x--)
    {
        for (int y = 0; y <= 25; y++)
        {
            if ((block[x][y] > 0) && (x == 0))
            {
                return false;
            }
            if ((block[x][y] > 0) && (pile[x - 1][y] > 0))
            {
                return false;
            }
        }
    }
    return true;
}

void pause()
{

    if ((gameOverTest()) && (delayPause < millis()))
    {
        active = !(active);
        start = !(start);

    }
    if (active)
    {
        for (int y = 0; y <= 3; y++)
        {
            for (int x = 0; x <= 17; x++)
            {
                iABoard[x][y] = 0;
            }
        }
    }
    else if (!(active))
    {
        animationPaused();
        // iABoard[0][0] = 18;//D
        iABoard[0][1] = 8;
        // iABoard[0][2] = 18;
        iABoard[1][0] = 8;
        //iABoard[1][1] = 18;
        iABoard[1][2] = 8;
        iABoard[2][0] = 8;
        iABoard[2][1] = 8;
        iABoard[2][2] = 8;

        iABoard[3][0] = 9;//E
        //iABoard[3][1] = 18;
        iABoard[3][2] = 9;
        iABoard[4][0] = 9;
        iABoard[4][1] = 9;
        iABoard[4][2] = 9;
        iABoard[5][0] = 9;
        iABoard[5][1] = 9;
        iABoard[5][2] = 9;

        //iABoard[6][0] = 18;//S
        //iABoard[6][1] = 18;
        iABoard[6][2] = 8;
        iABoard[7][0] = 8;
        iABoard[7][1] = 8;
        iABoard[7][2] = 8;
        iABoard[8][0] = 8;
        //iABoard[8][1] = 18;
        //iABoard[8][2] = 18;

        iABoard[9][0] = 9;//U
        iABoard[9][1] = 9;
        iABoard[9][2] = 9;
        iABoard[10][0] = 9;
        //iABoard[10][1] = 18;
        //iABoard[10][2] = 18;
        iABoard[11][0] = 9;
        iABoard[11][1] = 9;
        iABoard[11][2] = 9;

        iABoard[12][0] = 8;//A
        iABoard[12][1] = 8;
        //iABoard[12][2] = 18;
        //iABoard[13][0] = 18;
        iABoard[13][1] = 8;
        iABoard[13][2] = 8;
        iABoard[14][0] = 8;
        iABoard[14][1] = 8;
        // iABoard[14][2] = 18;

        //iABoard[15][0] = 18;//P
        iABoard[15][1] = 9;
        iABoard[15][2] = 9;
        // iABoard[16][0] = 18;
        iABoard[16][1] = 9;
        iABoard[16][2] = 9;
        iABoard[17][0] = 9;
        iABoard[17][1] = 9;
        iABoard[17][2] = 9;

    }
    delayPause = millis() + 2000;
    // Serial.println("PAUSE");
}
void ghostP()
{
    int dist = 30;
    for (int y = 0; y <= 25; y++)
    {
        for (int x = 0; x <= 9; x++)
        {
            ghBoard[x][y] = 0;
        }
    }

    for (int y = 0; y <= 25; y++)
    {
        for (int x = 0; x <= 9; x++)
        {
            if (block[x][y] > 0)
            {
                for (int z = 0; z < 25; z++)
                {

                    if (pile[x][z] > 0)
                    {
                        if (((y - z) < dist) && (y > z))
                        {
                            dist = y - z;
                        }
                    }
                    if (dist == 30)
                    {
                        dist = y + 1;
                    }
                }
            }
        }
    }
    for (int y = 0; y <= 25; y++)
    {
        for (int x = 0; x <= 9; x++)
        {
            if (block[x][y] > 0)
            {
                ghBoard[x][y - dist + 1] = 9;
            }

        }
    }
    for (int y = 0; y <= 25; y++)
    {
        for (int x = 0; x <= 9; x++)
        {
            if ((block[x][y] > 0) && (ghBoard[x][y] > 0))
            {
                ghBoard[x][y] = 0;
            }
        }
    }
    //Serial.println(dist);
}
uint16_t XY(uint8_t x, uint8_t y)
{
    uint16_t i;
    i = (y * 10) + x;
    return i;
}
uint16_t XYi(uint8_t x, uint8_t y)
{
    uint16_t i;
    i = (y * 18) + x;
    return i;
}
void GameB()
{

    for (int y = 0; y <= 25; y++)
    {
        for (int x = 0; x <= 9; x++)
        {
            gBoard[x][y] = (block[x][y]) | (pile[x][y] | (aBoard[x][y]) | (ghBoard[x][y]));
        }
    }

    for (int y = 0; y <= 20; y++)
    {
        for (int x = 0; x <= 9; x++)
        {

            if (gBoard[x][y] == 0)
            {
                leds[XY(x, y)] = 0x000000;//Gray
            }
            else if (gBoard[x][y] == 1)
            {//Yellow
                leds[XY(x, y)] = 0xFFFF00;
            }
            else if (gBoard[x][y] == 2)
            {//cyan
                leds[XY(x, y)] = 0x00FFFF;
            }
            else if (gBoard[x][y] == 3)
            {//green
                leds[XY(x, y)] = 0x008000;
            }
            else if (gBoard[x][y] == 4)
            {//red
                leds[XY(x, y)] = 0xFF0000;
            }
            else if (gBoard[x][y] == 5)
            {//orange
                leds[XY(x, y)] = 0x8e5c00;
            }
            else if (gBoard[x][y] == 6)
            {//blue
                leds[XY(x, y)] = 0x0000FF;
            }
            else if (gBoard[x][y] == 7)
            {//purple
                leds[XY(x, y)] = 0x800080;
            }
            else if (gBoard[x][y] == 8)
            {//dead color
                leds[XY(x, y)] = 0x660000;
            }
            else if (gBoard[x][y] == 9)
            {//line clear
                leds[XY(x, y)] = 0xDDDDDD;
            }
            else if (gBoard[x][y] > 30)
            {//line clear
                leds[XY(x, y)] = 0x000000;
            }
            else
            {
                leds[XY(x, y)] = 0xFFFFFF;
                //Serial.println("weewoo");
                //Serial.println(XY(x, y));
            }


        }

    }
}

void holdI()
{
    for (int y = 0; y <= 3; y++)
    {
        for (int x = 14; x <= 17; x++)
        {
            iPieceBoard[x][y] = 0;
        }
    }
    if (held == 1)//o
    {
        iPieceBoard[14][0] = 1;
        iPieceBoard[15][0] = 1;
        iPieceBoard[14][1] = 1;
        iPieceBoard[15][1] = 1;
    }
    else if (held == 2)//I
    {
        iPieceBoard[15][0] = 2;
        iPieceBoard[15][1] = 2;
        iPieceBoard[15][2] = 2;
        //iBoard[15][3] = 2;
    }
    else if (held == 3)//s
    {
        iPieceBoard[14][1] = 3;
        iPieceBoard[15][1] = 3;
        iPieceBoard[15][0] = 3;
        iPieceBoard[16][0] = 3;
    }
    else if (held == 4)//z
    {
        iPieceBoard[16][1] = 4;
        iPieceBoard[15][1] = 4;
        iPieceBoard[15][0] = 4;
        iPieceBoard[14][0] = 4;
    }
    else if (held == 5)//L
    {
        iPieceBoard[15][2] = 5;
        iPieceBoard[15][1] = 5;
        iPieceBoard[15][0] = 5;
        iPieceBoard[14][0] = 5;
    }
    else if (held == 6)//J
    {
        iPieceBoard[14][2] = 6;
        iPieceBoard[14][1] = 6;
        iPieceBoard[14][0] = 6;
        iPieceBoard[15][0] = 6;
    }
    else if (held == 7)//T
    {
        iPieceBoard[15][0] = 7;
        iPieceBoard[14][1] = 7;
        iPieceBoard[15][1] = 7;
        iPieceBoard[16][1] = 7;
    }
}
void nextPI(int r)
{
    //Serial.println("preview");
    //int preview = shuffle[nextP + r + 1];
    int preview = pQueque[r + 1];
    r = 9 - 4 * r;
    for (int y = 0; y <= 3; y++)
    {
        for (int x = r; x <= (3 + r); x++)
        {
            iPieceBoard[x][y] = 0;
        }
    }
    if (preview == 1)//o
    {
        iPieceBoard[r][0] = 1;
        iPieceBoard[r + 1][0] = 1;
        iPieceBoard[r][1] = 1;
        iPieceBoard[r + 1][1] = 1;
    }
    else if (preview == 2)//I
    {
        iPieceBoard[r + 1][0] = 2;
        iPieceBoard[r + 1][1] = 2;
        iPieceBoard[r + 1][2] = 2;
        //iBoard[15][3] = 2;
    }
    else if (preview == 3)//s
    {
        iPieceBoard[r][1] = 3;
        iPieceBoard[r + 1][1] = 3;
        iPieceBoard[r + 1][0] = 3;
        iPieceBoard[r + 2][0] = 3;
    }
    else if (preview == 4)//z
    {
        iPieceBoard[r + 2][1] = 4;
        iPieceBoard[r + 1][1] = 4;
        iPieceBoard[r + 1][0] = 4;
        iPieceBoard[r][0] = 4;
    }
    else if (preview == 5)//L
    {
        iPieceBoard[r + 1][2] = 5;
        iPieceBoard[r + 1][1] = 5;
        iPieceBoard[r + 1][0] = 5;
        iPieceBoard[r][0] = 5;
    }
    else if (preview == 6)//J
    {
        iPieceBoard[r][2] = 6;
        iPieceBoard[r][1] = 6;
        iPieceBoard[r][0] = 6;
        iPieceBoard[r + 1][0] = 6;
    }
    else if (preview == 7)//T
    {
        iPieceBoard[r + 1][0] = 7;
        iPieceBoard[r][1] = 7;
        iPieceBoard[r + 1][1] = 7;
        iPieceBoard[r + 2][1] = 7;
    }

}

void InfoB()
{
    holdI();
    nextPI(0);
    nextPI(1);
    nextPI(2);

    for (int y = 0; y <= 3; y++)
    {
        for (int x = 0; x <= 17; x++)
        {
            //iBoard[x][y] = 0;
            if (active)
            {
                iBoard[x][y] = iPieceBoard[x][y];
            }
            else if (!(active))
            {
                iBoard[x][y] = iABoard[x][y];
            }

        }
    }


    for (int y = 0; y <= 3; y++)
    {
        for (int x = 0; x <= 17; x++)
        {

            if (iBoard[x][y] == 0)
            {
                ledsI[XYi(x, y)] = 0x000000;//Gray
            }
            else if (iBoard[x][y] == 1)
            {//Yellow
                ledsI[XYi(x, y)] = 0xFFFF00;
            }
            else if (iBoard[x][y] == 2)
            {//cyan
                ledsI[XYi(x, y)] = 0x00FFFF;
            }
            else if (iBoard[x][y] == 3)
            {//green
                ledsI[XYi(x, y)] = 0x008000;
            }
            else if (iBoard[x][y] == 4)
            {//red
                ledsI[XYi(x, y)] = 0xFF0000;
            }
            else if (iBoard[x][y] == 5)
            {//orange
                ledsI[XYi(x, y)] = 0x8e5c00;
            }
            else if (iBoard[x][y] == 6)
            {//blue
                ledsI[XYi(x, y)] = 0x0000FF;
            }
            else if (iBoard[x][y] == 7)
            {//purple
                ledsI[XYi(x, y)] = 0x800080;
            }
            else if (iBoard[x][y] == 8)
            {//dead color
                ledsI[XYi(x, y)] = 0x555555;
            }
            else if (iBoard[x][y] == 9)
            {//white
                ledsI[XYi(x, y)] = 0xDDDDDD;
            }
            else if (iBoard[x][y] == 10)
            {
                ledsI[XY(x, y)] = 0x555555;
                //Serial.println("weewoo");
                //Serial.println(XY(x, y));
            }
            else if (iBoard[x][y] >= 27 && iBoard[x][y] <= 35)
            {
                ledsI[XY(x, y)] = 0x555555;
            }


        }

    }
}

void clearTest()
{

    int t = 0;
    bool g = false;
    if (gameOverTest())
    {
        for (int y = 0; y <= 20; y++)
        {
            for (int x = 0; x <= 9; x++)
            {
                if (pile[x][y] > 0)
                {
                    t++;
                }
                if (t == 10)
                {
                    del[y] = true;
                    g = true;
                    animationLineClear(y);

                }

            }
            t = 0;
        }
        if (g)
        {
            clearLine();
            g = false;
        }
    }
}
void clearLine()
{
    int t = 0;
    bool cleared[22];
    for (int y = 0; y <= 20; y++)
    {
        if (del[y])
        {
            t++;
        }
    }
    score(0, t);
    for (int y = 0; y <= 20; y++)
    {
        cleared[y] = del[y];
        if (del[y])
        {
            for (int x = 0; x <= 9; x++)
            {

                pile[x][y] = 0;

            }
        }
        del[y] = false;
    }
    for (int y = 20; y >= 0; y--)
    {
        if (cleared[y])
        {

            for (int z = y; z <= 20; z++)
            {
                //Serial.println(z);
                for (int x = 0; x <= 9; x++)
                {
                    pile[x][z] = pile[x][z + 1];
                }
            }
            cleared[y] = false;
        }

    }
}
void score(int down, int lines)
{
    //int x = 10;
    currentLine = currentLine + lines;
    //Serial.println(currentLine);
    //x = currentLine%10;
    if ((currentLine % 10) == 0)
    {
        currentLevel = currentLine / 10 + 1;
        delayLevel = pow((0.8 - ((currentLevel - 1) * 0.007)), currentLevel) * 1000;
        //Serial.println(currentLine);
    }
    currentScore = currentScore + down;
    if (lines = 1)
    {
        currentScore = currentScore + 100 * currentLevel;
    }
    else if (lines = 2)
    {
        currentScore = currentScore + 300 * currentLevel;
    }
    else if (lines = 3)
    {
        currentScore = currentScore + 500 * currentLevel;
    }
    else if (lines = 4)
    {
        currentScore = currentScore + 800 * currentLevel;
    }
}

void GameOver()
{//TODO
    if (digitalRead(startB) == LOW)
    {//RESET RESET RESET RESET RESET
        __asm__ __volatile__("jmp 0x0000");
    }
    //Serial.println("You're dead Fred");
    animationDeath();


}
bool gameOverTest()
{
    if ((pile[4][20] > 0) || (pile[5][20] > 0))
    {
        GameOver();
        return false;
    }
    else
    {
        return true;
    }
}

void timing()
{
    if (delay_ < millis())
    {

        delay_ = millis() + delayLevel;
        moveDown(false);
    }
    if (delayAnimation < millis())
    {
        delayAnimation = millis() + 250;
        coolDown++;
        if (coolDown == 60)
        {
            coolDown = 0;
        }

    }
    if ((delayAnimationDeath < millis()) && !(gameOverTest()))
    {
        delayAnimationDeath = millis() + 125;
        coolDownDeath++;
        if (coolDownDeath == 30)
        {
            coolDownDeath--;
        }
    }
    if (canDown() || stick())
    {
        delayNewBlock = millis() + 500;
    }
    if ((delayAnimationLineClear < millis()) && (animationClearLatch))
    {
        clearLine();
        animationClearLatch = false;
        for (int y = 0; y <= 25; y++)
        {
            for (int x = 0; x <= 9; x++)
            {
                aBoard[x][y] = 0;
            }
        }

    }
}

void loop()
{
    gameOverTest();
    buttons();
    timing();
    if (start)
    {
        newBlock();
        ghostP();
        stick();
        score(0, 0);
        InfoB();
        GameB();
        clearTest();

    }
    else
    {
        if (currentScore > 0)
        {
            animationPaused();
        }
        else if (currentScore == 0)
        {
            animationScroll();
        }
        InfoB();
        GameB();
    }
    FastLED.show();
    FastLED.clear();
    delay(30);
}

void rotate(bool turn)
{
    xSpin = 0;
    ySpin = 0;
    xOffset = 0;
    yOffset = 0;
    nRotation = currentRot;
    cturn = turn;
    rotated = false;
    if (pieceC == 2)//I fucked up programming long so I am just doing this than changing everything
    {
        turn = !(turn);
        cturn = !(cturn);
    }
    if (turn == true)//left
    {
        nRotation++;
    }
    else//right
    {
        nRotation--;
    }
    if (nRotation > 3)
    {
        nRotation = 0;
    }
    if (nRotation < 0)
    {
        nRotation = 3;
    }
    
    if (pieceC == 1)
    {//0 piece
        return;
    }
    //Serial.println(currentRot);
    //Serial.println(nRotation);

    for (int y = 0; y <= 25; y++)//Get the rotation points location
    {
        for (int x = 0; x <= 9; x++)
        {
            if (tPoint[x][y] > 0)
            {
                xSpin = x;
                ySpin = y;

            }
            newPlace[x][y] = 0;
        }
    }
    if (pieceC == 2)//long piece
    {
        /*if ((nRotation == 0) && (!(rotated)))
        {
            xOffset = 1;
            yOffset = 0;
           // Serial.println("rot 0");
            rotOffSet(xOffset, yOffset);
        }
        if ((nRotation == 1) && (!(rotated)))
        {
            xOffset = 0;
            yOffset = -1;
           // Serial.println("rot 1");
            rotOffSet(xOffset, yOffset);
        }
        if ((nRotation == 2) && (!(rotated)))
        {
            xOffset = -1;
            yOffset = 0;
           // Serial.println("rot 2");
            rotOffSet(xOffset, yOffset);
        }
        if ((nRotation == 3) && (!(rotated)))
        {
            xOffset = 0;
            yOffset = 1;
           // Serial.println("rot 3");
            rotOffSet(xOffset, yOffset);
        }*/
        
        if((currentRot == 0) && (nRotation == 1) && !(rotated))//L
        {
            xOffset = 0;
            yOffset = -1;
            //Serial.println("to the left");
            rotOffSet(xOffset, yOffset);
            lineRot32or01();
        }
        if ((currentRot == 0) && (nRotation == 3) && !(rotated))//R
        {
            xOffset = -1;
            yOffset = 0;
            //Serial.println("to the right");
            rotOffSet(xOffset, yOffset);
            lineRot03or12();
        }
        if ((currentRot == 1) && (nRotation == 2) && !(rotated))//L
        {
            xOffset = -1;
            yOffset = 0;
            rotOffSet(xOffset, yOffset);
            lineRot03or12();
        }
        if ((currentRot == 1) && (nRotation == 0) && !(rotated))//R
        {
            xOffset =0;
            yOffset = 1;
            rotOffSet(xOffset, yOffset);
            lineRot23or10();
            //lineRot0();
        }
        if ((currentRot == 2) && (nRotation == 3) && !(rotated))//L
        {
            xOffset = 0;
            yOffset = 1;
            rotOffSet(xOffset, yOffset);
            lineRot23or10();
        }
        if ((currentRot == 2) && (nRotation == 1) && !(rotated))//R
        {
            xOffset =1;
            yOffset = 0;
            rotOffSet(xOffset, yOffset);
            lineRot30or21();
        }
        if ((currentRot == 3) && (nRotation == 0) && !(rotated))//L
        {
            xOffset = 1;
            yOffset = 0;
            rotOffSet(xOffset, yOffset);
             lineRot30or21();
            //lineRot0();
        }
        if ((currentRot == 3) && (nRotation == 2) && !(rotated))//R
        {
            xOffset =0;
            yOffset = -1;
            rotOffSet(xOffset, yOffset);
            lineRot32or01();
        }

        if (!(rotated))//ground kick
        {
            xOffset = 0;
            yOffset = 2;
            rotOffSet(xOffset, yOffset);
        }

        return;
    }
    rotOffSet(0, 0);
   // Serial.println(nRotation);

    if ((nRotation == 1) && (!(rotated)))
    {
        xOffset = -1;
        yOffset = 0;
        rotOffSet(xOffset, yOffset);
        //Serial.println(" 1 Test 1");
    }
     if ((nRotation == 1) && (!(rotated)))
    {
        xOffset = -1;
        yOffset = -1;
        rotOffSet(xOffset, yOffset);
       // Serial.println(" 1 Test 2");
    }
     if ((nRotation == 1) && (!(rotated)))
    {
        xOffset = 0;
        yOffset = 2;
        rotOffSet(xOffset, yOffset);
       // Serial.println(" 1 Test 3");
    }
     if ((nRotation == 1) && (!(rotated)))
    {
        xOffset = -1;
        yOffset = 2;
        rotOffSet(xOffset, yOffset);
       // Serial.println(" 1 Test 4");
    }

    if ((nRotation == 3) && (!(rotated)))
    {
        xOffset = 1;
        yOffset = 0;
        rotOffSet(xOffset, yOffset);
        //Serial.println(" 2 Test 1");
    }
     if ((nRotation == 3) && (!(rotated)))
    {
        xOffset = 1;
        yOffset = -1;
        rotOffSet(xOffset, yOffset);
       // Serial.println(" 2 Test 2");
    }
     if ((nRotation == 3) && (!(rotated)))
    {
        xOffset = 0;
        yOffset = 2;
        rotOffSet(xOffset, yOffset);
        //Serial.println(" 2 Test 3");
    }
     if ((nRotation == 3) && (!(rotated)))
    {
        xOffset = 1;
        yOffset = 2;
        rotOffSet(xOffset, yOffset);
        //Serial.println(" 2 Test 4");
    }

    if ((nRotation == 2) && (!(rotated)))
    {
        xOffset = -1;
        yOffset = 0;
        rotOffSet(xOffset, yOffset);
       // Serial.println(" 1 Test 4");
    }
    if ((nRotation == 2) && (!(rotated)))
    {
        xOffset = 1;
        yOffset = 0;
        rotOffSet(xOffset, yOffset);
        //Serial.println(" 1 Test 4");
    }

    if ((nRotation == 0) && (!(rotated)))
    {
        xOffset = -1;
        yOffset = 0;
        rotOffSet(xOffset, yOffset);
        //Serial.println(" 1 Test 4");
    }
    if ((nRotation == 0) && (!(rotated)))
    {
        xOffset = 1;
        yOffset = 0;
        rotOffSet(xOffset, yOffset);
        //Serial.println(" 1 Test 4");
    }
    if (!(rotated))//ground kick
    {
        xOffset = 0;
        yOffset = 1;
        rotOffSet(xOffset, yOffset);
    }

}
void lineRot03or12 ()
{
    if(!(rotated))
    {
        Serial.println("we in bois");
        if (!(rotated))
        {
            xOffset = -2;
            yOffset = 0;
            rotOffSet(xOffset, yOffset);
           // Serial.println("Test 1");
        }
         if (!(rotated))
        {
            xOffset = 1;
            yOffset = 0;
            rotOffSet(xOffset, yOffset);
           // Serial.println("Test 2");
        }
         if (!(rotated))
        {
            xOffset = -2;
            yOffset = -1;
            rotOffSet(xOffset, yOffset);
           // Serial.println("Test 3");
        }
         if (!(rotated))
        {
            xOffset = 1;
            yOffset = 2;
            rotOffSet(xOffset, yOffset);
            //Serial.println("Test 4");
        }
    }
}
void lineRot30or21 ()
{
    if (!(rotated))
    {
        Serial.println("Made it in");
        if (!(rotated))
        {
            xOffset = 2;
            yOffset = 0;
            rotOffSet(xOffset, yOffset);
           // Serial.println("Test 1");
        }
         if (!(rotated))
        {
            xOffset = -1;
            yOffset = 0;
            rotOffSet(xOffset, yOffset);
           // Serial.println("Test 2");
        }
         if (!(rotated))
        {
            xOffset = 2;
            yOffset = 1;
            rotOffSet(xOffset, yOffset);
            //Serial.println("Test 3");
        }
         if (!(rotated))
        {
            xOffset = -1;
            yOffset = -2;
            rotOffSet(xOffset, yOffset);
           // Serial.println("Test 4");
        }
    }
}
void lineRot32or01 ()
{
    if (!(rotated))
    {
        //Serial.println("Made it in");
        if (!(rotated))
        {
            xOffset = 1;
            yOffset = 0;
            rotOffSet(xOffset, yOffset);
            //Serial.println("Test 1");
        }
         if (!(rotated))
        {
            xOffset = -2;
            yOffset = 0;
            rotOffSet(xOffset, yOffset);
            //Serial.println("Test 2");
        }
         if (!(rotated))
        {
            xOffset = 1;
            yOffset = 2;
            rotOffSet(xOffset, yOffset);
            //Serial.println("Test 3");
        }
         if (!(rotated))
        {
            xOffset = -2;
            yOffset = -1;
            rotOffSet(xOffset, yOffset);
            //Serial.println("Test 4");
        }
    }
}
void lineRot23or10 ()
{
    if (!(rotated))
    {
        if (!(rotated))
        {
            xOffset = -1;
            yOffset = 0;
            rotOffSet(xOffset, yOffset);
        }
         if (!(rotated))
        {
            xOffset = 2;
            yOffset = 0;
            rotOffSet(xOffset, yOffset);
        }
         if (!(rotated))
        {
            xOffset = -1;
            yOffset = -2;
            rotOffSet(xOffset, yOffset);
        }
         if (!(rotated))
        {
            xOffset = -2;
            yOffset = 1;
            rotOffSet(xOffset, yOffset);
        }
    }
}


void rotOffSet(int H, int V)
{
    int xRelative = 0;
    int yRelative = 0;
    int xT = 0;
    int yT = 0;
    for (int y = 0; y <= (ySpin + 4); y++)//clear
    {
        for (int x = 0; x <= 9; x++)
        {
            newPlace[x][y] = 0;
        }
    }
    for (int y = 0; y <= (ySpin + 4); y++)//spin it
    {
        for (int x = 0; x <= 9; x++)
        {
            if (block[x][y] > 0)
            {
                xRelative = x - xSpin;
                yRelative = y - ySpin;
                if (cturn)
                {
                    xT = 0 * xRelative + -1 * yRelative;
                    yT = 1 * xRelative + 0 * yRelative;
                    if (((xT + xSpin + H) >= 0) && ((yT + ySpin + V) >= 0))
                    {
                        newPlace[xT + xSpin + H][yT + ySpin + V] = block[x][y];
                    }
                    else
                    {
                        return;
                    }
                }
                else
                {
                    xT = 0 * xRelative + 1 * yRelative;
                    yT = -1 * xRelative + 0 * yRelative;
                    if (((xT + xSpin + H) >= 0) && ((yT + ySpin + V) >= 0))
                    {
                        newPlace[xT + xSpin + H][yT + ySpin + V] = block[x][y];
                    }
                    else
                    {
                        return;
                    }
                }
            }
        }
    }
    placeRotation();
}

bool testCollision()
{
    int pCount = 0;
    for (int y = 0; y <= 25; y++)//tests to see if there is a collision
    {
        for (int x = 0; x <= 9; x++)
        {
            if (newPlace[x][y] > 0)
            {
                pCount++;
            }
            if (newPlace[x][y] > 0 && pile[x][y] > 0)
            {
                rotated = false;
                return false;

            }
        }
    }
    if (pCount == 4)
    {
        return true;
    }
    else
    {
        rotated = false;
    }

}
void placeRotation()
{
    if (testCollision())//does the rotation
    {
        rotated = true;
        for (int y = 0; y <= 25; y++)
        {
            for (int x = 0; x <= 9; x++)
            {
                block[x][y] = 0;
                tPoint[x][y] = 0;
            }
        }
        currentRot = nRotation;
        tPoint[xSpin + xOffset][ySpin + yOffset] = 1;
        for (int y = 0; y <= 25; y++)
        {
            for (int x = 0; x <= 9; x++)
            {
                block[x][y] = newPlace[x][y];
                newPlace[x][y] = 0;
            }
        }
    }
}

void animationPaused()
{
    for (int y = 0; y <= 20; y++)
    {
        for (int x = 0; x <= 9; x++)
        {
            aBoard[x][y] = 31;
        }
    }

}
void animationScroll()
{



    int t = 0;
    aBoard[9][coolDown] = 4;
    aBoard[8][coolDown] = 4;
    aBoard[7][coolDown] = 4;
    aBoard[6][coolDown] = 4;
    aBoard[5][coolDown] = 4;
    aBoard[4][coolDown] = 4;
    t++;
    aBoard[9][coolDown - t] = 4;
    aBoard[8][coolDown - t] = 0;
    aBoard[7][coolDown - t] = 4;
    aBoard[6][coolDown - t] = 4;
    aBoard[5][coolDown - t] = 0;
    aBoard[4][coolDown - t] = 4;
    t++;
    aBoard[9][coolDown - t] = 0;
    aBoard[8][coolDown - t] = 0;
    aBoard[7][coolDown - t] = 4;
    aBoard[6][coolDown - t] = 4;
    aBoard[5][coolDown - t] = 0;
    aBoard[4][coolDown - t] = 0;
    t++;
    aBoard[9][coolDown - t] = 0;
    aBoard[8][coolDown - t] = 0;
    aBoard[7][coolDown - t] = 4;
    aBoard[6][coolDown - t] = 4;
    aBoard[5][coolDown - t] = 0;
    aBoard[4][coolDown - t] = 0;
    t++;
    aBoard[9][coolDown - t] = 0;
    aBoard[8][coolDown - t] = 4;
    aBoard[7][coolDown - t] = 4;
    aBoard[6][coolDown - t] = 4;
    aBoard[5][coolDown - t] = 4;
    aBoard[4][coolDown - t] = 0;
    t++;
    aBoard[9][coolDown - t] = 0;
    aBoard[8][coolDown - t] = 0;
    aBoard[7][coolDown - t] = 0;
    aBoard[6][coolDown - t] = 0;
    aBoard[5][coolDown - t] = 0;
    aBoard[4][coolDown - t] = 0;
    t++;
    aBoard[9][coolDown - t] = 3;
    aBoard[8][coolDown - t] = 3;
    aBoard[7][coolDown - t] = 3;
    aBoard[6][coolDown - t] = 3;
    aBoard[5][coolDown - t] = 3;
    aBoard[4][coolDown - t] = 3;
    t++;
    aBoard[9][coolDown - t] = 3;
    aBoard[8][coolDown - t] = 3;
    aBoard[7][coolDown - t] = 0;
    aBoard[6][coolDown - t] = 0;
    aBoard[5][coolDown - t] = 0;
    aBoard[4][coolDown - t] = 3;
    t++;
    aBoard[9][coolDown - t] = 3;
    aBoard[8][coolDown - t] = 3;
    aBoard[7][coolDown - t] = 3;
    aBoard[6][coolDown - t] = 3;
    aBoard[5][coolDown - t] = 0;
    aBoard[4][coolDown - t] = 0;
    t++;
    aBoard[9][coolDown - t] = 3;
    aBoard[8][coolDown - t] = 3;
    aBoard[7][coolDown - t] = 0;
    aBoard[6][coolDown - t] = 0;
    aBoard[5][coolDown - t] = 0;
    aBoard[4][coolDown - t] = 3;
    t++;
    aBoard[9][coolDown - t] = 3;
    aBoard[8][coolDown - t] = 3;
    aBoard[7][coolDown - t] = 3;
    aBoard[6][coolDown - t] = 3;
    aBoard[5][coolDown - t] = 3;
    aBoard[4][coolDown - t] = 3;
    t++;
    aBoard[9][coolDown - t] = 0;
    aBoard[8][coolDown - t] = 0;
    aBoard[7][coolDown - t] = 0;
    aBoard[6][coolDown - t] = 0;
    aBoard[5][coolDown - t] = 0;
    aBoard[4][coolDown - t] = 0;
    t++;
    aBoard[9][coolDown - t] = 5;
    aBoard[8][coolDown - t] = 5;
    aBoard[7][coolDown - t] = 5;
    aBoard[6][coolDown - t] = 5;
    aBoard[5][coolDown - t] = 5;
    aBoard[4][coolDown - t] = 5;
    t++;
    aBoard[9][coolDown - t] = 5;
    aBoard[8][coolDown - t] = 0;
    aBoard[7][coolDown - t] = 5;
    aBoard[6][coolDown - t] = 5;
    aBoard[5][coolDown - t] = 0;
    aBoard[4][coolDown - t] = 5;
    t++;
    aBoard[9][coolDown - t] = 0;
    aBoard[8][coolDown - t] = 0;
    aBoard[7][coolDown - t] = 5;
    aBoard[6][coolDown - t] = 5;
    aBoard[5][coolDown - t] = 0;
    aBoard[4][coolDown - t] = 0;
    t++;
    aBoard[9][coolDown - t] = 0;
    aBoard[8][coolDown - t] = 0;
    aBoard[7][coolDown - t] = 5;
    aBoard[6][coolDown - t] = 5;
    aBoard[5][coolDown - t] = 0;
    aBoard[4][coolDown - t] = 0;
    t++;
    aBoard[9][coolDown - t] = 0;
    aBoard[8][coolDown - t] = 5;
    aBoard[7][coolDown - t] = 5;
    aBoard[6][coolDown - t] = 5;
    aBoard[5][coolDown - t] = 5;
    aBoard[4][coolDown - t] = 0;
    t++;
    aBoard[9][coolDown - t] = 0;
    aBoard[8][coolDown - t] = 0;
    aBoard[7][coolDown - t] = 0;
    aBoard[6][coolDown - t] = 0;
    aBoard[5][coolDown - t] = 0;
    aBoard[4][coolDown - t] = 0;
    t++;
    aBoard[9][coolDown - t] = 7;
    aBoard[8][coolDown - t] = 7;
    aBoard[7][coolDown - t] = 7;
    aBoard[6][coolDown - t] = 7;
    aBoard[5][coolDown - t] = 7;
    aBoard[4][coolDown - t] = 7;
    t++;
    aBoard[9][coolDown - t] = 7;
    aBoard[8][coolDown - t] = 7;
    aBoard[7][coolDown - t] = 0;
    aBoard[6][coolDown - t] = 0;
    aBoard[5][coolDown - t] = 0;
    aBoard[4][coolDown - t] = 7;
    t++;
    aBoard[9][coolDown - t] = 7;
    aBoard[8][coolDown - t] = 7;
    aBoard[7][coolDown - t] = 7;
    aBoard[6][coolDown - t] = 7;
    aBoard[5][coolDown - t] = 7;
    aBoard[4][coolDown - t] = 7;
    t++;
    aBoard[9][coolDown - t] = 7;
    aBoard[8][coolDown - t] = 7;
    aBoard[7][coolDown - t] = 0;
    aBoard[6][coolDown - t] = 7;
    aBoard[5][coolDown - t] = 7;
    aBoard[4][coolDown - t] = 0;
    t++;
    aBoard[9][coolDown - t] = 7;
    aBoard[8][coolDown - t] = 7;
    aBoard[7][coolDown - t] = 0;
    aBoard[6][coolDown - t] = 7;
    aBoard[5][coolDown - t] = 7;
    aBoard[4][coolDown - t] = 7;
    t++;
    aBoard[9][coolDown - t] = 0;
    aBoard[8][coolDown - t] = 0;
    aBoard[7][coolDown - t] = 0;
    aBoard[6][coolDown - t] = 0;
    aBoard[5][coolDown - t] = 0;
    aBoard[4][coolDown - t] = 0;
    t++;
    aBoard[9][coolDown - t] = 2;
    aBoard[8][coolDown - t] = 2;
    aBoard[7][coolDown - t] = 2;
    aBoard[6][coolDown - t] = 2;
    aBoard[5][coolDown - t] = 2;
    aBoard[4][coolDown - t] = 2;
    t++;
    aBoard[9][coolDown - t] = 0;
    aBoard[8][coolDown - t] = 0;
    aBoard[7][coolDown - t] = 2;
    aBoard[6][coolDown - t] = 2;
    aBoard[5][coolDown - t] = 0;
    aBoard[4][coolDown - t] = 0;
    t++;
    aBoard[9][coolDown - t] = 0;
    aBoard[8][coolDown - t] = 0;
    aBoard[7][coolDown - t] = 2;
    aBoard[6][coolDown - t] = 2;
    aBoard[5][coolDown - t] = 0;
    aBoard[4][coolDown - t] = 0;
    t++;
    aBoard[9][coolDown - t] = 0;
    aBoard[8][coolDown - t] = 0;
    aBoard[7][coolDown - t] = 2;
    aBoard[6][coolDown - t] = 2;
    aBoard[5][coolDown - t] = 0;
    aBoard[4][coolDown - t] = 0;
    t++;
    aBoard[9][coolDown - t] = 2;
    aBoard[8][coolDown - t] = 2;
    aBoard[7][coolDown - t] = 2;
    aBoard[6][coolDown - t] = 2;
    aBoard[5][coolDown - t] = 2;
    aBoard[4][coolDown - t] = 2;
    t++;
    aBoard[9][coolDown - t] = 0;
    aBoard[8][coolDown - t] = 0;
    aBoard[7][coolDown - t] = 0;
    aBoard[6][coolDown - t] = 0;
    aBoard[5][coolDown - t] = 0;
    aBoard[4][coolDown - t] = 0;
    t++;
    aBoard[9][coolDown - t] = 1;
    aBoard[8][coolDown - t] = 1;
    aBoard[7][coolDown - t] = 1;
    aBoard[6][coolDown - t] = 1;
    aBoard[5][coolDown - t] = 1;
    aBoard[4][coolDown - t] = 1;
    t++;
    aBoard[9][coolDown - t] = 1;
    aBoard[8][coolDown - t] = 1;
    aBoard[7][coolDown - t] = 0;
    aBoard[6][coolDown - t] = 0;
    aBoard[5][coolDown - t] = 0;
    aBoard[4][coolDown - t] = 0;
    t++;
    aBoard[9][coolDown - t] = 1;
    aBoard[8][coolDown - t] = 1;
    aBoard[7][coolDown - t] = 1;
    aBoard[6][coolDown - t] = 1;
    aBoard[5][coolDown - t] = 1;
    aBoard[4][coolDown - t] = 1;
    t++;
    aBoard[9][coolDown - t] = 0;
    aBoard[8][coolDown - t] = 0;
    aBoard[7][coolDown - t] = 0;
    aBoard[6][coolDown - t] = 0;
    aBoard[5][coolDown - t] = 1;
    aBoard[4][coolDown - t] = 1;
    t++;
    aBoard[9][coolDown - t] = 1;
    aBoard[8][coolDown - t] = 1;
    aBoard[7][coolDown - t] = 1;
    aBoard[6][coolDown - t] = 1;
    aBoard[5][coolDown - t] = 1;
    aBoard[4][coolDown - t] = 1;
    t++;
    aBoard[9][coolDown - t] = 0;
    aBoard[8][coolDown - t] = 0;
    aBoard[7][coolDown - t] = 0;
    aBoard[6][coolDown - t] = 0;
    aBoard[5][coolDown - t] = 0;
    aBoard[4][coolDown - t] = 0;
    t++;
}
void animationDeath()
{
    for (int y = 0; y <= 25; y++)
    {//fix mefix mefix mefix mefix mefix mefix mefix mefix mefix me
        for (int x = 0; x <= 9; x++)
        {
            if (gBoard[x][y] == 0)
            {
                aBoard[x][y] = 8;
            }
        }
    }
    for (int y = 0; y <= coolDownDeath; y++)
    {
        for (int x = 0; x <= 9; x++)
        {
            if (!(aBoard[x][y] == 8))
            {
                aBoard[x][y] = 9;
            }
        }
    }


}
void animationLineClear(int y)
{
    delayAnimationLineClear = millis() + 150;
    animationClearLatch = true;
    for (int x = 0; x <= 9; x++)
    {
        aBoard[x][y] = 9;
    }
    //clearLine();
}
