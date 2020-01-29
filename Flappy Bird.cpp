#include <iostream>
#include <graphics.h>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <cstdio>
#include <fstream>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>

using namespace std;

/**
 * Window Stuff.
 */
const int windowWidth = 500;
const int windowHeight = 600;
const float fps = 1000/60;

/**
 * Score
 */
int playerScore = 0;
int highscore = 0;
bool newHighscore = false;
string playerName;
string highscoreName;
bool enterName = false;


/**
 * Functions.
 */
/// Write save file.
void saveGameData() {

    /// Construct a JSON string.
    string json = "{ \"highscore\": " + to_string(highscore) + ", \"highscoreName\": " + "\"" + highscoreName + "\"" + +" }";

    /// Instantiate new Document instance.
    rapidjson::Document document;

    /// Parse JSON string to JSON object.
    document.Parse(json.c_str());
    
    /// Open the gamedata.json file for writing.
    FILE *file = fopen("gamedata.json", "w");
    
    /// Create a output stream that writes to the buffer.
    char writeBuffer[2560];
    rapidjson::FileWriteStream os(file, writeBuffer, sizeof(writeBuffer));

    /// Create a Writer that writes to output stream. 
    rapidjson::Writer<rapidjson::FileWriteStream> writer(os);

    /// Output JSON from document to JSON text by Writer.
    document.Accept(writer);

    /// Close file.
    fclose(file);

}

/// Read save file.
void readGameData() {

    /// Open file gamedata.json for reading.
    FILE *file = fopen("gamedata.json", "r");

    if (file != NULL) {
    
        /// Create a input stream that reads to the buffer.
        char readBuffer[2560];
        rapidjson::FileReadStream is(file, readBuffer, sizeof(readBuffer));

        /// Instantiate new Document object.
        rapidjson::Document document;
        
        /// Parse the input stream to JSON object.
        document.ParseStream(is);

        /// Close file.
        fclose(file);

        /// Set the score.
        highscore = document["highscore"].GetInt();
        highscoreName = document["highscoreName"].GetString();

    } else {
        saveGameData();
    }

}

/// Sign.
int _sign(int num) {
    if (num < 0) {
        return -1;
    } else if (num > 0) {
        return 0;
    } else if (num > 0) {
        return 1;
    }
}

/**
 * Classes
 */
/// Bird
class Bird {
    public:
        /// Properties
        bool isDead = false;

        /// Position.
        const int x = 150;
        const int startY = 300;
        int y = startY;
        
        /// Width and Height;
        const int width = 50;
        const int height = 40;

        /// Movement
        int vSpeed = 0;
        const float gravity = 2.0f;
        const int maxFallSpeed = 30;
        const int upForce = 19;

        /// Falp animation
        const int maxWingSize = 12;
        int wingSize = maxWingSize;
        int wingAnimationSpeed = 2;
        int wingDirection = -1;

        /// Flap timer.
        const int maxFlapDelay = 80;
        int flapDelay = 0;

        /// Update method.
        void update() {
            flapDelay -= fps;

            /// Apply gravity to bird.
            if (vSpeed <= maxFallSpeed)
                vSpeed += gravity;
            else
                vSpeed = maxFallSpeed;

            /// Update bird's position.
            y += vSpeed;

            /// Check for boundaries
            if (y <= 0)
                y = 0;
            else if (y+height/2 >= windowHeight-30) {
                y = windowHeight-30-height/2;
                die();
            }

        }

        /// Render method.
        void render() {
            /// Draw bird.
            setcolor(14); // Yellow
            setfillstyle(1, 14); /// Solid Fill, Yellow.
            fillellipse(x, y, width/2, height/2);

            /// Draw bird features (eyes and wings).
            /// Eyes
            setcolor(0); /// black
            setfillstyle(1, 0); /// Solide fill, black.
            fillellipse(x+width/2/2, y-height/2/2, 2, 2);

            /// Wings
            /// Animation;
            wingSize += wingAnimationSpeed * wingDirection;
            if (wingSize > maxWingSize)
                wingDirection = -1;
            if (wingSize < 0)
                wingDirection = 1;

            setcolor(0); /// Black
            ellipse(x-10, y, 180, 0, 10, wingSize);
        }

