#include "game.h"

const int windowWidth = 700;
const int windowHeight = 1000;
const int szTable = 600;
const int xTable = ((windowWidth - szTable) / 2 + 1);
const int yTable = ((windowHeight - szTable) / 2 + 1);
int xClock, yClock, xtimeLiquid;
bool quit = false, isstartPage = true, islosePage = false;

// image things
SDL_Rect buttonClips[170];
Texture mainImage, tableBorder, mainPage, choosePlay, chooseScore;

// clock things
SDL_Rect timeClip;
int lenTimeliquid, timeClock;
Texture Clock, timeLiquid, onlyClock;

// button things
int buttonSize = 150, sz, totalUnfinished, curColor, saveUnfinished;
LButton a[10][10], playButton, scoreButton;
int colorList[10], sizeColorlist;

/*
    1: saveButton
    2: oldbuttonState: state of saveButton
*/
stack <dataRollback> rollbackList;

// sound things
const int totalSound = 4;
LSound completepathChunk, completelevelChunk, chooseChunk, chooseChunk1, soundtrack[10];
int cntMusic = 1e9, soundOrder[totalSound + 1];

Game::Game(SDL_Window* gWindow, SDL_Renderer* gRenderer, TTF_Font* gFont)
{
    window = gWindow;
    renderer = gRenderer;
    font = gFont;
    if (!loadMedia()) {
        cout << "Fail to load game media" << endl;
    }
}

Game::~Game() {
    window = NULL;
    renderer = NULL;
    font = NULL;
}

bool Game::loadMedia() {
    /*
        |  0     1      2      3      4      5     6     7       8       9     10 |
        |Blank, Blue, Brown, Green, Orange, Pink, Red, Yellow, Violet, Cyan, Block|
        |Unselected, Selected, Up, Right, Down, Left, LineUp, LineRight, LineDown, LineLeft,  LineRightUp, LineRightDown, LineLeftDown, LineLeftUp, LineLeftRight, LineUpDown, BridgeLeftRight, BridgeUpDown|
        |   1           2      3    4       5     6     7       8           9         10            11          12              13            14          15            16            17             18     |
        0 -> Blank
        Blue: 1 -> 18
        Brown: 19 -> 32
        ...
        Block: 163
        Bridge: 164
        SelectedBridge: 165
    */

    bool success = true;

    // load music and chunk
    if (!completepathChunk.loadChunk("../sound/StardewValleyPickupitems.mp3")) {
        success = false;
    }
    if (!completelevelChunk.loadChunk("../sound/doneLevel.mp3")) {
        success = false;
    }
    if (!chooseChunk.loadChunk("../sound/chooseSound.mp3")) {
        success = false;
    }
    if (!chooseChunk1.loadChunk("../sound/chooseSound1.mp3")) {
        success = false;
    }
    for (int i = 0; i < totalSound; i++) soundOrder[i] = i;
    if (!soundtrack[0].loadMusic("../sound/mainSong.mp3")) {
        success = false;
    }
    if (!soundtrack[1].loadMusic("../sound/2048Song.mp3")) {
        success = false;
    }
    if (!soundtrack[2].loadMusic("../sound/23conmuc.mp3")) {
        success = false;
    }
    if (!soundtrack[3].loadMusic("../sound/alreadydead.mp3")) {
        success = false;
    }

    // load image 
    if (!mainImage.loadTexture(renderer, "../images/Connect.png")) {
        success = false;
    }
    if (!tableBorder.loadTexture(renderer, "../images/tableBorder.png")) {
        success = false;
    }
    if (!Clock.loadTexture(renderer, "../images/Clock.png")) {
        success = false;
    }
    if (!timeLiquid.loadTexture(renderer, "../images/timeLiquid.png")) {
        success = false;
    }
    if (!onlyClock.loadTexture(renderer, "../images/onlyClock.png")) {
        success = false;
    }
    if (!mainPage.loadTexture(renderer, "../images/startPage.png")) {
        success = false;
    }
    if (!choosePlay.loadTexture(renderer, "../images/choosePlay.png")) {
        success = false;
    }
    if (!chooseScore.loadTexture(renderer, "../images/chooseScore.png")) {
        success = false;
    }
    xClock = ((windowWidth - Clock.dest.w) / 2 + 1);
    yClock = 870;
    xtimeLiquid = xClock + onlyClock.dest.w / 2;
    timeClip.x = 0;
    timeClip.y = 0;
    timeClip.w = timeLiquid.dest.w;
    timeClip.h = timeLiquid.dest.h;
    lenTimeliquid = timeLiquid.dest.w;
    Clock.setDest(xClock, yClock, Clock.dest.w, Clock.dest.h);
    timeLiquid.setDest(xClock + onlyClock.dest.w / 2, yClock, timeLiquid.dest.w, timeLiquid.dest.h);
    onlyClock.setDest(xClock, yClock, onlyClock.dest.w, onlyClock.dest.h);
    mainPage.setDest(0, 0, windowWidth, windowHeight);
    choosePlay.setDest(0, 0, windowWidth, windowHeight);
    chooseScore.setDest(0, 0, windowWidth, windowHeight);
    playButton.setPosition(256, 532, 166, 98);
    scoreButton.setPosition(238, 647, 191, 76);

    // Blank
    buttonClips[0].x = 0;
    buttonClips[0].y = 0;
    buttonClips[0].w = buttonSize;
    buttonClips[0].h = buttonSize;

    // Block
    buttonClips[163].x = buttonSize;
    buttonClips[163].y = 0;
    buttonClips[163].w = buttonSize;
    buttonClips[163].h = buttonSize;

    // Bridge
    buttonClips[164].x = buttonSize * 2;
    buttonClips[164].y = 0;
    buttonClips[164].w = buttonSize;
    buttonClips[164].h = buttonSize;

    // Selected Bridge
    buttonClips[165].x = buttonSize * 3;
    buttonClips[165].y = 0;
    buttonClips[165].w = buttonSize;
    buttonClips[165].h = buttonSize;

    // Remain
    int cnt = 1;
    sizeColorlist = 0;
    for (int i = 1; i <= 162; i++) {
        if (i % 18 == 1) colorList[++sizeColorlist] = i;
        buttonClips[i].x = ((i - 1) % 18) * buttonSize;
        buttonClips[i].y = cnt * buttonSize;
        buttonClips[i].w = buttonSize;
        buttonClips[i].h = buttonSize;
        if (i % 18 == 0) cnt++;
    }

    return success;
}

