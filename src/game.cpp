#include "game.h"

const int windowWidth = 700;
const int windowHeight = 1000;
const int szTable = 600;
const int xTable = ((windowWidth - szTable) / 2 + 1);
const int yTable = ((windowHeight - szTable) / 2 + 1);
int xClock, yClock, xtimeLiquid;
bool quit = false, isstartPage = true, islosePage = false, quitGame = false;

// image things
SDL_Rect buttonClips[170];
Texture mainImage, tableBorder, mainPage, choosePlay, chooseScore, gameOver, topThings, muteMusic, unmuteMusic;
Texture pauseGame, chooseReturn, chooseMainMenu;

// clock things
SDL_Rect timeClip;
int lenTimeliquid, timeClock;
Texture Clock, timeLiquid, onlyClock;

// button things
int buttonSize = 150, sz, totalUnfinished, curColor, saveUnfinished;
LButton a[10][10], playButton, scoreButton, musicButton, pauseButton, returnButton, mainmenuButton;
int colorList[10], sizeColorlist;
/*
    1: saveButton
    2: oldbuttonState: state of saveButton
*/
stack <dataRollback> rollbackList;

// sound things
const int totalSound = 4;
LSound completepathChunk, completelevelChunk, chooseChunk, chooseChunk1, soundtrack[10], gameoverSound;
int cntMusic = 1e9, soundOrder[totalSound + 1];

// ttf things
Texture playerName;

Game::Game(SDL_Window* gWindow, SDL_Renderer* gRenderer)
{
    window = gWindow;
    renderer = gRenderer;
    if (!loadMedia()) {
        cout << "Fail to load game media" << endl;
    }
}