        /// Check collision.
        void checkCollision(int ox, int oy, int ow, int oh) {
            if (x-width/2 >= ox+ow || x+width/2 <= ox || y-height/2 >= oy+oh || y+height/2 <= oy) {
                return;
            }
            die();
        }

        void die() {
            vSpeed = 0;
            isDead = true;

            /// Update highscore
            if (playerScore > highscore) {
                highscore = playerScore;
                newHighscore = true;
                enterName = true;
            }
        }

        /// Restart method. (Reset bird's variables).
        void reset() {
            y = startY;
            vSpeed = 0;
            flapDelay = 0;
            isDead = false;
        }

};

/// Pipe class.
class Pipe {
    public:
        /// Pipe position and size.
        int x = windowWidth;
        const int y = 0;
        const int width = 100;
        const int height = windowHeight-30;

        /// Gap properties.
        int gapX = x;
        int gapY = 0;
        const int gapWidth = width;
        const int gapHeight = 150;
        const int minSpace = 20; /// The smallest height the pipes can be.

        /// isCleared.
        bool isCleared = false;

        /// Speed.
        const float speed = 6.0f;

        /// Color
        const int color = 10; /// Light Green.

        /// Class Constructor.
        Pipe() {
            /// Randomly create the gap.
            gapY = rand() % ((height - gapHeight - minSpace) - minSpace) + minSpace;
        }
    
        /// Update Method.
        void update() {
            x = x - speed;
        }

        /// Render method.
        void render() {
            /// Draw upper half of pipe.
            setcolor(color);
            setfillstyle(1, color); /// Solid Fill.
            bar(x, y, x+width, y+gapY);
            bar(x-6, y+gapY-50, x+width+6, y+gapY); // Pipe detail

            /// Lower half.
            bar(x, gapY+gapHeight, x+width, gapY+gapHeight+(windowHeight-(gapY+gapHeight)-30));
            bar(x-6, gapY+gapHeight, x+width+6, gapY+gapHeight+50); /// Pipe detail.

            setcolor(0); /// black.
            setfillstyle(1, 0); /// solid fill, black
            bar(x, gapY+gapHeight+50, x+width, gapY+gapHeight+55); /// Lower half Shadow.
        }
};

/**
 * Global Variables
 */
Bird * bird;
vector<Pipe*> pipes;

const int maxPipeSpawnDelay = 1200;
int pipeSpawnTime = maxPipeSpawnDelay;

bool isGameStarted = false;
bool isGameOver = false;

/**
 * Input function.
 */
void input() {

    /// Check for keyboard input.
    if (!kbhit())
        return;

    cin.clear();

    /// Check for space.
    char input = getch();

    if (!bird->isDead) {
        if (input == ' ') {
            /// Add upForce to vSpeed.
            if (bird->flapDelay <= 0) {
                bird->vSpeed = -bird->upForce;
                bird->flapDelay = bird->maxFlapDelay;
            }

            if (!isGameStarted)
                isGameStarted = true;
        }
    } else {
        if (!enterName) {
            if (tolower(input) == 'r') {
                /// Reset bird's variables.
                bird->reset();

                /// Clear the pipes vector and reset pipe timer.
                pipes.clear();
                pipeSpawnTime = maxPipeSpawnDelay;

                /// Reset player's points.
                playerScore = 0;

                /// update isGameStarted.
                isGameStarted = false;
                newHighscore = false;
            }

            if (tolower(input) == 'q') {
                isGameOver = true;
            }
        } else {
            if (input == 13) { /// Enter
                highscoreName = playerName;
                saveGameData();
                enterName = false;
                playerName = "";
                return;
            } else if (input == 8) { /// Backspace
                string temp = playerName.substr(0, playerName.length()-1);
                playerName = temp;
                return;
            } else {
                playerName = playerName + input;
                return;
            }
        }
    }

}

/**
 * Update function.
 */