// just playing music ._.
void Game::playingMusic() {
    if (cntMusic > totalSound) {
        cntMusic = 0;
        random_shuffle(soundOrder, soundOrder + totalSound);
    }
    if (Mix_PlayingMusic() == 0) {
        soundtrack[soundOrder[cntMusic]].playMusic(0);
        cntMusic++;
    }
}

// load level ._.
bool Game::loadLevel(const char* path) {
    ifstream curLevel(path);
    /*if (!curLevel) {
        cout << "Can't load level: " << path << endl;
        return;
    }*/
    if (!curLevel) return false;

    cout << path << endl;
    random_shuffle(colorList + 1, colorList + sizeColorlist + 1);

    SDL_RenderClear(renderer);

    curLevel >> sz;

    tableBorder.setDest(xTable - 15, yTable - 15, 630 - 600 % sz, 630 - 600 % sz);
    tableBorder.drawTexture(renderer);

    resetTimeliquid();
    timeClock = 20;

    totalUnfinished = 0;
    for (int i = 1; i <= sz; i++)
        for (int j = 1; j <= sz; j++) {
            int x;
            curLevel >> x;
            if (x == -1) a[i][j].buttonState = 163;
            else if (x == -2) a[i][j].buttonState = 164;
            else if (x == 0) a[i][j].buttonState = 0;
            else a[i][j].buttonState = colorList[x];

            if (a[i][j].buttonState == 164) totalUnfinished += 2;
            else if (a[i][j].buttonState != 163) totalUnfinished++;

            // Draw into screen
            a[i][j].setPosition(xTable + (j - 1) * (szTable / sz), yTable + (i - 1) * (szTable / sz), (szTable / sz), (szTable / sz));
            a[i][j].render(renderer, mainImage, &buttonClips[a[i][j].buttonState]);
        }
    SDL_RenderPresent(renderer);
    return true;
}