Game::~Game() {
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    TTF_CloseFont(font);
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
    if (!gameoverSound.loadChunk("../sound/gameoverSound.mp3")) {
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
    if (!gameOver.loadTexture(renderer, "../images/gameOver.png")) {
        success = false;
    }
    if (!topThings.loadTexture(renderer, "../images/topThings.png")) {
        success = false;
    }
    if (!muteMusic.loadTexture(renderer, "../images/muteMusic.png")) {
        success = false;
    }
    if (!unmuteMusic.loadTexture(renderer, "../images/unmuteMusic.png")) {
        success = false;
    }
    if (!pauseGame.loadTexture(renderer, "../images/pauseGame.png")) {
        success = false;
    }
    if (!chooseReturn.loadTexture(renderer, "../images/chooseReturn.png")) {
        success = false;
    }
    if (!chooseMainMenu.loadTexture(renderer, "../images/chooseMainMenu.png")) {
        success = false;
    }

    // clock, timeliquid things
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

    // mainPage things
    mainPage.setDest(0, 0, windowWidth, windowHeight);
    choosePlay.setDest(0, 0, windowWidth, windowHeight);
    chooseScore.setDest(0, 0, windowWidth, windowHeight);
    playButton.setPosition(256, 532, 166, 98);
    scoreButton.setPosition(238, 647, 191, 76);

    // playPage things
    returnButton.setPosition(185, 352, 339, 115);
    mainmenuButton.setPosition(147, 539, 440, 119);

    //music and menu button
    musicButton.setPosition(539, 49, 43, 43);
    pauseButton.setPosition(611, 49, 43, 43);

    // Blank
    buttonClips[0] = { 0, 0, buttonSize, buttonSize };

    // Block
    buttonClips[163] = { buttonSize, 0, buttonSize, buttonSize };

    // Bridge
    buttonClips[164] = { buttonSize * 2, 0, buttonSize, buttonSize };

    // Selected Bridge
    buttonClips[165] = { buttonSize * 3, 0, buttonSize, buttonSize };

    // Remain
    int cnt = 1;
    sizeColorlist = 0;
    for (int i = 1; i <= 162; i++) {
        if (i % 18 == 1) colorList[++sizeColorlist] = i;
        buttonClips[i] = { ((i - 1) % 18) * buttonSize, cnt * buttonSize, buttonSize, buttonSize };
        if (i % 18 == 0) cnt++;
    }

    // ttf things
    // font = TTF_OpenFont("../ttf/Bitterbrush DEMO.otf", 30);
    //font = TTF_OpenFont("../ttf/Baloo-Regular.ttf", 60);
    font = TTF_OpenFont("../ttf/Pacifico-Regular.ttf", 40);
    // font = TTF_OpenFont("../ttf/NotoMono-Regular.ttf", 60);
    // font = TTF_OpenFont("../ttf/DroidSansMono.ttf", 60);
    // font = TTF_OpenFont("../ttf/ShortBaby-Mg2w.ttf", 60);
    if (font == NULL)
    {
        printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
        success = false;
    }
    else
    {
        //Render text
        SDL_Color textColor = { 0, 0, 0 };
        if (!playerName.loadFromRenderedText(renderer, font, "Lợi Vua Nối 342689", textColor))
        {
            printf("Failed to render text texture!\n");
            success = false;
        }
    }

    return success;
}

// just music things ._.   
void Game::stopMusic()
{
    if (Mix_PausedMusic() == 0) Mix_PauseMusic();
}
void Game::continueMusic() {
    if (Mix_PausedMusic() == 1) Mix_ResumeMusic();
}
void Game::resetMusic() {
    Mix_HaltMusic();
    cntMusic = 0;
    random_shuffle(soundOrder, soundOrder + totalSound);
}
void Game::playingMusic() {
    if (cntMusic > totalSound) resetMusic();
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
    topThings.drawTexture(renderer);
    if (Mix_PausedMusic()) muteMusic.drawTexture(renderer);
    resetTimeliquid();

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
    if (y < yTable) outside = true;
    if (x >= a[sz][sz].mPosition.x + a[sz][sz].mPosition.w) outside = true;
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

void Game::pauseGamePls() {
    while (true) {
        pauseGame.drawTexture(renderer);
        if (returnButton.checkMousein()) {
            returnButton.checkchunkwhenMousein(&chooseChunk, &chooseChunk1);
            chooseReturn.drawTexture(renderer);
        }
        if (mainmenuButton.checkMousein()) {
            mainmenuButton.checkchunkwhenMousein(&chooseChunk, &chooseChunk1);
            chooseMainMenu.drawTexture(renderer);
        }
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
                return;
            }
            // music button
            if (e.type == SDL_MOUSEBUTTONDOWN && musicButton.checkMousein()) {
                if (Mix_PausedMusic()) {
                    unmuteMusic.drawTexture(renderer);
                    continueMusic();
                }
                else {
                    muteMusic.drawTexture(renderer);
                    stopMusic();
                }
            }

            // return and mainmenu buttons
            if (!returnButton.checkMousein()) returnButton.mousein = false;
            else if (e.type == SDL_MOUSEBUTTONDOWN) return;
            if (!mainmenuButton.checkMousein()) mainmenuButton.mousein = false;
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                quitGame = true;
                isstartPage = true;
                resetMusic();
                return;
            }
        }
        SDL_RenderPresent(renderer);
    }
}

