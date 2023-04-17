#include "game.h"

const int windowWidth = 700;
const int windowHeight = 1000;
const int szTable = 600;
const int xTable = ((windowWidth - szTable) / 2 + 1);
const int yTable = ((windowHeight - szTable) / 2 + 1);

// get image things
SDL_Rect buttonClips[150];
Texture mainImage, tableBorder;

// button things
int buttonSize = 150, sz, totalUnfinished, curColor, saveUnfinished;
LButton a[10][10];
int colorList[10], sizeColorlist;

stack <dataRollback> rollbackList;

Game::Game(SDL_Window* gWindow, SDL_Renderer* gRenderer)
{
    window = gWindow;
    renderer = gRenderer;
    if (!loadMedia()) {
        cout << "Fail to load game media" << endl;
    }
}

Game::~Game() {
    window = NULL;
    renderer = NULL;
}

bool Game::loadMedia() {

    /*
        |  0     1      2      3      4      5     6     7       8       9  |
        |Blank, Blue, Brown, Green, Orange, Pink, Red, Yellow, Violet, Block|
        |Unselected, Selected, Up, Right, Down, Left, LineUp, LineRight, LineDown, LineLeft,  LineRightUp, LineRightDown, LineLeftDown, LineLeftUp, LineLeftRight, LineUpDown|
        |   1           2      3    4       5     6     7       8           9         10            11          12              13            14          15            16
        0 -> Blank
        Blue: 1 -> 16
        Brown: 17 -> 32
    */

    bool success = true;
    if (!mainImage.loadTexture(renderer, "../images/Connect.png")) {
        success = false;
    }
    if (!tableBorder.loadTexture(renderer, "../images/tableBorder.png")) {
        success = false;
    }

    // Blank
    buttonClips[0].x = 0;
    buttonClips[0].y = 0;
    buttonClips[0].w = buttonSize;
    buttonClips[0].h = buttonSize;

    // Block
    buttonClips[145].x = buttonSize;
    buttonClips[145].y = 0;
    buttonClips[145].w = buttonSize;
    buttonClips[145].h = buttonSize;

    // Remain
    int cnt = 1;
    sizeColorlist = 0;
    for (int i = 1; i <= 144; i++) {
        if (i % 16 == 1) colorList[++sizeColorlist] = i;
        buttonClips[i].x = ((i - 1) % 16) * buttonSize;
        buttonClips[i].y = cnt * buttonSize;
        buttonClips[i].w = buttonSize;
        buttonClips[i].h = buttonSize;
        if (i % 16 == 0) cnt++;
    }

    return success;
}

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
    tableBorder.setDest(xTable - 15, yTable - 15, 630, 630);
    tableBorder.drawTexture(renderer);


    totalUnfinished = 0;

    curLevel >> sz;
    for (int i = 1; i <= sz; i++)
        for (int j = 1; j <= sz; j++) {
            int x;
            curLevel >> x;
            if (x == -1) a[i][j].buttonState = 145;
            else if (x == 0) a[i][j].buttonState = 0;
            else a[i][j].buttonState = colorList[x];

            if (a[i][j].buttonState != 145) totalUnfinished++;

            // Draw into screen
            a[i][j].setPosition(xTable + (j - 1) * (szTable / sz), yTable + (i - 1) * (szTable / sz), (szTable / sz), (szTable / sz));
            a[i][j].render(renderer, mainImage, &buttonClips[a[i][j].buttonState]);
        }
    return true;
}

void Game::RollBackOneButton() {
    for (int i = 0; i <= 1; i++) {
        LButton* curbutton = rollbackList.top().saveButton;
        int oldbuttonSave = rollbackList.top().oldbuttonState;
        rollbackList.pop();

        curbutton->buttonState = oldbuttonSave;
        curbutton->render(renderer, mainImage, &buttonClips[curbutton->buttonState]);
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
    }
}

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

void Game::Successful() {
    while (!rollbackList.empty()) rollbackList.pop();
}

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