void Game::resetTimeliquid() {
    timeLiquid.dest.w = lenTimeliquid;
    timeClip.x = 0;
    timeClip.y = 0;
    timeClip.w = timeLiquid.dest.w;
    timeClip.h = timeLiquid.dest.h;
    Clock.drawTexture(renderer);
    timeLiquid.render(renderer, &timeLiquid.dest, &timeClip);
    onlyClock.drawTexture(renderer);
}

void Game::movetimeliquidLeft() {
    if (timeClip.w == 0) return;
    timeClip.x++;
    timeClip.w--;
    timeLiquid.dest.w--;
    Clock.drawTexture(renderer);
    timeLiquid.render(renderer, &timeLiquid.dest, &timeClip);
    onlyClock.drawTexture(renderer);
    SDL_RenderPresent(renderer);
}

void Game::timeDecay(double& timeperTile, double& curTime) {
    while (SDL_GetTicks() - curTime >= timeperTile) {
        movetimeliquidLeft();
        curTime += timeperTile;
    }
}

void Game::RollBackOneButton() {
    for (int i = 0; i <= 1; i++) {
        LButton* curbutton = rollbackList.top().saveButton;
        int oldbuttonSave = rollbackList.top().oldbuttonState;
        rollbackList.pop();

        updateButton(curbutton, oldbuttonSave);
    }
    totalUnfinished++;
}

void Game::RollBack() {
    totalUnfinished = saveUnfinished;
    while (!rollbackList.empty()) {
        LButton* curbutton = rollbackList.top().saveButton;
        int oldbuttonSave = rollbackList.top().oldbuttonState;
        rollbackList.pop();
        updateButton(curbutton, oldbuttonSave);
        //SDL_RenderPresent(renderer); SDL_Delay(1000);
    }
}

//check if mouse is out of the table
bool Game::checkMouseouttable() {
    int x, y;
    SDL_GetMouseState(&x, &y);
    bool outside = false;
    if (x < xTable) outside = true;
    if (x >= a[sz][sz].mPosition.x + a[sz][sz].mPosition.w) outside = true;
    if (y < yTable) outside = true;
    if (y >= a[sz][sz].mPosition.y + a[sz][sz].mPosition.h) outside = true;
    return outside;
}

// check if move wrong at bridge
bool Game::checkBridge(LButton* curButton) {
    int posState = getPosstate(rollbackList.top().saveButton->mPosition.x, rollbackList.top().saveButton->mPosition.y, curButton->mPosition.x, curButton->mPosition.y);
    if (rollbackList.top().saveButton->buttonState == 166) {
        if (posState == 0 || posState == 2) return false;
    }
    if (rollbackList.top().saveButton->buttonState == 167) {
        if (posState == 1 || posState == 3) return false;
    }
    if (rollbackList.top().oldbuttonState == 167) {
        if (posState == 0 || posState == 2) return false;
    }
    if (rollbackList.top().oldbuttonState == 166) {
        if (posState == 1 || posState == 3) return false;
    }
    return true;
}

// if the path is not wrong ._.
void Game::Successful() {
    while (!rollbackList.empty()) rollbackList.pop();
}

// return {0, 1, 2, 3} if the curButton is {Up, Right, Down, Left} from the prevButton
int Game::getPosstate(int x, int y, int xx, int yy) {
    if (x == xx && y > yy) return 0; // Up
    if (y == yy && x < xx) return 1; // Right
    if (x == xx && y < yy) return 2; // Down
    return 3; // Left
}

LButton* Game::getSecondbutton() {
    dataRollback topButton = rollbackList.top();
    rollbackList.pop();
    LButton* secondButton = rollbackList.top().saveButton;
    rollbackList.push(topButton);
    return secondButton;
}


