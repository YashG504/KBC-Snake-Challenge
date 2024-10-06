#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <vector>

using namespace std;

bool gameOver;
const int width = 20;
const int height = 20;
int x, y, fruitX, fruitY, score;
int tailX[100], tailY[100];
int nTail;
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirection dir;

struct Question {
    string question;
    vector<string> options;
    int correctOption;
};

vector<Question> kbcQuestions = {
    {"What is the capital of France?", {"1. Berlin", "2. Madrid", "3. Paris", "4. Rome"}, 3},
    {"Which planet is known as the Red Planet?", {"1. Earth", "2. Mars", "3. Jupiter", "4. Venus"}, 2},
    {"What is the largest ocean on Earth?", {"1. Atlantic Ocean", "2. Indian Ocean", "3. Arctic Ocean", "4. Pacific Ocean"}, 4},
    {"Which element has the chemical symbol O?", {"1. Gold", "2. Oxygen", "3. Osmium", "4. Hydrogen"}, 2},
    {"Who wrote 'Romeo and Juliet'?", {"1. Charles Dickens", "2. Mark Twain", "3. William Shakespeare", "4. Jane Austen"}, 3},
    {"What is the smallest prime number?", {"1. 0", "2. 1", "3. 2", "4. 3"}, 3}
};

void Setup() {
    gameOver = false;
    dir = STOP;
    x = width / 2;
    y = height / 2;
    fruitX = rand() % width;
    fruitY = rand() % height;
    score = 0;
}

void Draw() {
    system("clear"); // Clear console
    for (int i = 0; i < width + 2; i++)  // Top border
        cout << "#";
    cout << endl;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0)
                cout << "#";  // Left border

            if (i == y && j == x)
                cout << "O";  // Snake head
            else if (i == fruitY && j == fruitX)
                cout << "F";  // Fruit
            else {
                bool printTail = false;
                for (int k = 0; k < nTail; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        cout << "o";  // Snake tail
                        printTail = true;
                    }
                }
                if (!printTail)
                    cout << " ";
            }

            if (j == width - 1)
                cout << "#";  // Right border
        }
        cout << endl;
    }

    for (int i = 0; i < width + 2; i++)  // Bottom border
        cout << "#";
    cout << endl;
    cout << "Score: " << score << endl;
}

bool kbhit() {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // Set raw mode
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    char c;
    int result = read(STDIN_FILENO, &c, 1);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restore old settings
    fcntl(STDIN_FILENO, F_SETFL, oldf); // Restore old flags
    return (result == 1);
}

char getch() {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // Set raw mode
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    char c;
    read(STDIN_FILENO, &c, 1);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restore old settings
    return c;
}

void Input() {
    if (kbhit()) {
        switch (getch()) {
        case 'a':
            dir = LEFT;
            break;
        case 'd':
            dir = RIGHT;
            break;
        case 'w':
            dir = UP;
            break;
        case 's':
            dir = DOWN;
            break;
        case 'x':
            gameOver = true;
            break;
        }
    }
}

void AskKBCQuestion() {
    int questionIndex = rand() % kbcQuestions.size(); // Randomly select a question
    cout << "KBC Question: " << kbcQuestions[questionIndex].question << endl;
    for (const auto& option : kbcQuestions[questionIndex].options) {
        cout << option << endl;
    }
    cout << "Your answer (1-4): ";
    int answer;
    cin >> answer;

    if (answer == kbcQuestions[questionIndex].correctOption) {
        cout << "Correct! Continue your game." << endl;
    } else {
        cout << "Wrong! Going back." << endl;
        // Logic to go back (e.g., reduce score or lose a life)
        score -= 5; // Deduct points as a penalty
        if (score < 0) score = 0; // Ensure score doesn't go negative
    }
}

void Logic() {
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;
    for (int i = 1; i < nTail; i++) {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }
    switch (dir) {
    case LEFT:
        x--;
        break;
    case RIGHT:
        x++;
        break;
    case UP:
        y--;
        break;
    case DOWN:
        y++;
        break;
    default:
        break;
    }

    // Wall collision
    if (x >= width) x = 0; else if (x < 0) x = width - 1;
    if (y >= height) y = 0; else if (y < 0) y = height - 1;

    // Tail collision
    for (int i = 0; i < nTail; i++) {
        if (tailX[i] == x && tailY[i] == y)
            gameOver = true;
    }

    // Fruit collision
    if (x == fruitX && y == fruitY) {
        score += 10;
        fruitX = rand() % width;
        fruitY = rand() % height;
        nTail++;
        AskKBCQuestion(); // Ask a KBC question
    }
}

int main() {
    Setup();
    while (!gameOver) {
        Draw();
        Input();
        Logic();
        usleep(100000);  // Sleep for 0.1 second (100 milliseconds)
    }
    return 0;
}