void update() {

    if (!isGameStarted)
        return;

    /// Bird update.
    bird->update();

    /// Create the pipes.
    if (!bird->isDead) {
        if (pipeSpawnTime <= 0) {
            pipes.push_back(new Pipe());
            pipeSpawnTime = maxPipeSpawnDelay;
        } else { 
            pipeSpawnTime -= fps;
        }
    }

    /// Pipe update.
    if (!bird->isDead) {
        for (int i=0; i<pipes.size(); i++) {
            /// Remove pipe from vector (off-screen).
            if (pipes[i]->x+pipes[i]->width <= 0) {
                pipes.erase(pipes.begin());
                continue;
            }

            /// Collision with upper half of pipe.
            bird->checkCollision(pipes[i]->x, pipes[i]->y, pipes[i]->width, pipes[i]->gapY);

            /// Collision with lower half of pipe.
            bird->checkCollision(pipes[i]->x, pipes[i]->gapY+pipes[i]->gapHeight, pipes[i]->width, pipes[i]->height);

            /// Check if bird cleared indexed pipe.
            if (!pipes[i]->isCleared) {
                if (bird->x > pipes[i]->x+pipes[i]->width/2) {
                    pipes[i]->isCleared = true;
                    /// Increment player's score.
                    playerScore = playerScore + 1;
                }
            }

            /// Run pipe's update method.
            pipes[i]->update();
        }
    }

}

/**
 * Render function.
 */
void render() {

    /// Clear the screen.
    cleardevice();

    /// Set background.
    setbkcolor(9);

    /// Render pipe.
    for (int i=0; i<pipes.size(); i++) {
        pipes[i]->render();
    }

    /// Ground
    setcolor(6); /// Brown
    setfillstyle(1, 6); /// Solid, brown.
    bar(0, windowHeight-30, windowWidth, windowHeight);

    /// Render bird.
    bird->render();

    /**
     * Write text
     */
    char str[256];

    /// Score.
    setcolor(15); /// White.
    settextstyle(0,0, 3);
    settextjustify(1, 1); /// Center, center.
    strcpy(str, to_string(playerScore).c_str());
    outtextxy(windowWidth/2-10, 30, str);

    /// Print restart/quit message.
    if (bird->isDead) {
        settextstyle(0, 0, 2);
        if (!enterName)
            outtextxy(windowWidth/2-10, windowHeight/2, "[R] to Restart, [Q] to Quit.");

        /// Display new high score.
        if (newHighscore) {
            outtextxy(windowWidth/2-10, windowHeight/2+20, "NEW HIGHSCORE!");
        }

        /// Enter name.
        if (enterName) {
            outtextxy(windowWidth/2-10, windowHeight/2+40, "Please enter your name.");
            char tempHighscoreName[playerName.length()];
            for (int i=0; i<playerName.size(); i++) {
                tempHighscoreName[i] = playerName.at(i);
            }
            outtextxy(windowWidth/2-10, windowHeight/2+80, tempHighscoreName);
        }
    }

    /// Start screen.
    if (!isGameStarted) {
        settextstyle(0, 0, 2);
        outtextxy(windowWidth/2-10, windowHeight/2+60, "Press [Space] to play!");

        string highscoreMessage = "Highest score: " + to_string(highscore);

        if (highscore != 0 && highscoreName != "") {
            highscoreMessage =  "Highest score: " + to_string(highscore) + ",";
        }
        strcpy(str, highscoreMessage.c_str());
        outtextxy(windowWidth/2-10, windowHeight/2+80, str);
        if (highscore != 0 && highscoreName != "") {
            strcpy(str, ("by " + highscoreName).c_str());
            outtextxy(windowWidth/2-10, windowHeight/2+100, str);
        }
    }

    outtext(" "); /// The outtextxy above won't render unless this is here.

}

/**
 * Main function.
 */ 
int main() {

    /// Random seed.
    srand(time(NULL));

    /// Initialize Window.
    initwindow(windowWidth, windowHeight);

    /// Create the bird object.
    bird = new Bird();

    /// Read game data.
    readGameData();

    while (!isGameOver) {
        input();
        update();
        render();

        Sleep(fps);
    }

    closegraph();

    return 1;

}