/*
    update curButtonState = updateState
*/
void Game::updateButton(LButton* curButton, int updateState) {
    // normalBridge
    if (updateState == 164) {
        curButton->render(renderer, mainImage, &buttonClips[164]);
    }
    // leftrightBridge
    else if (updateState == 166) {
        curButton->render(renderer, mainImage, &buttonClips[165]);
        curButton->render(renderer, mainImage, &buttonClips[curButton->bridgeleftrightColor + 16]);
    }
    // updownBridge
    else if (updateState == 167) {
        curButton->render(renderer, mainImage, &buttonClips[165]);
        curButton->render(renderer, mainImage, &buttonClips[curButton->bridgeupdownColor + 17]);
    }
    // fullBridge
    else if (updateState == 168) {
        if (curButton->buttonState == 166) {
            curButton->render(renderer, mainImage, &buttonClips[curColor + 17]);
        }
        else {
            curButton->render(renderer, mainImage, &buttonClips[curColor + 16]);
        }
    }
    else curButton->render(renderer, mainImage, &buttonClips[updateState]);
    curButton->buttonState = updateState;
}

/*
    |  0     1      2      3      4      5     6     7       8       9  |
    |Blank, Blue, Brown, Green, Orange, Pink, Red, Yellow, Violet, Block|
    |Unselected, Selected, Up, Right, Down, Left, LineUp, LineRight, LineDown, LineLeft,  LineRightUp, LineRightDown, LineLeftDown, LineLeftUp, LineLeftRight, LineUpDown|
    |   1           2      3    4       5     6     7       8           9         10            11          12              13            14          15            16
    0 -> Blank
    Blue: 1 -> 18
    Brown: 19 -> 32
*/

/*
    prevButton is the previous button
    curButton is the current button
    goal = true if the curButton is goal
*/
void Game::getNextstate(LButton* prevButton, LButton* curButton, bool goal) {
    // get prevState
    int prevState = prevButton->buttonState % 18;

    // get posState of two buttons
    int posState = getPosstate(prevButton->mPosition.x, prevButton->mPosition.y, curButton->mPosition.x, curButton->mPosition.y);
    totalUnfinished--;
    rollbackList.push({ prevButton, prevButton->buttonState });
    rollbackList.push({ curButton, curButton->buttonState });
    switch (prevState)
    {
        // Selected 
    case 2:
        updateButton(prevButton, prevButton->buttonState + posState + 1);
        break;

        ///////////////////////////////////////////////////////////////////////////////////

                // LineUp
    case 7:
        switch (posState)
        {
            // turn left
        case 3:
            updateButton(prevButton, prevButton->buttonState + 13 - prevState); // lineLeftDown 13
            break;
            // turn up
        case 0:
            updateButton(prevButton, prevButton->buttonState + 16 - prevState); // LineUpDown 16
            break;
            // turn right
        case 1:
            updateButton(prevButton, prevButton->buttonState + 12 - prevState); // LineRightDown 12
            break;
        }
        break;

        ///////////////////////////////////////////////////////////////////////////////////////

        // LineRight
    case 8:
        switch (posState)
        {
            // turn up
        case 0:
            updateButton(prevButton, prevButton->buttonState + 14 - prevState); // LineLeftUp 14
            break;
            // turn right
        case 1:
            updateButton(prevButton, prevButton->buttonState + 15 - prevState); // LineLeftRight 15
            break;
            // turn down
        case 2:
            updateButton(prevButton, prevButton->buttonState + 13 - prevState); // LineLeftDown 13
            break;
        }
        break;

        /////////////////////////////////////////////////////////////////////////////////////////

        //LineDown
    case 9:
        switch (posState)
        {
            // turn left
        case 3:
            updateButton(prevButton, prevButton->buttonState + 14 - prevState); // lineLeftUp 14
            break;
            // turn down
        case 2:
            updateButton(prevButton, prevButton->buttonState + 16 - prevState); // LineUpDown 16
            break;
            // turn right
        case 1:
            updateButton(prevButton, prevButton->buttonState + 11 - prevState); // LineRightUp 11
            break;
        }
        break;

        /////////////////////////////////////////////////////////////////////////////////////////////

        //LineLeft
    case 10:
        switch (posState)
        {
            // turn left
        case 3:
            updateButton(prevButton, prevButton->buttonState + 15 - prevState); // lineLeftRight 15
            break;
            // turn up
        case 0:
            updateButton(prevButton, prevButton->buttonState + 11 - prevState); // LineRightUp 11
            break;
            // turn down
        case 2:
            updateButton(prevButton, prevButton->buttonState + 12 - prevState); // LineRightDown 12
            break;
        }
        break;
    }

    // if curButton is normalBridge
    if (curButton->buttonState == 164) {
        if (posState == 1 || posState == 3) curButton->bridgeleftrightColor = curColor, updateButton(curButton, 166);
        else curButton->bridgeupdownColor = curColor, updateButton(curButton, 167);
    }
    // if curButton is leftrightBridge or updownBridge (next state is fullBridge)
    else if (curButton->buttonState == 166 || curButton->buttonState == 167) {
        if (curButton->buttonState == 166) curButton->bridgeupdownColor = curColor;
        else curButton->bridgeleftrightColor = curColor;
        updateButton(curButton, 168);
    }
    else if (goal == false) updateButton(curButton, curColor + 6 + posState);
    else updateButton(curButton, curColor + 2 + (posState + 2) % 4);
}