void Game::playGame() {
    double timeperTile = 1.0 * timeClock / timeLiquid.dest.w;
    double curTime = SDL_GetTicks();

    playingMusic();

    while (totalUnfinished) {
        if (quit) return;
        if (quitGame) return;

        // Time up
        if (timeClip.w <= 0) {
            quitGame = true;
            stopMusic();
            resetMusic();
            gameoverSound.playChunk();
            SDL_Delay(2000);
            return;
        }
        // check still playing music
        playingMusic();
        // time decay
        timeDecay(timeperTile, curTime);

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
                return;
            }
            if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) {
                // musicButton
                if (e.type == SDL_MOUSEBUTTONDOWN && musicButton.checkMousein()) {
                    if (Mix_PausedMusic()) {
                        unmuteMusic.drawTexture(renderer);
                        continueMusic();
                    }
                    else {
                        muteMusic.drawTexture(renderer);
                        stopMusic();
                    }
                }
                // pauseButton
                if (e.type == SDL_MOUSEBUTTONDOWN && pauseButton.checkMousein()) {
                    pauseGamePls();
                    for (int i = 1; i <= sz; i++)
                        for (int j = 1; j <= sz; j++) a[i][j].render(renderer, mainImage, &buttonClips[a[i][j].buttonState]);
                    curTime = SDL_GetTicks();
                    continue;
                }

                // if move out the table => rollback
                if (!rollbackList.empty() && checkMouseouttable()) {
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
                            if (e.type == SDL_MOUSEBUTTONDOWN && a[i][j].buttonState < 163 && a[i][j].buttonState % 18 == 1) {
                                rollbackList.push({ &a[i][j], a[i][j].buttonState });

                                // save the curren color of the path
                                curColor = a[i][j].buttonState;

                                totalUnfinished--;

                                updateButton(&a[i][j], a[i][j].buttonState + 1);
                            }
                        }
                        else {
                            // if move into goal button and mouseup => save the path
                            if (e.type == SDL_MOUSEBUTTONUP && rollbackList.top().oldbuttonState == curColor && rollbackList.size() > 1) {
                                completepathChunk.playChunk();
                                Successful();
                                continue;
                            }

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

                            //if move into a previous button => rollback once
                            if (rollbackList.size() >= 2 && &a[i][j] == getSecondbutton()) {
                                RollBackOneButton();
                                continue;
                            }

                            // If standing in goal button but move into a new button => save the path
                            if (rollbackList.top().oldbuttonState == curColor && rollbackList.size() > 1) {
                                completepathChunk.playChunk();
                                Successful();
                                continue;
                            }

                            // if move into a button that != Blank, != Bridge, ! Goal => wrong path
                            if (a[i][j].buttonState && a[i][j].buttonState != curColor && a[i][j].buttonState < 164) {
                                //cout << "button that != Blank" << endl;
                                RollBack();
                                continue;
                            }


                            //cout << "getNextstate" << endl;
                            if (a[i][j].buttonState != curColor) getNextstate(rollbackList.top().saveButton, &a[i][j], false);
                            else getNextstate(rollbackList.top().saveButton, &a[i][j], true);

                        }
                    }
            }
            SDL_RenderPresent(renderer);
        }
    }
    Successful();
    completelevelChunk.playChunk();
    SDL_Delay(500);
}

// balance things
// size          0  1  2  3  4  5  6  7   8  9
int easy[10] = { 0, 0, 0, 0, 0, 15, 8, 14, 7, 13 };
// size            0  1  2  3   4  5   6   7   8   9
int normal[10] = { 0, 0, 0, 0, 20, 20, 20, 25, 30, 35 };
// size          0  1  2  3  4  5  6   7   8   9
int hard[10] = { 0, 0, 0, 0, 0, 30, 30, 35, 40, 45 };

void Game::gameStart() {
    playingMusic();
    //    loadLevel("../level/9hard3.txt");
    //  timeClock = 40000;
    // playGame();
    for (int i = 4; i <= 9; i++)
        for (int j = 1; j <= 12; j++)
        {
            {
                string name = "../level/" + to_string(i);
                name += "easy" + to_string(j) + ".txt";
                if (loadLevel(name.c_str())) {
                    timeClock = easy[i] * 1000;
                    playGame();
                    if (quit == true) return;
                    if (quitGame == true) return;
                }
                name = "../level/" + to_string(i);
                name += "normal" + to_string(j) + ".txt";
                if (loadLevel(name.c_str())) {
                    timeClock = normal[i] * 1000;
                    playGame();
                    if (quit == true) return;
                    if (quitGame == true) return;
                }
                name = "../level/" + to_string(i);
                name += "hard" + to_string(j) + ".txt";
                if (loadLevel(name.c_str())) {
                    timeClock = hard[i] * 1000;
                    playGame();
                    if (quit == true) return;
                    if (quitGame == true) return;
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
            if (isstartPage && !playButton.checkMousein()) playButton.mousein = false;
            if (isstartPage && !scoreButton.checkMousein()) scoreButton.mousein = false;
            if (isstartPage && playButton.checkMousein() && e.type == SDL_MOUSEBUTTONDOWN) {
                isstartPage = false;
                quitGame = false;
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
        else {
            SDL_RenderClear(renderer);
            gameOver.drawTexture(renderer);
            SDL_RenderPresent(renderer);
        }
    }
}