void Game::updateButton(LButton* curButton, int updateState) {
    curButton->buttonState = updateState;
    curButton->render(renderer, mainImage, &buttonClips[updateState]);
}

/*
    |  0     1      2      3      4      5     6     7       8       9  |
    |Blank, Blue, Brown, Green, Orange, Pink, Red, Yellow, Violet, Block|
    |Unselected, Selected, Up, Right, Down, Left, LineUp, LineRight, LineDown, LineLeft,  LineRightUp, LineRightDown, LineLeftDown, LineLeftUp, LineLeftRight, LineUpDown|
    |   1           2      3    4       5     6     7       8           9         10            11          12              13            14          15            16
    0 -> Blank
    Blue: 1 -> 16
    Brown: 17 -> 32
*/

void Game::getNextstate(LButton* prevButton, LButton* curButton, bool goal) {
    // get prevState
    int prevState = prevButton->buttonState % 16;

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
    if (goal == false) updateButton(curButton, curColor + 6 + posState);
    else updateButton(curButton, curColor + 2 + (posState + 2) % 4);
}

void Game::playGame() {
    while (totalUnfinished) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                exit(0);
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
                            if (e.type == SDL_MOUSEBUTTONDOWN && a[i][j].buttonState % 16 >= 3 && a[i][j].buttonState % 16 <= 6) {
                                // left and right of the same color
                                int le = a[i][j].buttonState - a[i][j].buttonState % 16;
                                int ri = le + 16;

                                for (int u = 1; u <= sz; u++)
                                    for (int v = 1; v <= sz; v++) if (a[u][v].buttonState > le && a[u][v].buttonState <= ri) {
                                        if (a[u][v].buttonState % 16 < 3 || a[u][v].buttonState % 16 > 6) updateButton(&a[u][v], 0);
                                        else updateButton(&a[u][v], le + 1);
                                        totalUnfinished++;
                                    }
                                continue;
                            }

                            saveUnfinished = totalUnfinished;
                            // start a path
                            if (e.type == SDL_MOUSEBUTTONDOWN && a[i][j].buttonState != 145 && a[i][j].buttonState % 16 == 1) {
                                rollbackList.push({ &a[i][j], a[i][j].buttonState });

                                // save the curren color of the path
                                curColor = a[i][j].buttonState;

                                totalUnfinished--;

                                updateButton(&a[i][j], a[i][j].buttonState + 1);
                            }
                        }
                        else {
                            // if move into goal button => save the path
                            if (a[i][j].buttonState == curColor) {
                                getNextstate(rollbackList.top().saveButton, &a[i][j], true);
                                Successful();
                                //cout << "goal" << endl;
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

                            //if move into a previous button => rollback once
                            if (rollbackList.size() >= 2 && &a[i][j] == getSecondbutton()) {
                                RollBackOneButton();
                                continue;
                            }

                            // if move into a button that != Blank => wrong path
                            if (a[i][j].buttonState) {
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
    SDL_Delay(500);
}

void Game::gameStart() {
    loadLevel("../level/9normal8.txt");
    playGame();
    /*for (int i = 4; i <= 9; i++) {
        for (int j = 1; j <= 12; j++) {
            string name = "../level/" + to_string(i);
            name += "easy" + to_string(j) + ".txt";
            if (loadLevel(name.c_str())) {
                playGame();
            }
            name = "../level/" + to_string(i);
            name += "normal" + to_string(j) + ".txt";
            if (loadLevel(name.c_str())) {
                playGame();
            }
            name = "../level/" + to_string(i);
            name += "hard" + to_string(j) + ".txt";
            if (loadLevel(name.c_str())) {
                playGame();
            }
        }
    }*/
}

void Game::gameLoop() {
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;
    Uint32 frameStart;
    int frameTime;

    //Main loop flag
    bool quit = false;

    //Event handler
    SDL_Event e;

    //While application is running
    while (!quit)
    {
        frameStart = SDL_GetTicks();

        //Handle events on queue
        while (SDL_PollEvent(&e) != 0)
        {
            //User requests quit
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
        }
        gameStart();
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
}