void Game::playGame() {
    double timeperTile = 1.0 * timeClock * 1000 / timeLiquid.dest.w;
    double curTime = SDL_GetTicks();

    while (totalUnfinished) {
        playingMusic();
        timeDecay(timeperTile, curTime);

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
                return;
            }
            if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) {
                // if move out the table => rollback
                if (checkMouseouttable() && !rollbackList.empty()) {
                    RollBack();
                    continue;
                }

                for (int i = 1; i <= sz; i++)
                    for (int j = 1; j <= sz; j++) if (a[i][j].checkMousein()) {
                        if (rollbackList.empty()) {
                            // deleted path
                            if (e.type == SDL_MOUSEBUTTONDOWN && a[i][j].buttonState < 164 && a[i][j].buttonState % 18 >= 3 && a[i][j].buttonState % 18 <= 6) {
                                // left and right of the same color
                                int le = a[i][j].buttonState - a[i][j].buttonState % 18;
                                int ri = le + 18;
                                for (int u = 1; u <= sz; u++)
                                    for (int v = 1; v <= sz; v++) if (a[u][v].buttonState > le && a[u][v].buttonState <= ri) {
                                        if (a[u][v].buttonState % 18 < 3 || a[u][v].buttonState % 18 > 6) updateButton(&a[u][v], 0);
                                        else updateButton(&a[u][v], le + 1);
                                        totalUnfinished++;
                                    }
                                    else if (a[u][v].buttonState > 164) {
                                        if (a[u][v].buttonState == 166 && a[u][v].bridgeleftrightColor == le + 1) {
                                            totalUnfinished++;
                                            updateButton(&a[u][v], 164);
                                        }
                                        if (a[u][v].buttonState == 167 && a[u][v].bridgeupdownColor == le + 1) {
                                            totalUnfinished++;
                                            updateButton(&a[u][v], 164);
                                        }
                                        if (a[u][v].buttonState == 168) {
                                            if (a[u][v].bridgeleftrightColor == le + 1) {
                                                totalUnfinished++;
                                                updateButton(&a[u][v], 167);
                                            }
                                            if (a[u][v].bridgeupdownColor == le + 1) {
                                                totalUnfinished++;
                                                updateButton(&a[u][v], 166);
                                            }
                                        }
                                    }
                                continue;
                            }

                            saveUnfinished = totalUnfinished;
                            // start a path
                            if (e.type == SDL_MOUSEBUTTONDOWN && a[i][j].buttonState != 163 && a[i][j].buttonState % 18 == 1) {
                                rollbackList.push({ &a[i][j], a[i][j].buttonState });

                                // save the curren color of the path
                                curColor = a[i][j].buttonState;

                                totalUnfinished--;

                                updateButton(&a[i][j], a[i][j].buttonState + 1);
                            }
                        }
                        else {
                            // if mouse up middle of path => wrong path
                            if (e.type == SDL_MOUSEBUTTONUP) {
                                RollBack();
                                //cout << "mouse up middle" << endl;
                                continue;
                            }

                            // if curbutton == stack top button => do nothing
                            if (&a[i][j] == rollbackList.top().saveButton) continue;

                            // if move at bridge => wrong path
                            if (checkBridge(&a[i][j]) == false) {
                                //cout << "Bridge false" << endl;
                                RollBack();
                                continue;
                            }

                            // if move into goal button => save the path
                            if (a[i][j].buttonState == curColor) {
                                completepathChunk.playChunk();
                                getNextstate(rollbackList.top().saveButton, &a[i][j], true);
                                Successful();
                                //cout << "goal" << endl;
                                continue;
                            }

                            //if move into a previous button => rollback once
                            if (rollbackList.size() >= 2 && &a[i][j] == getSecondbutton()) {
                                RollBackOneButton();
                                continue;
                            }

                            // if move into a button that != Blank & != Bridge => wrong path
                            if (a[i][j].buttonState && a[i][j].buttonState < 164) {
                                //cout << "button that != Blank" << endl;
                                RollBack();
                                continue;
                            }

                            //cout << "getNextstate" << endl;
                            getNextstate(rollbackList.top().saveButton, &a[i][j], false);

                        }
                    }
            }
            SDL_RenderPresent(renderer);
        }
    }
    completelevelChunk.playChunk();
    SDL_Delay(500);
}

void Game::gameStart() {
    playingMusic();
    //loadLevel("../level/5normal10.txt");
    //playGame();
    for (int i = 4; i <= 9; i++)
        for (int j = 1; j <= 12; j++)
        {
            {
                string name = "../level/" + to_string(i);
                name += "easy" + to_string(j) + ".txt";
                if (loadLevel(name.c_str())) {
                    playGame();
                    if (quit == true) return;
                }
                name = "../level/" + to_string(i);
                name += "normal" + to_string(j) + ".txt";
                if (loadLevel(name.c_str())) {
                    playGame();
                    if (quit == true) return;
                }
                name = "../level/" + to_string(i);
                name += "hard" + to_string(j) + ".txt";
                if (loadLevel(name.c_str())) {
                    playGame();
                    if (quit == true) return;
                }
            }
        }
}

void Game::gameLoop() {
    SDL_Surface* cursorSurface = IMG_Load("../images/Mouse1.png");
    // Create new mouse from image
    SDL_Cursor* cursor = SDL_CreateColorCursor(cursorSurface, 1, 1);
    // Set up new mouse
    SDL_SetCursor(cursor);

    //Event handler
    SDL_Event e;
    bool mouseinPlay = false, mouseinScore = false;

    //While application is running
    while (!quit)
    {

        //Handle events on queue
        while (SDL_PollEvent(&e) != 0)
        {
            //User requests quit
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            if (!playButton.checkMousein()) playButton.mousein = false;
            if (!scoreButton.checkMousein()) scoreButton.mousein = false;
            if (playButton.checkMousein() && e.type == SDL_MOUSEBUTTONDOWN) {
                isstartPage = false;
                gameStart();
            }
        }
        if (isstartPage) {
            SDL_RenderClear(renderer);
            mainPage.drawTexture(renderer);
            if (playButton.checkMousein()) {
                playButton.checkchunkwhenMousein(&chooseChunk, &chooseChunk1);
                choosePlay.drawTexture(renderer);
            }
            if (scoreButton.checkMousein()) {
                scoreButton.checkchunkwhenMousein(&chooseChunk, &chooseChunk1);
                chooseScore.drawTexture(renderer);
            }
            SDL_RenderPresent(renderer);
        }
    }
}


